/*
 * Copyright (C) 2018-2020 The LineageOS Project
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

#pragma once

#include <android/hardware/light/2.0/ILight.h>

#include <unordered_map>

namespace android {
namespace hardware {
namespace light {
namespace V2_0 {
namespace implementation {

using ::android::hardware::Return;
using ::android::hardware::light::V2_0::ILight;
using ::android::hardware::light::V2_0::LightState;
using ::android::hardware::light::V2_0::Status;
using ::android::hardware::light::V2_0::Type;

class Light : public ILight {
  public:
    Light();

    Return<Status> setLight(Type type, const LightState& state) override;
    Return<void> getSupportedTypes(getSupportedTypes_cb _hidl_cb) override;

  private:
    void setLightNotification(Type type, const LightState& state);
    void applyNotificationState(const LightState& state);

    uint32_t max_led_brightness_;

    std::unordered_map<Type, std::function<void(Type type, const LightState&)>> lights_{
            {Type::ATTENTION, [this](auto&&... args) { setLightNotification(args...); }},
            {Type::BATTERY, [this](auto&&... args) { setLightNotification(args...); }},
            {Type::NOTIFICATIONS, [this](auto&&... args) { setLightNotification(args...); }}};

    // Keep sorted in the order of importance.
    std::array<std::pair<Type, LightState>, 3> notif_states_ = {{
            {Type::ATTENTION, {}},
            {Type::NOTIFICATIONS, {}},
            {Type::BATTERY, {}},
    }};
};

}  // namespace implementation
}  // namespace V2_0
}  // namespace light
}  // namespace hardware
}  // namespace android
