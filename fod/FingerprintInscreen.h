/*
 * Copyright (C) 2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VENDOR_P404_BIOMETRICS_FINGERPRINT_INSCREEN_V1_0_FINGERPRINTINSCREEN_H
#define VENDOR_P404_BIOMETRICS_FINGERPRINT_INSCREEN_V1_0_FINGERPRINTINSCREEN_H

#include <vendor/p404/biometrics/fingerprint/inscreen/1.0/IFingerprintInscreen.h>
#include <vendor/xiaomi/hardware/fingerprintextension/1.0/IXiaomiFingerprint.h>

namespace vendor {
namespace p404 {
namespace biometrics {
namespace fingerprint {
namespace inscreen {
namespace V1_0 {
namespace implementation {

using ::android::sp;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::vendor::xiaomi::hardware::fingerprintextension::V1_0::IXiaomiFingerprint;

class FingerprintInscreen : public IFingerprintInscreen {
public:
    FingerprintInscreen();
    Return<int32_t> getPositionX() override;
    Return<int32_t> getPositionY() override;
    Return<int32_t> getSize() override;
    Return<void> onStartEnroll() override;
    Return<void> onFinishEnroll() override;
    Return<void> onPress() override;
    Return<void> onRelease() override;
    Return<void> onShowFODView() override;
    Return<void> onHideFODView() override;
    Return<bool> handleAcquired(int32_t acquiredInfo, int32_t vendorCode) override;
    Return<bool> handleError(int32_t error, int32_t vendorCode) override;
    Return<void> setLongPressEnabled(bool enabled) override;
    Return<int32_t> getDimAmount(int32_t brightness) override;
    Return<bool> shouldBoostBrightness() override;
    Return<void> setCallback(const sp<IFingerprintInscreenCallback>& callback) override;

private:
    bool mFodCircleVisible;
    sp<IXiaomiFingerprint> xiaomiFingerprintService;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace inscreenx
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace p404
}  // namespace vendor

#endif  // VENDOR_P404_BIOMETRICS_FINGERPRINT_INSCREEN_V1_0_FINGERPRINTINSCREEN_H
