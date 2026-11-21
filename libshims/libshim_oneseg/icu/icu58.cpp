/*
 * Forwarders from the ICU 58 symbol names the oneseg/mmb blobs were
 * linked against (Android 8.1 era) to the ICU 67 names Android 11
 * ships. The C API signatures of these functions are unchanged
 * between the two versions; only the version suffix appended to
 * every exported symbol differs. All parameters are declared with
 * opaque types on purpose: the argument registers/stack layout is
 * identical, and the shim never inspects the values.
 *
 * Consumers (wired in BoardConfig.mk via TARGET_LD_SHIM_LIBS):
 * libMmbPoXmlMp.so, libMmbStBmlMwPorting.so, libskia_LRX21V.so.
 */

#include <stdint.h>

extern "C" {

/* The Android 11 (ICU 67) targets. */
void ucnv_close_67(void *cnv);
void *ucnv_open_67(const char *name, int *err);
int32_t ucnv_fromUChars_67(void *cnv, char *dest, int32_t destCapacity,
                           const uint16_t *src, int32_t srcLength, int *err);
int32_t ucnv_toUChars_67(void *cnv, uint16_t *dest, int32_t destCapacity,
                         const char *src, int32_t srcLength, int *err);
void ucnv_convertEx_67(const char *targetEncoding, const char *sourceEncoding,
                       char *target, const char *targetLimit,
                       const char *source, const char *sourceLimit,
                       uint16_t *pivotStart, uint16_t **pivotSource,
                       uint16_t **pivotTarget, const char *pivotLimit,
                       int reset, int flush, int *err);
void ucnv_setFromUCallBack_67(void *cnv, void *newAction, const void *newContext,
                              void **oldAction, const void **oldContext, int *err);
void ucnv_setToUCallBack_67(void *cnv, void *newAction, const void *newContext,
                            void **oldAction, const void **oldContext, int *err);
void UCNV_FROM_U_CALLBACK_STOP_67(const void *context, void *args,
                                  const uint16_t *codeUnits, int32_t length,
                                  int32_t codePoint, int reason, int *err);
void UCNV_TO_U_CALLBACK_STOP_67(const void *context, void *args,
                                const char *codeUnits, int32_t length,
                                int reason, int *err);
void *u_memset_67(void *dest, uint16_t c, int32_t count);
uint16_t *u_strcat_67(uint16_t *dest, const uint16_t *src);
uint16_t *u_strcpy_67(uint16_t *dest, const uint16_t *src);
int32_t u_strlen_67(const uint16_t *s);

/* The ICU 58 names the blobs import. */
void ucnv_close_58(void *cnv) { ucnv_close_67(cnv); }

void *ucnv_open_58(const char *name, int *err) {
    return ucnv_open_67(name, err);
}

int32_t ucnv_fromUChars_58(void *cnv, char *dest, int32_t destCapacity,
                           const uint16_t *src, int32_t srcLength, int *err) {
    return ucnv_fromUChars_67(cnv, dest, destCapacity, src, srcLength, err);
}

int32_t ucnv_toUChars_58(void *cnv, uint16_t *dest, int32_t destCapacity,
                         const char *src, int32_t srcLength, int *err) {
    return ucnv_toUChars_67(cnv, dest, destCapacity, src, srcLength, err);
}

void ucnv_convertEx_58(const char *targetEncoding, const char *sourceEncoding,
                       char *target, const char *targetLimit,
                       const char *source, const char *sourceLimit,
                       uint16_t *pivotStart, uint16_t **pivotSource,
                       uint16_t **pivotTarget, const char *pivotLimit,
                       int reset, int flush, int *err) {
    ucnv_convertEx_67(targetEncoding, sourceEncoding, target, targetLimit,
                      source, sourceLimit, pivotStart, pivotSource, pivotTarget,
                      pivotLimit, reset, flush, err);
}

void ucnv_setFromUCallBack_58(void *cnv, void *newAction, const void *newContext,
                              void **oldAction, const void **oldContext, int *err) {
    ucnv_setFromUCallBack_67(cnv, newAction, newContext, oldAction, oldContext, err);
}

void ucnv_setToUCallBack_58(void *cnv, void *newAction, const void *newContext,
                            void **oldAction, const void **oldContext, int *err) {
    ucnv_setToUCallBack_67(cnv, newAction, newContext, oldAction, oldContext, err);
}

void UCNV_FROM_U_CALLBACK_STOP_58(const void *context, void *args,
                                  const uint16_t *codeUnits, int32_t length,
                                  int32_t codePoint, int reason, int *err) {
    UCNV_FROM_U_CALLBACK_STOP_67(context, args, codeUnits, length, codePoint,
                                 reason, err);
}

void UCNV_TO_U_CALLBACK_STOP_58(const void *context, void *args,
                                const char *codeUnits, int32_t length,
                                int reason, int *err) {
    UCNV_TO_U_CALLBACK_STOP_67(context, args, codeUnits, length, reason, err);
}

void *u_memset_58(void *dest, uint16_t c, int32_t count) {
    return u_memset_67(dest, c, count);
}

uint16_t *u_strcat_58(uint16_t *dest, const uint16_t *src) {
    return u_strcat_67(dest, src);
}

uint16_t *u_strcpy_58(uint16_t *dest, const uint16_t *src) {
    return u_strcpy_67(dest, src);
}

int32_t u_strlen_58(const uint16_t *s) { return u_strlen_67(s); }

}  // extern "C"
