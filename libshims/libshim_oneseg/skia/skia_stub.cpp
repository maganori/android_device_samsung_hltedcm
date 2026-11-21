/*
 * Stub implementations of the legacy (LRX21V-era) Skia symbols that
 * the oneseg blobs import.
 *
 * The original libshim_oneseg_skia forwarded to the system libskia.so,
 * but Android 11 no longer builds libskia as a shared library, so
 * there is nothing left to forward to. Nine oneseg/mmb blobs link
 * Skia via DT_NEEDED libskia.so: libBMLJNI, libSDtvService,
 * libSDtvStack, libbroadcastForOneSeg_jni, libMmbStBmlMw,
 * libMmbStBmlMwPorting, libMmbStCtlMwPorting, libMmbMfMp4FwMw and
 * libmmbfcmp4mw_jni. Their DT_NEEDED entries are patched to this
 * library in the vendor repo (patchelf --replace-needed), so this
 * stub now serves all of them. None of the consumers is on the
 * live-viewing decode path in a way that needs real pixels (BML
 * data-broadcast rendering and MP4 thumbnails/metadata), so these
 * bodies intentionally do nothing and return zero/nullptr: the
 * callers keep working, only the Skia-drawn content is lost.
 *
 * The class declarations below exist only to make the compiler emit
 * the exact mangled symbol names the blobs import. The objects live
 * in the callers' own storage (sized by the legacy headers), so the
 * constructors and destructors must not touch any member: they are
 * empty on purpose.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef float SkScalar;
typedef uint32_t SkColor;

enum SkColorType {
    kUnknown_SkColorType,
    kAlpha_8_SkColorType,
    kRGB_565_SkColorType,
    kARGB_4444_SkColorType,
    kRGBA_8888_SkColorType,
    kBGRA_8888_SkColorType,
};

enum SkAlphaType {
    kIgnore_SkAlphaType,
    kOpaque_SkAlphaType,
    kPremul_SkAlphaType,
    kUnpremul_SkAlphaType,
};

struct SkRect {
    float fLeft, fTop, fRight, fBottom;
};

class SkImageInfo;
class SkColorTable;

class SkRegion {
public:
    enum Op {
        kDifference_Op,
        kIntersect_Op,
        kUnion_Op,
        kXOR_Op,
        kReverseDifference_Op,
        kReplace_Op,
    };
};

class SkTypeface {
public:
    enum Style {
        kNormal = 0,
        kBold = 1,
        kItalic = 2,
        kBoldItalic = 3,
    };
    static SkTypeface* CreateFromFile(const char*) { return nullptr; }
    static SkTypeface* CreateFromName(const char*, Style) { return nullptr; }
    static SkTypeface* CreateFromTypeface(const SkTypeface*, Style) { return nullptr; }
};

class SkPaintOptionsAndroid {
public:
    void setLanguage(const char*) {}
};

class SkPaint {
public:
    enum Align {
        kLeft_Align,
        kCenter_Align,
        kRight_Align,
    };
    enum Style {
        kFill_Style,
        kStroke_Style,
        kStrokeAndFill_Style,
    };
    enum FilterLevel {
        kNone_FilterLevel,
        kLow_FilterLevel,
        kMedium_FilterLevel,
        kHigh_FilterLevel,
    };
    enum TextEncoding {
        kUTF8_TextEncoding,
        kUTF16_TextEncoding,
        kUTF32_TextEncoding,
        kGlyphID_TextEncoding,
    };
    /* Legacy (LRX21V) layout of SkPaint::FontMetrics. Callers own the
     * storage; only the first 40 bytes are cleared, which is the size
     * of the struct in the headers the blobs were built against. */
    struct FontMetrics {
        uint32_t fFlags;
        float fTop, fAscent, fDescent, fBottom, fLeading;
        float fAvgCharWidth, fXMin, fXMax, fXHeight;
    };

    SkPaint() {}
    SkPaint(const SkPaint&) {}
    ~SkPaint() {}

    void setColor(SkColor) {}
    void setARGB(uint32_t, uint32_t, uint32_t, uint32_t) {}
    void setStyle(Style) {}
    void setStrokeWidth(SkScalar) {}
    void setAntiAlias(bool) {}
    void setTextSize(SkScalar) {}
    void setTextAlign(Align) {}
    void setTextScaleX(SkScalar) {}
    void setTypeface(SkTypeface*) {}
    void setFilterLevel(FilterLevel) {}
    void setFakeBoldText(bool) {}
    void setTextEncoding(TextEncoding) {}
    void setPaintOptionsAndroid(const SkPaintOptionsAndroid&) {}

    SkScalar measureText(const void*, size_t, SkRect*, SkScalar) const { return 0; }
    SkScalar getFontMetrics(FontMetrics* metrics, SkScalar) const {
        if (metrics != nullptr) {
            memset(metrics, 0, sizeof(*metrics));
        }
        return 0;
    }
};

class SkBitmap {
public:
    enum Config {
        kNo_Config,
        kA1_Config,
        kA8_Config,
        kIndex8_Config,
        kRGB_565_Config,
        kARGB_4444_Config,
        kARGB_8888_Config,
    };
    class Allocator {};

    SkBitmap() {}
    ~SkBitmap() {}

    void reset() {}
    bool setInfo(const SkImageInfo&, size_t) { return true; }
    void setConfig(Config, int, int, size_t, SkAlphaType) {}
    Config config() const { return kNo_Config; }
    void lockPixels() const {}
    void unlockPixels() const {}
    bool allocPixels(Allocator*, SkColorTable*) { return true; }
    void setPixels(void*, SkColorTable*) {}
    bool copyTo(SkBitmap*, SkColorType, Allocator*) const { return false; }
};

SkColorType SkBitmapConfigToColorType(SkBitmap::Config) {
    return kUnknown_SkColorType;
}

class SkXfermode {
public:
    enum Mode {
        kClear_Mode,
        kSrc_Mode,
        kDst_Mode,
        kSrcOver_Mode,
        kDstOver_Mode,
        kSrcIn_Mode,
        kDstIn_Mode,
        kSrcOut_Mode,
        kDstOut_Mode,
        kSrcATop_Mode,
        kDstATop_Mode,
        kXor_Mode,
    };
};

class SkCanvas {
public:
    SkCanvas(const SkBitmap&) {}
    ~SkCanvas() {}

    int save() { return 0; }
    void restore() {}
    bool clipRect(const SkRect&, SkRegion::Op, bool) { return true; }
    void drawColor(SkColor, SkXfermode::Mode) {}
    void drawARGB(uint32_t, uint32_t, uint32_t, uint32_t, SkXfermode::Mode) {}
    void drawPoint(SkScalar, SkScalar, SkColor) {}
    void drawLine(SkScalar, SkScalar, SkScalar, SkScalar, const SkPaint&) {}
    void drawText(const void*, size_t, SkScalar, SkScalar, const SkPaint&) {}
};

class SkImageDecoder {
public:
    enum Mode {
        kDecodeBounds_Mode,
        kDecodePixels_Mode,
    };
    enum Format {
        kUnknown_Format,
        kBMP_Format,
        kGIF_Format,
        kICO_Format,
        kJPEG_Format,
        kPNG_Format,
        kWBMP_Format,
        kWEBP_Format,
    };
    static bool DecodeFile(const char*, SkBitmap*, SkColorType, Mode, Format*) { return false; }
};
