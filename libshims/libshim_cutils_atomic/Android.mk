LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_cutils_atomic
LOCAL_SRC_FILES := cutils_atomic.c
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
