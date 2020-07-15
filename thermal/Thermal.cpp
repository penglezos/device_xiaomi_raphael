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

#include <cerrno>
#include <mutex>
#include <string>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>

#include "Thermal.h"
#include "thermal-helper.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V2_0 {
namespace implementation {

namespace {

using ::android::hardware::interfacesEqual;
using ::android::hardware::thermal::V1_0::ThermalStatus;
using ::android::hardware::thermal::V1_0::ThermalStatusCode;
using ::android::hidl::base::V1_0::IBase;

template <typename T, typename U>
Return<void> setFailureAndCallback(T _hidl_cb, hidl_vec<U> data, std::string_view debug_msg) {
    ThermalStatus status;
    status.code = ThermalStatusCode::FAILURE;
    status.debugMessage = debug_msg.data();
    _hidl_cb(status, data);
    return Void();
}

template <typename T, typename U>
Return<void> setInitFailureAndCallback(T _hidl_cb, hidl_vec<U> data) {
    return setFailureAndCallback(_hidl_cb, data, "Failure initializing thermal HAL");
}

}  // namespace

// On init we will spawn a thread which will continually watch for
// throttling.  When throttling is seen, if we have a callback registered
// the thread will call notifyThrottling() else it will log the dropped
// throttling event and do nothing.  The thread is only killed when
// Thermal() is killed.
Thermal::Thermal()
    : thermal_helper_(
          std::bind(&Thermal::sendThermalChangedCallback, this, std::placeholders::_1)) {}

// Methods from ::android::hardware::thermal::V1_0::IThermal.
Return<void> Thermal::getTemperatures(getTemperatures_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    hidl_vec<Temperature_1_0> temperatures;

    if (!thermal_helper_.isInitializedOk()) {
        LOG(ERROR) << "ThermalHAL not initialized properly.";
        return setInitFailureAndCallback(_hidl_cb, temperatures);
    }

    if (!thermal_helper_.fillTemperatures(&temperatures)) {
        return setFailureAndCallback(_hidl_cb, temperatures, "Failed to read thermal sensors.");
    }

    _hidl_cb(status, temperatures);
    return Void();
}

Return<void> Thermal::getCpuUsages(getCpuUsages_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    hidl_vec<CpuUsage> cpu_usages;

    if (!thermal_helper_.isInitializedOk()) {
        return setInitFailureAndCallback(_hidl_cb, cpu_usages);
    }

    if (!thermal_helper_.fillCpuUsages(&cpu_usages)) {
        return setFailureAndCallback(_hidl_cb, cpu_usages, "Failed to get CPU usages.");
    }

    _hidl_cb(status, cpu_usages);
    return Void();
}

Return<void> Thermal::getCoolingDevices(getCoolingDevices_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    hidl_vec<CoolingDevice_1_0> cooling_devices;

    if (!thermal_helper_.isInitializedOk()) {
        return setInitFailureAndCallback(_hidl_cb, cooling_devices);
    }
    _hidl_cb(status, cooling_devices);
    return Void();
}

Return<void> Thermal::getCurrentTemperatures(bool filterType, TemperatureType_2_0 type,
                                             getCurrentTemperatures_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    hidl_vec<Temperature_2_0> temperatures;

    if (!thermal_helper_.isInitializedOk()) {
        LOG(ERROR) << "ThermalHAL not initialized properly.";
        return setInitFailureAndCallback(_hidl_cb, temperatures);
    }

    if (!thermal_helper_.fillCurrentTemperatures(filterType, type, &temperatures)) {
        return setFailureAndCallback(_hidl_cb, temperatures, "Failed to read thermal sensors.");
    }

    _hidl_cb(status, temperatures);
    return Void();
}

Return<void> Thermal::getTemperatureThresholds(bool filterType, TemperatureType_2_0 type,
                                               getTemperatureThresholds_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    hidl_vec<TemperatureThreshold> temperatures;

    if (!thermal_helper_.isInitializedOk()) {
        LOG(ERROR) << "ThermalHAL not initialized properly.";
        return setInitFailureAndCallback(_hidl_cb, temperatures);
    }

    if (!thermal_helper_.fillTemperatureThresholds(filterType, type, &temperatures)) {
        return setFailureAndCallback(_hidl_cb, temperatures, "Failed to read thermal sensors.");
    }

    _hidl_cb(status, temperatures);
    return Void();
}

Return<void> Thermal::getCurrentCoolingDevices(bool filterType, CoolingType type,
                                               getCurrentCoolingDevices_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    hidl_vec<CoolingDevice_2_0> cooling_devices;

    if (!thermal_helper_.isInitializedOk()) {
        LOG(ERROR) << "ThermalHAL not initialized properly.";
        return setInitFailureAndCallback(_hidl_cb, cooling_devices);
    }

    if (!thermal_helper_.fillCurrentCoolingDevices(filterType, type, &cooling_devices)) {
        return setFailureAndCallback(_hidl_cb, cooling_devices, "Failed to read thermal sensors.");
    }

    _hidl_cb(status, cooling_devices);
    return Void();
}

Return<void> Thermal::registerThermalChangedCallback(const sp<IThermalChangedCallback> &callback,
                                                     bool filterType, TemperatureType_2_0 type,
                                                     registerThermalChangedCallback_cb _hidl_cb) {
    ThermalStatus status;
    if (callback == nullptr) {
        status.code = ThermalStatusCode::FAILURE;
        status.debugMessage = "Invalid nullptr callback";
        LOG(ERROR) << status.debugMessage;
        _hidl_cb(status);
        return Void();
    } else {
        status.code = ThermalStatusCode::SUCCESS;
    }
    std::lock_guard<std::mutex> _lock(thermal_callback_mutex_);
    if (std::any_of(callbacks_.begin(), callbacks_.end(), [&](const CallbackSetting &c) {
            return interfacesEqual(c.callback, callback);
        })) {
        status.code = ThermalStatusCode::FAILURE;
        status.debugMessage = "Same callback registered already";
        LOG(ERROR) << status.debugMessage;
    } else {
        callbacks_.emplace_back(callback, filterType, type);
        LOG(INFO) << "a callback has been registered to ThermalHAL, isFilter: " << filterType
                  << " Type: " << android::hardware::thermal::V2_0::toString(type);
    }
    _hidl_cb(status);
    return Void();
}

Return<void> Thermal::unregisterThermalChangedCallback(
    const sp<IThermalChangedCallback> &callback, unregisterThermalChangedCallback_cb _hidl_cb) {
    ThermalStatus status;
    if (callback == nullptr) {
        status.code = ThermalStatusCode::FAILURE;
        status.debugMessage = "Invalid nullptr callback";
        LOG(ERROR) << status.debugMessage;
        _hidl_cb(status);
        return Void();
    } else {
        status.code = ThermalStatusCode::SUCCESS;
    }
    bool removed = false;
    std::lock_guard<std::mutex> _lock(thermal_callback_mutex_);
    callbacks_.erase(
        std::remove_if(callbacks_.begin(), callbacks_.end(),
                       [&](const CallbackSetting &c) {
                           if (interfacesEqual(c.callback, callback)) {
                               LOG(INFO)
                                   << "a callback has been unregistered to ThermalHAL, isFilter: "
                                   << c.is_filter_type << " Type: "
                                   << android::hardware::thermal::V2_0::toString(c.type);
                               removed = true;
                               return true;
                           }
                           return false;
                       }),
        callbacks_.end());
    if (!removed) {
        status.code = ThermalStatusCode::FAILURE;
        status.debugMessage = "The callback was not registered before";
        LOG(ERROR) << status.debugMessage;
    }
    _hidl_cb(status);
    return Void();
}

void Thermal::sendThermalChangedCallback(const std::vector<Temperature_2_0> &temps) {
    std::lock_guard<std::mutex> _lock(thermal_callback_mutex_);
    for (auto &t : temps) {
        LOG(INFO) << "Sending notification: "
                  << " Type: " << android::hardware::thermal::V2_0::toString(t.type)
                  << " Name: " << t.name << " CurrentValue: " << t.value << " ThrottlingStatus: "
                  << android::hardware::thermal::V2_0::toString(t.throttlingStatus);
        callbacks_.erase(
            std::remove_if(callbacks_.begin(), callbacks_.end(),
                           [&](const CallbackSetting &c) {
                               if (!c.is_filter_type || t.type == c.type) {
                                   Return<void> ret = c.callback->notifyThrottling(t);
                                   return !ret.isOk();
                               }
                               LOG(ERROR)
                                   << "a Thermal callback is dead, removed from callback list.";
                               return false;
                           }),
            callbacks_.end());
    }
}

Return<void> Thermal::debug(const hidl_handle &handle, const hidl_vec<hidl_string> &) {
    if (handle != nullptr && handle->numFds >= 1) {
        int fd = handle->data[0];
        std::ostringstream dump_buf;

        if (!thermal_helper_.isInitializedOk()) {
            dump_buf << "ThermalHAL not initialized properly." << std::endl;
        } else {
            {
                hidl_vec<Temperature_1_0> temperatures;
                dump_buf << "getTemperatures:" << std::endl;
                if (!thermal_helper_.fillTemperatures(&temperatures)) {
                    dump_buf << "Failed to read thermal sensors." << std::endl;
                }

                for (const auto &t : temperatures) {
                    dump_buf << " Type: " << android::hardware::thermal::V1_0::toString(t.type)
                             << " Name: " << t.name << " CurrentValue: " << t.currentValue
                             << " ThrottlingThreshold: " << t.throttlingThreshold
                             << " ShutdownThreshold: " << t.shutdownThreshold
                             << " VrThrottlingThreshold: " << t.vrThrottlingThreshold << std::endl;
                }
            }
            {
                hidl_vec<CpuUsage> cpu_usages;
                dump_buf << "getCpuUsages:" << std::endl;
                if (!thermal_helper_.fillCpuUsages(&cpu_usages)) {
                    dump_buf << "Failed to get CPU usages." << std::endl;
                }

                for (const auto &usage : cpu_usages) {
                    dump_buf << " Name: " << usage.name << " Active: " << usage.active
                             << " Total: " << usage.total << " IsOnline: " << usage.isOnline
                             << std::endl;
                }
            }
            {
                dump_buf << "getCurrentTemperatures:" << std::endl;
                hidl_vec<Temperature_2_0> temperatures;
                if (!thermal_helper_.fillCurrentTemperatures(false, TemperatureType_2_0::SKIN,
                                                             &temperatures)) {
                    dump_buf << "Failed to getCurrentTemperatures." << std::endl;
                }

                for (const auto &t : temperatures) {
                    dump_buf << " Type: " << android::hardware::thermal::V2_0::toString(t.type)
                             << " Name: " << t.name << " CurrentValue: " << t.value
                             << " ThrottlingStatus: "
                             << android::hardware::thermal::V2_0::toString(t.throttlingStatus)
                             << std::endl;
                }
            }
            {
                dump_buf << "getTemperatureThresholds:" << std::endl;
                hidl_vec<TemperatureThreshold> temperatures;
                if (!thermal_helper_.fillTemperatureThresholds(false, TemperatureType_2_0::SKIN,
                                                               &temperatures)) {
                    dump_buf << "Failed to getTemperatureThresholds." << std::endl;
                }

                for (const auto &t : temperatures) {
                    dump_buf << " Type: " << android::hardware::thermal::V2_0::toString(t.type)
                             << " Name: " << t.name;
                    dump_buf << " hotThrottlingThreshold: [";
                    for (size_t i = 0; i < kThrottlingSeverityCount; ++i) {
                        dump_buf << t.hotThrottlingThresholds[i] << " ";
                    }
                    dump_buf << "] coldThrottlingThreshold: [";
                    for (size_t i = 0; i < kThrottlingSeverityCount; ++i) {
                        dump_buf << t.coldThrottlingThresholds[i] << " ";
                    }
                    dump_buf << "] vrThrottlingThreshold: " << t.vrThrottlingThreshold;
                    dump_buf << std::endl;
                }
            }
            {
                dump_buf << "getCurrentCoolingDevices:" << std::endl;
                hidl_vec<CoolingDevice_2_0> cooling_devices;
                if (!thermal_helper_.fillCurrentCoolingDevices(false, CoolingType::CPU,
                                                               &cooling_devices)) {
                    dump_buf << "Failed to getCurrentCoolingDevices." << std::endl;
                }

                for (const auto &c : cooling_devices) {
                    dump_buf << " Type: " << android::hardware::thermal::V2_0::toString(c.type)
                             << " Name: " << c.name << " CurrentValue: " << c.value << std::endl;
                }
            }
            {
                dump_buf << "Callbacks: Total " << callbacks_.size() << std::endl;
                for (const auto &c : callbacks_) {
                    dump_buf << " IsFilter: " << c.is_filter_type
                             << " Type: " << android::hardware::thermal::V2_0::toString(c.type)
                             << std::endl;
                }
            }
            {
                dump_buf << "getHysteresis:" << std::endl;
                const auto &map = thermal_helper_.GetSensorInfoMap();
                for (const auto &name_info_pair : map) {
                    dump_buf << " Name: " << name_info_pair.first;
                    dump_buf << " hotHysteresis: [";
                    for (size_t i = 0; i < kThrottlingSeverityCount; ++i) {
                        dump_buf << name_info_pair.second.hot_hysteresis[i] << " ";
                    }
                    dump_buf << "] coldHysteresis: [";
                    for (size_t i = 0; i < kThrottlingSeverityCount; ++i) {
                        dump_buf << name_info_pair.second.cold_hysteresis[i] << " ";
                    }
                    dump_buf << "]" << std::endl;
                }
            }
            {
                dump_buf << "Monitor:" << std::endl;
                const auto &map = thermal_helper_.GetSensorInfoMap();
                for (const auto &name_info_pair : map) {
                    dump_buf << " Name: " << name_info_pair.first;
                    dump_buf << " Monitor: " << std::boolalpha << name_info_pair.second.is_monitor
                             << std::noboolalpha << std::endl;
                }
            }
        }
        std::string buf = dump_buf.str();
        if (!android::base::WriteStringToFd(buf, fd)) {
            PLOG(ERROR) << "Failed to dump state to fd";
        }
        fsync(fd);
    }
    return Void();
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android
