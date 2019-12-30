#
# Copyright 2019 The Paranoid Android Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

$(call inherit-product, $(SRC_TARGET_DIR)/product/product_launched_with_p.mk)

# Setup dalvik vm configs
$(call inherit-product, frameworks/native/build/phone-xhdpi-6144-dalvik-heap.mk)

# Get non-open-source specific aspects
$(call inherit-product-if-exists, vendor/xiaomi/cepheus/cepheus-vendor.mk)

# Overlays
DEVICE_PACKAGE_OVERLAYS += \
    $(LOCAL_PATH)/overlay \
    $(LOCAL_PATH)/overlay-pa

# Properties
-include $(LOCAL_PATH)/system_prop.mk

# Device uses high-density artwork where available
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xxhdpi

# AID/fs configs
PRODUCT_PACKAGES += \
    fs_config_files

# Audio
PRODUCT_PACKAGES += \
    android.hardware.audio@5.0 \
    android.hardware.audio.common@5.0 \
    android.hardware.audio.common@5.0-util \
    android.hardware.audio.effect@5.0 \
    libaudio-resampler

# Bluetooth
PRODUCT_PACKAGES += \
    libbluetooth_qti \
    libbt-logClient.so

# Display
PRODUCT_PACKAGES += \
    libdisplayconfig \
    libqdMetaData.system

# Fingerprint
PRODUCT_PACKAGES += \
    pa.biometrics.fingerprint.inscreen@1.0-service.xiaomi_cepheus

PRODUCT_COPY_FILES += \
    vendor/pa/config/permissions/vendor.pa.biometrics.fingerprint.inscreen.xml:system/etc/permissions/vendor.pa.biometrics.fingerprint.inscreen.xml

# HIDL
PRODUCT_PACKAGES += \
    android.hidl.base@1.0 \
    android.hidl.manager@1.0

# Hotword Enrollment
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/hotword-hiddenapi-package-whitelist.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/sysconfig/hotword-hiddenapi-package-whitelist.xml \
    $(LOCAL_PATH)/permissions/privapp-permissions-hotword.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/privapp-permissions-hotword.xml

# Init
PRODUCT_PACKAGES += \
    init.mi_thermald.rc \
    init.qcom.rc

# IR
PRODUCT_PACKAGES += \
    android.hardware.ir@1.0-impl \
    android.hardware.ir@1.0-service

# Keylayouts
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/keylayout/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl

# Lights
PRODUCT_PACKAGES += \
    android.hardware.light@2.0-service.xiaomi_msmnile

# NFC
PRODUCT_PACKAGES += \
    com.android.nfc_extras \
    com.gsma.services.nfc \
    com.nxp.nfc.nq \
    libnqnfc-nci \
    nqnfcee_access.xml \
    nqnfcse_access.xml \
    NfcNci \
    SecureElement \
    Tag

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/nfc/libnfc-nci.conf:$(TARGET_COPY_OUT_SYSTEM)/etc/libnfc-nci.conf \

# NN
PRODUCT_PACKAGES += \
    libprotobuf-cpp-full-rtti

# Paranoid Doze
PRODUCT_PACKAGES += ParanoidDoze

# Perf
-include vendor/qcom/common/av/qti-av.mk
-include vendor/qcom/common/bt/qti-bt.mk
-include vendor/qcom/common/perf/qti-perf.mk

# Power
PRODUCT_PACKAGES += \
    android.hardware.power@1.2-service

# QCOM
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/privapp-permissions-qti.xml:system/etc/permissions/privapp-permissions-qti.xml \
    $(LOCAL_PATH)/configs/qti_whitelist.xml:system/etc/sysconfig/qti_whitelist.xml

# Recovery
PRODUCT_PACKAGES += \
    librecovery_updater_xiaomi

# VNDK
PRODUCT_TARGET_VNDK_VERSION := 29
