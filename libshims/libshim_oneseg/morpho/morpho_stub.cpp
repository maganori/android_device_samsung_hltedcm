/*
 * Stub for libmorpho_frame_interp2.so.
 *
 * The stock library has text relocations, and Android 11's
 * linker refuses it for every process at platform API level
 * (init services included - verified on device: SDtvService
 * exits with the TEXTREL error even when started by init).
 * That single library blocks the whole SDtvService daemon,
 * which the viewer needs for ISDtvService.SDtvStackService.
 *
 * Frame interpolation is an optional picture enhancement
 * inside libSDtvStack (the only consumer). These stubs
 * export exactly the symbols libSDtvStack imports.
 *
 * Return contract, reverse-engineered from libSDtvStack.so
 * (SDTV_FI_Init / SDTV_FI_SetDecodedFrame /
 * SDTV_FI_GetInterpolatedFrame): 0 means success everywhere
 * the stack checks the result, and the stack treats a
 * nonzero initialize as fatal: SDTV_FI_Init returns -1 and
 * the SDDIfVideo thread then enters its message loop with a
 * NULL queue pointer, dying with SIGSEGV fault 0x4 on the
 * first video message (tombstone_15). So the stubs must
 * report success for the init path (initialize / getBufferSize
 * / setImageFormat / setCancelThreshold / setStageNum) and for
 * the per-frame path (start / addImage / process /
 * getInterpolationImage). process() takes (handle, int* ready)
 * and must store 1 to *ready, or SetDecodedFrame spins
 * calling it forever. No real interpolation happens: with
 * these values the stack runs with the feature effectively
 * unavailable but keeps playing the decoded frames.
 */
extern "C" {

int morpho_FrameInterp_initialize(void) { return 0; }
int morpho_FrameInterp_finalize(void) { return 0; }
int morpho_FrameInterp_start(void) { return 0; }
int morpho_FrameInterp_process(void* handle, int* outReady) {
    (void)handle;
    if (outReady != nullptr) {
        *outReady = 1;
    }
    return 0;
}
int morpho_FrameInterp_addImage(void) { return 0; }
int morpho_FrameInterp_getInterpolationImage(void) { return 0; }
/* One YUV420 frame at the 384x192 size FI_Init asks for. The
 * value only sizes a working buffer the stub never touches. */
int morpho_FrameInterp_getBufferSize(void) { return 384 * 192 * 3 / 2; }
int morpho_FrameInterp_getVersion(void) { return 0; }
int morpho_FrameInterp_setCancelThreshold(void) { return 0; }
int morpho_FrameInterp_setImageFormat(void) { return 0; }
int morpho_FrameInterp_setStageNum(void) { return 0; }

}  // extern "C"
