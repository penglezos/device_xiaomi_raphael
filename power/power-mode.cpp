/*
 * Copyright (C) 2020 The LineageOS Project
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

#include <aidl/android/hardware/power/BnPower.h>
#include <android-base/file.h>
#include <android-base/logging.h>
#include <linux/input.h>

namespace {
int open_ts_input() {
    int fd = -1;
    DIR* dir = opendir("/dev/input");

    if (dir != NULL) {
        struct dirent* ent;

        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_CHR) {
                char absolute_path[PATH_MAX] = {0};
                char name[80] = {0};

                strcpy(absolute_path, "/dev/input/");
                strcat(absolute_path, ent->d_name);

                fd = open(absolute_path, O_RDWR);
                if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) > 0) {
                    if (strcmp(name, "goodix_ts") == 0)
                        break;
                }

                close(fd);
                fd = -1;
            }
        }

        closedir(dir);
    }

    return fd;
}
}  // anonymous namespace

namespace aidl {
namespace android {
namespace hardware {
namespace power {
namespace impl {

static constexpr int kInputEventWakeupModeOff = 4;
static constexpr int kInputEventWakeupModeOn = 5;

using ::aidl::android::hardware::power::Mode;

bool isDeviceSpecificModeSupported(Mode type, bool* _aidl_return) {
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE:
            *_aidl_return = true;
            return true;
        default:
            return false;
    }
}

bool setDeviceSpecificMode(Mode type, bool enabled) {
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE:
            {
                int fd = open_ts_input();
                if (fd == -1) {
                    LOG(WARNING)
                        << "DT2W won't work because no supported touchscreen input devices were found";
                    return false;
                }
                struct input_event ev;
                ev.type = EV_SYN;
                ev.code = SYN_CONFIG;
                ev.value = enabled ? kInputEventWakeupModeOn : kInputEventWakeupModeOff;
                write(fd, &ev, sizeof(ev));
                close(fd);
            }
            return true;
        default:
            return false;
    }
}

}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace android
}  // namespace aidl
