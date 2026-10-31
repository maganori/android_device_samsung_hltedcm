/*
 * Shim for the legacy libcutils atomic API (cutils/atomic.h), removed
 * from Android after 5.x and therefore missing in lineage-18.1.
 *
 * Needed by the stock oneseg middleware: verified with readelf that
 * vendor lib libMmbFcMp4MwPlayerService.so has an undefined reference
 * to android_atomic_inc (its only atomic symbol). The full legacy API
 * surface is provided here so other stock binaries can use it too.
 *
 * In the lineage-15.1-oneseg tree, device.mk and BoardConfig.mk
 * referenced libshim_cutils_atomic but no module definition existed
 * anywhere in the device or vendor trees; this file fills that gap.
 * Implementations use GCC/Clang __sync builtins, matching the
 * semantics of the original libcutils implementations.
 */

#include <stdint.h>

int32_t android_atomic_inc(volatile int32_t* addr) {
    return __sync_fetch_and_add((volatile int32_t*)addr, 1);
}

int32_t android_atomic_dec(volatile int32_t* addr) {
    return __sync_fetch_and_sub((volatile int32_t*)addr, 1);
}

int32_t android_atomic_add(int32_t value, volatile int32_t* addr) {
    return __sync_fetch_and_add((volatile int32_t*)addr, value);
}

int32_t android_atomic_and(int32_t value, volatile int32_t* addr) {
    return __sync_fetch_and_and((volatile int32_t*)addr, value);
}

int32_t android_atomic_or(int32_t value, volatile int32_t* addr) {
    return __sync_fetch_and_or((volatile int32_t*)addr, value);
}

int32_t android_atomic_swap(int32_t value, volatile int32_t* addr) {
    int32_t old;
    do {
        old = *addr;
    } while (__sync_val_compare_and_swap((volatile int32_t*)addr, old, value) != old);
    return old;
}

int android_atomic_cmpxchg(int32_t oldvalue, int32_t newvalue, volatile int32_t* addr) {
    return (int)(__sync_val_compare_and_swap((volatile int32_t*)addr, oldvalue, newvalue) != oldvalue);
}

int32_t android_atomic_release_load(volatile const int32_t* addr) {
    __sync_synchronize();
    return *addr;
}

void android_atomic_acquire_store(int32_t value, volatile int32_t* addr) {
    *addr = value;
    __sync_synchronize();
}

void android_atomic_release_store(int32_t value, volatile int32_t* addr) {
    __sync_synchronize();
    *addr = value;
}
