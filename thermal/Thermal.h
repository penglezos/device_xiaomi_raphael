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
#ifndef ANDROID_HARDWARE_THERMAL_V2_0_CROSSHATCH_THERMAL_H
#define ANDROID_HARDWARE_THERMAL_V2_0_CROSSHATCH_THERMAL_H

#include <mutex>
#include <thread>

#include <android/hardware/thermal/2.0/IThermal.h>
#include <android/hardware/thermal/2.0/IThermalChangedCallback.h>
#include <hidl/Status.h>

#include "thermal-helper.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V2_0 {
namespace implementation {

using ::android::sp;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::thermal::V2_0::IThermal;
using ::android::hardware::thermal::V2_0::IThermalChangedCallback;

struct CallbackSetting {
    CallbackSetting(sp<IThermalChangedCallback> callback, bool is_filter_type,
                    TemperatureType_2_0 type)
        : callback(callback), is_filter_type(is_filter_type), type(type) {}
    sp<IThermalChangedCallback> callback;
    bool is_filter_type;
    TemperatureType_2_0 type;
};

class Thermal : public IThermal {
  public:
    Thermal();
    ~Thermal() = default;

    // Disallow copy and assign.
    Thermal(const Thermal &) = delete;
    void operator=(const Thermal &) = delete;

    // Methods from ::android::hardware::thermal::V1_0::IThermal.
    Return<void> getTemperatures(getTemperatures_cb _hidl_cb) override;
    Return<void> getCpuUsages(getCpuUsages_cb _hidl_cb) override;
    Return<void> getCoolingDevices(getCoolingDevices_cb _hidl_cb) override;

    // Methods from ::android::hardware::thermal::V2_0::IThermal follow.
    Return<void> getCurrentTemperatures(bool filterType, TemperatureType_2_0 type,
                                        getCurrentTemperatures_cb _hidl_cb) override;
    Return<void> getTemperatureThresholds(bool filterType, TemperatureType_2_0 type,
                                          getTemperatureThresholds_cb _hidl_cb) override;
    Return<void> registerThermalChangedCallback(const sp<IThermalChangedCallback> &callback,
                                                bool filterType, TemperatureType_2_0 type,
                                                registerThermalChangedCallback_cb _hidl_cb) override;
    Return<void> unregisterThermalChangedCallback(
        const sp<IThermalChangedCallback> &callback,
        unregisterThermalChangedCallback_cb _hidl_cb) override;
    Return<void> getCurrentCoolingDevices(bool filterType, CoolingType type,
                                          getCurrentCoolingDevices_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    Return<void> debug(const hidl_handle &fd, const hidl_vec<hidl_string> &args) override;

    // Helper function for calling callbacks
    void sendThermalChangedCallback(const std::vector<Temperature_2_0> &temps);

  private:
    ThermalHelper thermal_helper_;
    std::mutex thermal_callback_mutex_;
    std::vector<CallbackSetting> callbacks_;
};

}  // namespace implementation
}  // namespace V2_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_THERMAL_V2_0_CROSSHATCH_THERMAL_H
