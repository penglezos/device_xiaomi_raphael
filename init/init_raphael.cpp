/*
 * Copyright (c) 2021, The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "vendor_init.h"
#include "property_service.h"

void property_override(char const prop[], char const value[]) {
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);

    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void load_raphael() {
    property_override("ro.build.fingerprint", "Xiaomi/raphael/raphael:11/RKQ1.200826.002/V12.5.4.0.RFKCNXM:user/release-keys");
    property_override("ro.build.product", "raphael");
    property_override("ro.product.device", "raphael");
    property_override("ro.product.model", "Redmi K20 Pro");
}

void load_raphaelglobal() {
    property_override("ro.build.fingerprint", "Xiaomi/raphael/raphael:10/QKQ1.190825.002/V12.0.0.8.QFKMIXM:user/release-keys");
    property_override("ro.build.product", "raphael");
    property_override("ro.product.device", "raphael");
    property_override("ro.product.model", "Mi 9T Pro");
}

void load_raphaelin() {
    property_override("ro.build.fingerprint", "Xiaomi/raphaelin/raphaelin:10/QKQ1.190825.002/V12.0.5.0.QFKINXM:user/release-keys");
    property_override("ro.build.product", "raphaelin");
    property_override("ro.product.device", "raphaelin");
    property_override("ro.product.model", "Redmi K20 Pro");
}

void vendor_load_properties() {
    std::string region = android::base::GetProperty("ro.boot.hwc", "");

    if (region.find("CN") != std::string::npos)
        load_raphael();
    else if (region.find("GLOBAL") != std::string::npos)
        load_raphaelglobal();
    else if (region.find("INDIA") != std::string::npos)
        load_raphaelin();
}
