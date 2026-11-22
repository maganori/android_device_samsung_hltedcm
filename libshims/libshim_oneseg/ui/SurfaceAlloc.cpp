/*
 * Interpose operator new for the frozen libSDtvVout.so (Android 5.0 binary).
 *
 * Its SetNativeWindow does `new android::Surface(producer, false)` with the
 * Android 5.0 sizeof(Surface) == 1784 baked in as the operator-new size,
 * but on Android 11 sizeof(Surface) is ~6160 (64 slots x 88 bytes,
 * mRemovedBuffers at offset 0x1804). The Android 11 constructor therefore
 * writes ~4.4 KB past the 1784-byte allocation, corrupting the heap.
 * Destroying the old Surface on the 2nd SET_DISPLAY then faults in
 * Surface::disconnect -> mRemovedBuffers.clear() -> RefBase::decStrong
 * (mRefs == 0, ldrex [0], tombstone_49, 2026-11-22 17:48:52).
 *
 * Grow exactly-1784 allocations to 8192 bytes. Over-allocation is harmless
 * for any other 1784-byte allocation in the process; the frozen binary
 * itself is untouched.
 */
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>

#include <android/log.h>

#define LOG_TAG "oneseg_shim"

namespace {
typedef void* (*NewFn)(unsigned int);
NewFn g_real_new = nullptr;
bool g_resolving = false;
bool g_logged = false;
}  // namespace

// operator new(unsigned int), mangled name _Znwj, C linkage so the symbol
// name stays exactly _Znwj and interposes the frozen lib's reference.
extern "C" void* _Znwj(unsigned int size) {
    if (size == 1784) {
        if (!g_logged) {
            g_logged = true;
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG,
                                "oneseg_shim: growing Surface allocation 1784 -> 8192");
        }
        size = 8192;
    }
    if (g_real_new == nullptr) {
        if (g_resolving) {
            // Re-entered while resolving (dlsym must not call operator new,
            // but be safe): fall back to malloc.
            return malloc(size);
        }
        g_resolving = true;
        g_real_new = reinterpret_cast<NewFn>(dlsym(RTLD_NEXT, "_Znwj"));
        g_resolving = false;
        if (g_real_new == nullptr) {
            return malloc(size);
        }
    }
    return g_real_new(size);
}
