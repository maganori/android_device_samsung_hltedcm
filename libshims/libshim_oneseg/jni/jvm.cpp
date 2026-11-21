/*
 * JNI_GetCreatedJavaVMs for the mmb JNI libraries
 * (libMmbStBmlMwService_jni, libMmbStCtlMwService_jni,
 * libMmbStRecMwService_jni).
 *
 * They DT_NEEDED libart.so, which a vendor process cannot load on
 * Android 11, so the NEEDED entry is removed in the vendor repo
 * and this shim supplies the one VM symbol they import. A native
 * middleware service has no Java VM to find: report success with
 * zero VMs, which is the honest answer ART itself gives when no
 * VM has been created in the process. Callers take their no-VM
 * path from there.
 */

#include <stdint.h>

typedef int32_t jint;
typedef jint jsize;

struct JavaVM;

extern "C" jint JNI_GetCreatedJavaVMs(JavaVM** vmBuf, jsize bufLen, jsize* nVMs) {
    (void)vmBuf;
    (void)bufLen;
    if (nVMs != nullptr) {
        *nVMs = 0;
    }
    return 0;  // JNI_OK
}
