/*
 * Copyright (C) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dlfcn.h>
#include <pthread.h>

/*
 * AudioSystem::get_audio_flinger() / get_audio_policy_service()
 * ABI adapter (fix 10).
 *
 * Android 5 returns these as `const sp<T>&`: a pointer to the
 * static strong pointer, returned in r0. Android 11 changed both
 * to return `sp<T>` by value, which on ARM uses a hidden sret
 * pointer in r0, but kept the same mangled names. The frozen A5
 * libSDtvAout.so calls them the A5 way: it passes no sret pointer
 * (r0 still holds AudioTrack::start()'s 0 return) and then
 * dereferences the returned pointer once to reach the interface
 * vtable. The A11 function stores 0 to [r0] first thing, so
 * SDtvService dies with SIGSEGV fault 0x0 in
 * AudioSystem::get_audio_flinger()+36 (tombstone_13, SDtvThAudio,
 * on the first audio write after the track starts; the call site
 * is SDTV_WriteAudio's down-mix tail at libSDtvAout.so+0x1e9a).
 * The same trap waits at the two later call sites in
 * SDTV_WriteAudio (a second get_audio_flinger call at +0x2004
 * and a get_audio_policy_service call at +0x2042), so both
 * symbols are interposed here.
 *
 * Each interposed function returns a pointer to its own static
 * slot holding the interface pointer, which is exactly what the
 * A5 caller dereferences. The real A11 function is called once
 * through a raw `void (*)(void*)` pointer, which is its exact
 * machine-level form (r0 = sret out-pointer, returns void); it
 * writes the 4-byte strong pointer into the slot and bumps the
 * refcount. The reference is never released: both services are
 * process-lifetime singletons.
 */

namespace android {
class AudioSystem {
public:
    // A5 ABI: returns const sp<IAudioFlinger>& /
    // const sp<IAudioPolicyService>&, i.e. a pointer to the
    // static strong pointer. The return type is not part of the
    // mangled name, so `const void*` mangles to the same symbols
    // as the A11 by-value versions.
    static const void* get_audio_flinger();
    static const void* get_audio_policy_service();
};
}  // namespace android

namespace {

pthread_mutex_t gAudioSystemLock = PTHREAD_MUTEX_INITIALIZER;
void* gAudioFlinger = nullptr;
void* gAudioPolicyService = nullptr;

// Machine-level form of the A11 by-value getters:
// `sp<T> get_audio_flinger()` is `void get_audio_flinger(void* sret)`.
typedef void (*GetServiceRawFn)(void* sretOut);

void* getRealAudioSystemFn(const char* symbol) {
    static void* handle = nullptr;
    if (handle == nullptr) {
        handle = dlopen("libaudioclient.so", RTLD_NOW | RTLD_NOLOAD);
        if (handle == nullptr) {
            handle = dlopen("libaudioclient.so", RTLD_NOW);
        }
    }
    return handle != nullptr ? dlsym(handle, symbol) : nullptr;
}

// Fill *cache with the interface pointer from the real A11
// getter. A null result is not cached, so a later call retries.
void* getCachedService(void** cache, const char* symbol) {
    pthread_mutex_lock(&gAudioSystemLock);
    if (*cache == nullptr) {
        GetServiceRawFn realFn = reinterpret_cast<GetServiceRawFn>(
                getRealAudioSystemFn(symbol));
        if (realFn != nullptr) {
            void* slot = nullptr;
            realFn(&slot);
            *cache = slot;
        }
    }
    void* result = *cache;
    pthread_mutex_unlock(&gAudioSystemLock);
    return result;
}

}  // namespace

const void* android::AudioSystem::get_audio_flinger() {
    getCachedService(&gAudioFlinger,
                     "_ZN7android11AudioSystem17get_audio_flingerEv");
    // Pointer to the slot itself: the A5 caller dereferences it
    // once to reach the interface pointer.
    return &gAudioFlinger;
}

const void* android::AudioSystem::get_audio_policy_service() {
    getCachedService(&gAudioPolicyService,
                     "_ZN7android11AudioSystem24get_audio_policy_serviceEv");
    return &gAudioPolicyService;
}
