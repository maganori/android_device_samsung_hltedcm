LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_oneseg_video
LOCAL_SRC_FILES := ui/GraphicBuffer.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libui
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_oneseg_audio
LOCAL_SRC_FILES := media/AudioTrack.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libaudioclient
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
# Android 11 has no shared libskia to forward to; the shim is now a
# self-contained stub of the legacy symbols the blobs import.
LOCAL_MODULE := libshim_oneseg_skia
# Installed as libskia.so: Android 11 has no shared libskia, and nine
# oneseg/mmb blobs DT_NEEDED it. The module name stays
# libshim_oneseg_skia (a module named libskia would collide with the
# Soong libskia in external/skia); only the installed file name
# changes, via the stem.
LOCAL_MODULE_STEM := libskia
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
LOCAL_SRC_FILES := binder/Parcel.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder
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
