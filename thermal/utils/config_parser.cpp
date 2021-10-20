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
#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include <cmath>
#include <set>

#include <json/reader.h>
#include <json/value.h>

#include "config_parser.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V2_0 {
namespace implementation {

using ::android::hardware::hidl_enum_range;
using ::android::hardware::thermal::V2_0::toString;
using TemperatureType_2_0 = ::android::hardware::thermal::V2_0::TemperatureType;

namespace {

template <typename T>
// Return false when failed parsing
bool getTypeFromString(std::string_view str, T *out) {
    auto types = hidl_enum_range<T>();
    for (const auto &type : types) {
        if (toString(type) == str) {
            *out = type;
            return true;
        }
    }
    return false;
}

float getFloatFromValue(const Json::Value &value) {
    if (value.isString()) {
        return std::stof(value.asString());
    } else {
        return value.asFloat();
    }
}

}  // namespace

std::map<std::string, SensorInfo> ParseSensorInfo(std::string_view config_path) {
    std::string json_doc;
    std::map<std::string, SensorInfo> sensors_parsed;
    if (!android::base::ReadFileToString(config_path.data(), &json_doc)) {
        LOG(ERROR) << "Failed to read JSON config from " << config_path;
        return sensors_parsed;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    std::string errorMessage;

    if (!reader->parse(&*json_doc.begin(), &*json_doc.end(), &root, &errorMessage)) {
        LOG(ERROR) << "Failed to parse JSON config";
        return sensors_parsed;
    }

    Json::Value sensors = root["Sensors"];
    std::size_t total_parsed = 0;
    std::set<std::string> sensors_name_parsed;

    for (Json::Value::ArrayIndex i = 0; i < sensors.size(); ++i) {
        const std::string &name = sensors[i]["Name"].asString();
        LOG(INFO) << "Sensor[" << i << "]'s Name: " << name;
        if (name.empty()) {
            LOG(ERROR) << "Failed to read "
                       << "Sensor[" << i << "]'s Name";
            sensors_parsed.clear();
            return sensors_parsed;
        }

        auto result = sensors_name_parsed.insert(name);
        if (!result.second) {
            LOG(ERROR) << "Duplicate Sensor[" << i << "]'s Name";
            sensors_parsed.clear();
            return sensors_parsed;
        }

        std::string sensor_type_str = sensors[i]["Type"].asString();
        LOG(INFO) << "Sensor[" << name << "]'s Type: " << sensor_type_str;
        TemperatureType_2_0 sensor_type;

        if (!getTypeFromString(sensor_type_str, &sensor_type)) {
            LOG(ERROR) << "Invalid "
                       << "Sensor[" << name << "]'s Type: " << sensor_type_str;
            sensors_parsed.clear();
            return sensors_parsed;
        }

        std::array<float, kThrottlingSeverityCount> hot_thresholds;
        hot_thresholds.fill(NAN);
        std::array<float, kThrottlingSeverityCount> cold_thresholds;
        cold_thresholds.fill(NAN);
        std::array<float, kThrottlingSeverityCount> hot_hysteresis;
        hot_hysteresis.fill(0.0);
        std::array<float, kThrottlingSeverityCount> cold_hysteresis;
        cold_hysteresis.fill(0.0);

        Json::Value values = sensors[i]["HotThreshold"];
        if (values.size() != kThrottlingSeverityCount) {
            LOG(ERROR) << "Invalid "
                       << "Sensor[" << name << "]'s HotThreshold count" << values.size();
            sensors_parsed.clear();
            return sensors_parsed;
        } else {
            float min = std::numeric_limits<float>::min();
            for (Json::Value::ArrayIndex j = 0; j < kThrottlingSeverityCount; ++j) {
                hot_thresholds[j] = getFloatFromValue(values[j]);
                if (!std::isnan(hot_thresholds[j])) {
                    if (hot_thresholds[j] < min) {
                        LOG(ERROR) << "Invalid "
                                   << "Sensor[" << name << "]'s HotThreshold[j" << j
                                   << "]: " << hot_thresholds[j] << " < " << min;
                        sensors_parsed.clear();
                        return sensors_parsed;
                    }
                    min = hot_thresholds[j];
                }
                LOG(INFO) << "Sensor[" << name << "]'s HotThreshold[" << j
                          << "]: " << hot_thresholds[j];
            }
        }

        values = sensors[i]["HotHysteresis"];
        if (values.size() != kThrottlingSeverityCount) {
            LOG(INFO) << "Cannot find valid "
                      << "Sensor[" << name << "]'s HotHysteresis, default all to 0.0";
        } else {
            for (Json::Value::ArrayIndex j = 0; j < kThrottlingSeverityCount; ++j) {
                hot_hysteresis[j] = getFloatFromValue(values[j]);
                if (std::isnan(hot_hysteresis[j])) {
                    LOG(ERROR) << "Invalid "
                               << "Sensor[" << name << "]'s HotHysteresis: " << hot_hysteresis[j];
                    sensors_parsed.clear();
                    return sensors_parsed;
                }
                LOG(INFO) << "Sensor[" << name << "]'s HotHysteresis[" << j
                          << "]: " << hot_hysteresis[j];
            }
        }

        values = sensors[i]["ColdThreshold"];
        if (values.size() != kThrottlingSeverityCount) {
            LOG(INFO) << "Cannot find valid "
                      << "Sensor[" << name << "]'s ColdThreshold, default all to NAN";
        } else {
            float max = std::numeric_limits<float>::max();
            for (Json::Value::ArrayIndex j = 0; j < kThrottlingSeverityCount; ++j) {
                cold_thresholds[j] = getFloatFromValue(values[j]);
                if (!std::isnan(cold_thresholds[j])) {
                    if (cold_thresholds[j] > max) {
                        LOG(ERROR) << "Invalid "
                                   << "Sensor[" << name << "]'s ColdThreshold[j" << j
                                   << "]: " << cold_thresholds[j] << " > " << max;
                        sensors_parsed.clear();
                        return sensors_parsed;
                    }
                    max = cold_thresholds[j];
                }
                LOG(INFO) << "Sensor[" << name << "]'s ColdThreshold[" << j
                          << "]: " << cold_thresholds[j];
            }
        }

        values = sensors[i]["ColdHysteresis"];
        if (values.size() != kThrottlingSeverityCount) {
            LOG(INFO) << "Cannot find valid "
                      << "Sensor[" << name << "]'s ColdHysteresis, default all to 0.0";
        } else {
            for (Json::Value::ArrayIndex j = 0; j < kThrottlingSeverityCount; ++j) {
                cold_hysteresis[j] = getFloatFromValue(values[j]);
                if (std::isnan(cold_hysteresis[j])) {
                    LOG(ERROR) << "Invalid "
                               << "Sensor[" << name
                               << "]'s ColdHysteresis: " << cold_hysteresis[j];
                    sensors_parsed.clear();
                    return sensors_parsed;
                }
                LOG(INFO) << "Sensor[" << name << "]'s ColdHysteresis[" << j
                          << "]: " << cold_hysteresis[j];
            }
        }

        float vr_threshold = NAN;
        vr_threshold = getFloatFromValue(sensors[i]["VrThreshold"]);
        LOG(INFO) << "Sensor[" << name << "]'s VrThreshold: " << vr_threshold;

        float multiplier = sensors[i]["Multiplier"].asFloat();
        LOG(INFO) << "Sensor[" << name << "]'s Multiplier: " << multiplier;

        bool is_monitor = false;
        if (sensors[i]["Monitor"].empty() || !sensors[i]["Monitor"].isBool()) {
            LOG(INFO) << "Failed to read Sensor[" << name << "]'s Monitor, set to 'false'";
        } else {
            is_monitor = sensors[i]["Monitor"].asBool();
        }
        LOG(INFO) << "Sensor[" << name << "]'s Monitor: " << std::boolalpha << is_monitor
                  << std::noboolalpha;

        sensors_parsed[name] = {
                .type = sensor_type,
                .hot_thresholds = hot_thresholds,
                .cold_thresholds = cold_thresholds,
                .hot_hysteresis = hot_hysteresis,
                .cold_hysteresis = cold_hysteresis,
                .vr_threshold = vr_threshold,
                .multiplier = multiplier,
                .is_monitor = is_monitor,
        };
        ++total_parsed;
    }

    LOG(INFO) << total_parsed << " Sensors parsed successfully";
    return sensors_parsed;
}

std::map<std::string, CoolingType> ParseCoolingDevice(std::string_view config_path) {
    std::string json_doc;
    std::map<std::string, CoolingType> cooling_devices_parsed;
    if (!android::base::ReadFileToString(config_path.data(), &json_doc)) {
        LOG(ERROR) << "Failed to read JSON config from " << config_path;
        return cooling_devices_parsed;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    std::string errorMessage;

    if (!reader->parse(&*json_doc.begin(), &*json_doc.end(), &root, &errorMessage)) {
        LOG(ERROR) << "Failed to parse JSON config";
        return cooling_devices_parsed;
    }

    Json::Value cooling_devices = root["CoolingDevices"];
    std::size_t total_parsed = 0;
    std::set<std::string> cooling_devices_name_parsed;

    for (Json::Value::ArrayIndex i = 0; i < cooling_devices.size(); ++i) {
        const std::string &name = cooling_devices[i]["Name"].asString();
        LOG(INFO) << "CoolingDevice[" << i << "]'s Name: " << name;
        if (name.empty()) {
            LOG(ERROR) << "Failed to read "
                       << "CoolingDevice[" << i << "]'s Name";
            cooling_devices_parsed.clear();
            return cooling_devices_parsed;
        }

        auto result = cooling_devices_name_parsed.insert(name.data());
        if (!result.second) {
            LOG(ERROR) << "Duplicate CoolingDevice[" << i << "]'s Name";
            cooling_devices_parsed.clear();
            return cooling_devices_parsed;
        }

        std::string cooling_device_type_str = cooling_devices[i]["Type"].asString();
        LOG(INFO) << "CoolingDevice[" << name << "]'s Type: " << cooling_device_type_str;
        CoolingType cooling_device_type;

        if (!getTypeFromString(cooling_device_type_str, &cooling_device_type)) {
            LOG(ERROR) << "Invalid "
                       << "CoolingDevice[" << name << "]'s Type: " << cooling_device_type_str;
            cooling_devices_parsed.clear();
            return cooling_devices_parsed;
        }

        cooling_devices_parsed[name] = cooling_device_type;

        ++total_parsed;
    }

    LOG(INFO) << total_parsed << " CoolingDevices parsed successfully";
    return cooling_devices_parsed;
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android
