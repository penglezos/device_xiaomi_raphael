# Copyright 2013 The Android Open Source Project

LOCAL_PATH := $(call my-dir)

### pixel_charger_res_images ###
ifneq ($(strip $(LOCAL_CHARGER_NO_UI)),true)
define _add-product-charger-image
include $$(CLEAR_VARS)
LOCAL_MODULE := pixel_charger_res_images_charger_$(notdir $(1))
LOCAL_MODULE_STEM := $(notdir $(1))
_img_modules += $$(LOCAL_MODULE)
LOCAL_SRC_FILES := $1
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $$(TARGET_OUT_PRODUCT)/etc/res/images/charger
LOCAL_PRODUCT_MODULE := true
include $$(BUILD_PREBUILT)
endef

_img_modules :=
_images :=
$(foreach _img, $(call find-subdir-subdir-files, "res/images/charger", "*.png"), \
  $(eval $(call _add-product-charger-image,$(_img))))

### pixel_charger_animation_file ###
define _add-product-charger-animation-file
include $$(CLEAR_VARS)
LOCAL_MODULE := pixel_charger_res_values_charger_$(notdir $(1))
LOCAL_MODULE_STEM := $(notdir $(1))
_anim_modules += $$(LOCAL_MODULE)
LOCAL_SRC_FILES := $1
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $$(TARGET_OUT_PRODUCT)/etc/res/values/charger
LOCAL_PRODUCT_MODULE := true
include $$(BUILD_PREBUILT)
endef

_anim_modules :=
$(foreach _txt, $(call find-subdir-subdir-files, "res/images/charger", "*.txt"), \
  $(eval $(call _add-product-charger-animation-file,$(_txt))))

include $(CLEAR_VARS)
LOCAL_MODULE := product_charger_res_images
LOCAL_MODULE_TAGS := optional
LOCAL_REQUIRED_MODULES := $(_img_modules) $(_anim_modules)
include $(BUILD_PHONY_PACKAGE)

_add-product-charger-image :=
_add-product-charger-animation-file :=
_img_modules :=
_anim_modules :=
endif # LOCAL_CHARGER_NO_UI
