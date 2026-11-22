LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_oneseg_video
LOCAL_SRC_FILES := ui/GraphicBuffer.cpp ui/SurfaceAlloc.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libui liblog libdl
# DF_1_GLOBAL: libSDtvVout.so is BIND_NOW, so its operator-new PLT entry
# is bound eagerly from the load-group lookup list (global group first,
# then BFS local group). libstdc++.so (depth 1, direct dep of the
# SDtvService executable) otherwise wins over this shim (child of
# libSDtvVout, depth 2+). Being in the global group puts the shim first.
LOCAL_LDFLAGS := -Wl,-z,global
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_oneseg_audio
LOCAL_SRC_FILES := media/AudioTrack.cpp media/AudioSystem.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libaudioclient libdl
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
# Android 11 has no shared libskia to forward to; the shim is now a
# self-contained stub of the legacy symbols the blobs import.
LOCAL_MODULE := libshim_oneseg_skia
LOCAL_SRC_FILES := skia/skia_stub.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_oneseg_utils
LOCAL_SRC_FILES := utils/String8.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_oneseg_binder
LOCAL_SRC_FILES := \
    binder/Parcel.cpp \
    binder/BinderBridge.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder libutils liblog libdl
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_oneseg_jpeg
LOCAL_SRC_FILES := \
    jpeg/jdhuff.c \
    jpeg/jdapistd.c \
    jpeg/jdtrans.c \
    jpeg/jdinput.c
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libjpeg
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_oneseg_png
LOCAL_SRC_FILES := \
    png/pngrio.c \
    png/pngread.c \
    png/pngrutil.c
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libpng libz
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_oneseg_jvm
LOCAL_SRC_FILES := jni/jvm.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libmorpho_frame_interp2
LOCAL_SRC_FILES := morpho/morpho_stub.cpp
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
