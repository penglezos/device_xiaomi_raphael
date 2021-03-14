/*
 * Copyright (C) 2019-2020 The LineageOS Project
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

#define LOG_TAG "vendor.lineage.livedisplay@2.1-service.raphael"

#include <android-base/logging.h>
#include <binder/ProcessState.h>
#include <hidl/HidlTransportSupport.h>

#include "AntiFlicker.h"
#include "SunlightEnhancement.h"
#include "livedisplay/sdm/SDMController.h"

using android::OK;
using android::sp;
using android::status_t;

using ::vendor::lineage::livedisplay::V2_0::sdm::SDMController;
using ::vendor::lineage::livedisplay::V2_1::IAntiFlicker;
using ::vendor::lineage::livedisplay::V2_1::ISunlightEnhancement;
using ::vendor::lineage::livedisplay::V2_1::implementation::AntiFlicker;
using ::vendor::lineage::livedisplay::V2_1::implementation::SunlightEnhancement;

int main() {
    status_t status = OK;
    std::shared_ptr<SDMController> controller = std::make_shared<SDMController>();
    sp<AntiFlicker> af = new AntiFlicker();
    sp<SunlightEnhancement> se = new SunlightEnhancement();
    android::hardware::configureRpcThreadpool(1, true /*callerWillJoin*/);

    // AntiFlicker service
    status = af->registerAsService();
    if (status != OK) {
        LOG(ERROR) << "Could not register service for LiveDisplay HAL AntiFlicker Iface ("
                   << status << ")";
        return 1;
    }

    // SunlightEnhancement service
    status = se->registerAsService();
    if (status != OK) {
        LOG(ERROR) << "Could not register service for LiveDisplay HAL SunlightEnhancement Iface ("
                   << status << ")";
        return 1;
    }

    LOG(INFO) << "LiveDisplay HAL service is ready.";

    android::hardware::joinRpcThreadpool();

    LOG(ERROR) << "LiveDisplay HAL service failed to join thread pool.";
    return 1;
}
