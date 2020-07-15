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
#ifndef THERMAL_UTILS_THERMAL_WATCHER_H_
#define THERMAL_UTILS_THERMAL_WATCHER_H_

#include <chrono>
#include <condition_variable>
#include <future>
#include <list>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <android-base/chrono_utils.h>
#include <android-base/unique_fd.h>
#include <utils/Looper.h>
#include <utils/Thread.h>

namespace android {
namespace hardware {
namespace thermal {
namespace V2_0 {
namespace implementation {

using android::base::boot_clock;
using android::base::unique_fd;
using WatcherCallback = std::function<bool(const std::set<std::string> &name)>;

// A helper class for monitoring thermal files changes.
class ThermalWatcher : public ::android::Thread {
  public:
    ThermalWatcher(const WatcherCallback &cb)
        : Thread(false), cb_(cb), looper_(new Looper(true)) {}
    ~ThermalWatcher() = default;

    // Disallow copy and assign.
    ThermalWatcher(const ThermalWatcher &) = delete;
    void operator=(const ThermalWatcher &) = delete;

    // Start the thread and return true if it succeeds.
    bool startWatchingDeviceFiles();
    // Give the file watcher a list of files to start watching. This helper
    // class will by default wait for modifications to the file with a looper.
    // This should be called before starting watcher thread.
    void registerFilesToWatch(const std::set<std::string> &sensors_to_watch, bool uevent_monitor);
    // Wake up the looper thus the worker thread, immediately. This can be called
    // in any thread.
    void wake();

  private:
    // The work done by the watcher thread. This will use inotify to check for
    // modifications to the files to watch. If any modification is seen this
    // will callback the registered function with the new data read from the
    // modified file.
    bool threadLoop() override;

    // Parse uevent message
    void parseUevent(std::set<std::string> *sensor_name);

    // Maps watcher filer descriptor to watched file path.
    std::unordered_map<int, std::string> watch_to_file_path_map_;

    // The callback function. Called whenever thermal uevent is seen.
    // The function passed in should expect a string in the form (type).
    // Where type is the name of the thermal zone that trigger a uevent notification.
    // Callback will return thermal trigger status for next polling decision.
    const WatcherCallback cb_;

    sp<Looper> looper_;

    // For uevent socket registration.
    android::base::unique_fd uevent_fd_;
    // Sensor list which monitor flag is enabled.
    std::set<std::string> monitored_sensors_;
    // Flag to point out if any sensor across the first threshold.
    bool thermal_triggered_;
    // Flag to point out if device can support uevent notify.
    bool is_polling_;
    // Timestamp for last thermal update
    boot_clock::time_point last_update_time_;
};

}  // namespace implementation
}  // namespace V2_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif  // THERMAL_UTILS_THERMAL_WATCHER_H_
