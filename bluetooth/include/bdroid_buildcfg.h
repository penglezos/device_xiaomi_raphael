/*
 *
 *  Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *  Not a Contribution, Apache license notifications and license are retained
 *  for attribution purposes only.
 *
 * Copyright (C) 2012 The Android Open Source Project
 * Copyright (C) 2018-2019 The LineageOS Project
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

#ifndef _BDROID_BUILDCFG_H
#define _BDROID_BUILDCFG_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
int property_get(const char *key, char *value, const char *default_value);
#ifdef __cplusplus
}
#endif

#include "osi/include/osi.h"

typedef struct {
    const char *product_device;
    const char *product_model;
} device_t;

static const device_t devices[] = {
    {"raphael", "Xiaomi Mi 9T Pro"},
};

static inline const char *BtmGetDefaultName()
{
    char product_device[92];
    property_get("ro.product.device", product_device, "");

    for (unsigned int i = 0; i < ARRAY_SIZE(devices); i++) {
        device_t device = devices[i];

        if (strcmp(device.product_device, product_device) == 0) {
            return device.product_model;
        }
    }

    // Fallback to ro.product.model
    return "";
}

#define BTM_DEF_LOCAL_NAME BtmGetDefaultName()
// Disables read remote device feature
#define MAX_ACL_CONNECTIONS   16
#define MAX_L2CAP_CHANNELS    32
#define BTM_WBS_INCLUDED   TRUE
#define BTIF_HF_WBS_PREFERRED   TRUE
#define BLE_VND_INCLUDED   TRUE
#define GATT_MAX_PHY_CHANNEL  10
// Skips conn update at conn completion
#define BT_CLEAN_TURN_ON_DISABLED 1
// Increasing SEPs to 12 from 6 to support SHO/MCast i.e. two streams per codec
#define AVDT_NUM_SEPS 35

#endif
