/*
 * Copyright (C) 2018 The Android Open Source Project
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

#ifndef THERMAL_UTILS_CONFIG_PARSER_H__
#define THERMAL_UTILS_CONFIG_PARSER_H__

#include <map>
#include <string>

#include <android/hardware/thermal/2.0/IThermal.h>

namespace android {
namespace hardware {
namespace thermal {
namespace V2_0 {
namespace implementation {

using ::android::hardware::hidl_enum_range;
using ::android::hardware::thermal::V2_0::CoolingType;
using TemperatureType_2_0 = ::android::hardware::thermal::V2_0::TemperatureType;
using ::android::hardware::thermal::V2_0::ThrottlingSeverity;
constexpr size_t kThrottlingSeverityCount = std::distance(
    hidl_enum_range<ThrottlingSeverity>().begin(), hidl_enum_range<ThrottlingSeverity>().end());
using ThrottlingArray = std::array<float, static_cast<size_t>(kThrottlingSeverityCount)>;

struct SensorInfo {
    TemperatureType_2_0 type;
    ThrottlingArray hot_thresholds;
    ThrottlingArray cold_thresholds;
    ThrottlingArray hot_hysteresis;
    ThrottlingArray cold_hysteresis;
    float vr_threshold;
    float multiplier;
    bool is_monitor;
};

std::map<std::string, SensorInfo> ParseSensorInfo(std::string_view config_path);
std::map<std::string, CoolingType> ParseCoolingDevice(std::string_view config_path);

}  // namespace implementation
}  // namespace V2_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif  // THERMAL_UTILS_CONFIG_PARSER_H__
