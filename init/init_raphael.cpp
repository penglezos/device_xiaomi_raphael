/*
   Copyright (c) 2014, The Linux Foundation. All rights reserved.
   Copyright (C) 2021 The LineageOS Project.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <vector>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <sys/sysinfo.h>

using android::base::GetProperty;

void property_override(char const prop[], char const value[], bool add = true) {
    prop_info* pi;

    pi = (prop_info*)__system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void load_dalvikvm_properties() {
    struct sysinfo sys;

    sysinfo(&sys);
    if (sys.totalram < 7000ull * 1024 * 1024) {
        // 4/6GB RAM
        property_override("dalvik.vm.heapstartsize", "16m");
        property_override("dalvik.vm.heaptargetutilization", "0.5");
        property_override("dalvik.vm.heapmaxfree", "32m");
    } else {
        // 8/12/16GB RAM
        property_override("dalvik.vm.heapstartsize", "24m");
        property_override("dalvik.vm.heaptargetutilization", "0.46");
        property_override("dalvik.vm.heapmaxfree", "48m");
    }

    property_override("dalvik.vm.heapgrowthlimit", "256m");
    property_override("dalvik.vm.heapsize", "512m");
    property_override("dalvik.vm.heapminfree", "8m");
}

std::vector<std::string> ro_props_default_source_order = {
        "", "bootimage.", "odm.", "product.", "system.", "system_ext.", "vendor.",
};

void set_ro_build_prop(const std::string& prop, const std::string& value) {
    for (const auto& source : ro_props_default_source_order) {
        auto prop_name = "ro." + source + "build." + prop;
        if (source == "")
            property_override(prop_name.c_str(), value.c_str());
        else
            property_override(prop_name.c_str(), value.c_str(), false);
    }
};

void set_ro_product_prop(const std::string& prop, const std::string& value) {
    for (const auto& source : ro_props_default_source_order) {
        auto prop_name = "ro.product." + source + prop;
        property_override(prop_name.c_str(), value.c_str(), false);
    }
};

void vendor_load_properties() {
    std::string region;
    std::string hardware_revision;
    region = GetProperty("ro.boot.hwc", "GLOBAL");
    hardware_revision = GetProperty("ro.boot.hwversion", "UNKNOWN");

    std::string model;
    std::string device;
    std::string fingerprint;
    std::string description;
    std::string mod_device;

    if (region == "GLOBAL") {
        model = "Mi 9T Pro";
        device = "raphael";
        fingerprint =
                "Xiaomi/raphael/raphael:10/QKQ1.190825.002/V12.0.4.0.QFKMIXM:user/release-keys";
        description = "raphael-user 10 QKQ1.190825.002 V12.0.4.0.QFKMIXM release-keys";
        mod_device = "raphael_global";
    } else if (region == "CN") {
        model = "Redmi K20 Pro";
        device = "raphael";
        fingerprint =
                "Xiaomi/raphael/raphael:10/QKQ1.190825.002/V12.0.6.0.QFKCNXM:user/release-keys";
        description = "raphael-user 10 QKQ1.190825.002 V12.0.6.0.QFKCNXM release-keys";
    } else if (region == "INDIA") {
        model = "Redmi K20 Pro";
        device = "raphaelin";
        fingerprint =
                "Xiaomi/raphaelin/raphaelin:10/QKQ1.190825.002/V12.0.4.0.QFKINXM:user/release-keys";
        description = "raphaelin-user 10 QKQ1.190825.002 V12.0.4.0.QFKINXM release-keys";
        mod_device = "raphaelin_in_global";
    }

    // SafetyNet workaround
    property_override("ro.boot.verifiedbootstate", "green");
    fingerprint = "Xiaomi/dipper/dipper:8.1.0/OPM1.171019.011/V9.5.5.0.OEAMIFA:user/release-keys";
    description = "dipper-user 8.1.0 OPM1.171019.011 V9.5.5.0.OEAMIFA release-keys";

    set_ro_build_prop("fingerprint", fingerprint);
    set_ro_product_prop("device", device);
    set_ro_product_prop("model", model);
    property_override("ro.build.description", description.c_str());
    if (mod_device != "") {
        property_override("ro.product.mod_device", mod_device.c_str());
    }

    property_override("ro.boot.hardware.revision", hardware_revision.c_str());

    load_dalvikvm_properties();
}
