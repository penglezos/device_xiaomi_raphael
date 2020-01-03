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
    android.hardware.audio@2.0-service \
    android.hardware.audio@5.0-impl \
    android.hardware.audio.effect@5.0-impl \
    android.hardware.bluetooth.audio@2.0-impl \
    android.hardware.soundtrigger@2.1-impl

PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio.bluetooth.default \
    audio.usb.default \
    audio.r_submix.default

PRODUCT_PACKAGES += \
    libvolumelistener \
    libqcomvisualizer \
    libqcomvoiceprocessing \
    libqcomvoiceprocessingdescriptors \
    libqcompostprocbundle

PRODUCT_PACKAGES += \
    tinymix \
    libaudio-resampler

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/audio/audio_configs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_configs.xml \
    $(LOCAL_PATH)/audio/audio_configs_stock.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_configs_stock.xml \
    $(LOCAL_PATH)/audio/audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_configuration.xml \
    $(LOCAL_PATH)/audio/audio_effects.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_effects.xml \
    $(LOCAL_PATH)/audio/audio_io_policy.conf:$(TARGET_COPY_OUT_VENDOR)/etc/audio_io_policy.conf \
    $(LOCAL_PATH)/audio/audio_platform_info.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_platform_info.xml \
    $(LOCAL_PATH)/audio/audio_policy_volumes.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
    $(LOCAL_PATH)/audio/audio_tuning_mixer_tavil.txt:$(TARGET_COPY_OUT_VENDOR)/etc/audio_tuning_mixer_tavil.txt

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/audio/graphite_ipc_platform_info.xml:$(TARGET_COPY_OUT_VENDOR)/etc/graphite_ipc_platform_info.xml \
    $(LOCAL_PATH)/audio/sound_trigger_mixer_paths_wcd9340.xml:$(TARGET_COPY_OUT_VENDOR)/etc/sound_trigger_mixer_paths_wcd9340.xml \
    $(LOCAL_PATH)/audio/sound_trigger_platform_info.xml:$(TARGET_COPY_OUT_VENDOR)/etc/sound_trigger_platform_info.xml

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/audio/mixer_paths_overlay_dynamic.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mixer_paths_overlay_dynamic.xml \
    $(LOCAL_PATH)/audio/mixer_paths_overlay_static.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mixer_paths_overlay_static.xml \
    $(LOCAL_PATH)/audio/mixer_paths_tavil.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mixer_paths_tavil.xml

PRODUCT_COPY_FILES += \
    frameworks/av/services/audiopolicy/config/a2dp_in_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/a2dp_in_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/usb_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.audio.low_latency.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.low_latency.xml \
    frameworks/native/data/etc/android.hardware.audio.pro.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.pro.xml \
    frameworks/native/data/etc/android.software.midi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.midi.xml

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

# GPS
PRODUCT_PACKAGES += \
    libsensorndkbridge

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/gps/apdr.conf:$(TARGET_COPY_OUT_VENDOR)/etc/apdr.conf \
    $(LOCAL_PATH)/gps/flp.conf:$(TARGET_COPY_OUT_VENDOR)/etc/flp.conf \
    $(LOCAL_PATH)/gps/gps.conf:$(TARGET_COPY_OUT_VENDOR)/etc/gps.conf \
    $(LOCAL_PATH)/gps/izat.conf:$(TARGET_COPY_OUT_VENDOR)/etc/izat.conf \
    $(LOCAL_PATH)/gps/lowi.conf:$(TARGET_COPY_OUT_VENDOR)/etc/lowi.conf \
    $(LOCAL_PATH)/gps/sap.conf:$(TARGET_COPY_OUT_VENDOR)/etc/sap.conf \
    $(LOCAL_PATH)/gps/xtwifi.conf:$(TARGET_COPY_OUT_VENDOR)/etc/xtwifi.conf

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml

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
    init.qcom.post_boot.sh \
    init.qcom.rc \
    init.qcom.sh \
    init.qcom.usb.rc \
    init.target.rc \
    init.panel_info.sh

# IR
PRODUCT_PACKAGES += \
    android.hardware.ir@1.0-impl \
    android.hardware.ir@1.0-service

# IRSC
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/sec_config:$(TARGET_COPY_OUT_VENDOR)/etc/sec_config

# Lights
PRODUCT_PACKAGES += \
    android.hardware.light@2.0-service.xiaomi_msmnile

# Media
PRODUCT_PACKAGES += \
    libOmxAacEnc \
    libOmxAmrEnc \
    libOmxCore \
    libOmxEvrcEnc \
    libOmxG711Enc \
    libOmxQcelp13Enc \
    libOmxVdec \
    libOmxVenc \
    libstagefrighthw

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/seccomp_policy/mediacodec.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediacodec.policy

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/media/system_properties.xml:$(TARGET_COPY_OUT_VENDOR)/etc/system_properties.xml

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/media/media_codecs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs.xml \
    $(LOCAL_PATH)/media/media_codecs_vendor_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_vendor_audio.xml \
    $(LOCAL_PATH)/media/media_codecs_c2.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_c2.xml \
    $(LOCAL_PATH)/media/media_codecs_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_performance.xml \
    $(LOCAL_PATH)/media/media_profiles_vendor.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_vendor.xml

PRODUCT_COPY_FILES += \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_video.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_video_le.xml

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
    $(LOCAL_PATH)/nfc/libnfc-nxp.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nxp.conf

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
    $(LOCAL_PATH)/permissions/product_privapp-permissions-qti.xml:$(TARGET_COPY_OUT_PRODUCT)/etc/permissions/product_privapp-permissions-qti.xml \
    $(LOCAL_PATH)/configs/qti_whitelist.xml:system/etc/sysconfig/qti_whitelist.xml

# Recovery
PRODUCT_PACKAGES += \
    librecovery_updater_xiaomi

# VNDK
PRODUCT_TARGET_VNDK_VERSION := 29

# WiFi
PRODUCT_PACKAGES += \
    android.hardware.wifi@1.0-service \
    hostapd \
    libwifi-hal-qcom \
    libwpa_client \
    wpa_supplicant \
    wpa_supplicant.conf

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/wifi/wpa_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant_overlay.conf \
    $(LOCAL_PATH)/wifi/p2p_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/p2p_supplicant_overlay.conf \
    $(LOCAL_PATH)/wifi/WCNSS_qcom_cfg.ini:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/WCNSS_qcom_cfg.ini

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.wifi.passpoint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.passpoint.xml \
    frameworks/native/data/etc/android.hardware.wifi.aware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.aware.xml \
    frameworks/native/data/etc/android.hardware.wifi.rtt.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.rtt.xml
