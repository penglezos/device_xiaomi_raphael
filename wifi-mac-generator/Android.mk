LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE       := wifi-mac-generator
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_EXECUTABLES)
LOCAL_SRC_FILES    := wifi-mac-generator.sh
LOCAL_INIT_RC      := wifi-mac-generator.rc
include $(BUILD_PREBUILT)
