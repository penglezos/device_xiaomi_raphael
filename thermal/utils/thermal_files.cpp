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

#include <algorithm>
#include <string_view>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include "thermal_files.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V2_0 {
namespace implementation {

std::string ThermalFiles::getThermalFilePath(std::string_view thermal_name) const {
    auto sensor_itr = thermal_name_to_path_map_.find(thermal_name.data());
    if (sensor_itr == thermal_name_to_path_map_.end()) {
        return "";
    }
    return sensor_itr->second;
}

bool ThermalFiles::addThermalFile(std::string_view thermal_name, std::string_view path) {
    return thermal_name_to_path_map_.emplace(thermal_name, path).second;
}

bool ThermalFiles::readThermalFile(std::string_view thermal_name, std::string *data) const {
    std::string sensor_reading;
    std::string file_path = getThermalFilePath(std::string_view(thermal_name));
    *data = "";
    if (file_path.empty()) {
        return false;
    }

    if (!::android::base::ReadFileToString(file_path, &sensor_reading)) {
        PLOG(WARNING) << "Failed to read sensor: " << thermal_name;
        return false;
    }

    // Strip the newline.
    *data = ::android::base::Trim(sensor_reading);
    return true;
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android
