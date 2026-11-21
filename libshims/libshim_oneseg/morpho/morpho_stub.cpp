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
 * export exactly the symbols libSDtvStack imports and report
 * failure, so the stack runs with the feature unavailable
 * instead of not running at all.
 */
extern "C" {

int morpho_FrameInterp_initialize(void) { return 1; }
int morpho_FrameInterp_finalize(void) { return 0; }
int morpho_FrameInterp_start(void) { return 1; }
int morpho_FrameInterp_process(void) { return 1; }
int morpho_FrameInterp_addImage(void) { return 1; }
int morpho_FrameInterp_getInterpolationImage(void) { return 1; }
int morpho_FrameInterp_getBufferSize(void) { return 0; }
int morpho_FrameInterp_getVersion(void) { return 0; }
int morpho_FrameInterp_setCancelThreshold(void) { return 0; }
int morpho_FrameInterp_setImageFormat(void) { return 0; }
int morpho_FrameInterp_setStageNum(void) { return 0; }

}  // extern "C"
