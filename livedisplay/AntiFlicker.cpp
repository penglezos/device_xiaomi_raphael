/*
 * Copyright (C) 2021 The LineageOS Project
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

#define LOG_TAG "AntiFlickerService"

#include "AntiFlicker.h"
#include <android-base/logging.h>
#include <fstream>

namespace vendor {
namespace lineage {
namespace livedisplay {
namespace V2_1 {
namespace implementation {

static constexpr const char* kDcDimmingPath =
    "/sys/devices/platform/soc/soc:qcom,dsi-display-primary/msm_fb_ea_enable";

Return<bool> AntiFlicker::isEnabled() {
    std::ifstream file(kDcDimmingPath);
    int result = -1;
    file >> result;
    LOG(DEBUG) << "Got result " << result << " fail " << file.fail();
    return !file.fail() && result > 0;
}

Return<bool> AntiFlicker::setEnabled(bool enabled) {
    std::ofstream file(kDcDimmingPath);
    file << (enabled ? "1" : "0");
    LOG(DEBUG) << "setEnabled fail " << file.fail();
    return !file.fail();
}

}  // namespace implementation
}  // namespace V2_1
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
