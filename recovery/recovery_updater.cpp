/*
 * Copyright (C) 2016, The CyanogenMod Project
 * Copyright (C) 2017-2018, The LineageOS Project
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

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "edify/expr.h"
#include "otautil/error_code.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define ALPHABET_LEN 256

#define MODEM_PART_PATH "/dev/block/bootdevice/by-name/modem"
#define MODEM_VER_STR "Time_Stamp\": \""
#define MODEM_VER_STR_LEN 14
#define MODEM_VER_BUF_LEN 20

#define VENDOR_PART_PATH "/dev/block/bootdevice/by-name/vendor"
#define VENDOR_DATE_STR "ro.vendor.build.date.utc="
#define VENDOR_DATE_STR_LEN 25
#define VENDOR_DATE_BUF_LEN 11

#define VNDK_VER_STR "ro.vndk.version="
#define VNDK_VER_STR_LEN 16
#define VNDK_VER_BUF_LEN 3

/* Boyer-Moore string search implementation from Wikipedia */

/* Return longest suffix length of suffix ending at str[p] */
static int max_suffix_len(const char* str, size_t str_len, size_t p) {
    uint32_t i;

    for (i = 0; (str[p - i] == str[str_len - 1 - i]) && (i < p);) {
        i++;
    }

    return i;
}

/* Generate table of distance between last character of pat and rightmost
 * occurrence of character c in pat
 */
static void bm_make_delta1(int* delta1, const char* pat, size_t pat_len) {
    uint32_t i;
    for (i = 0; i < ALPHABET_LEN; i++) {
        delta1[i] = pat_len;
    }
    for (i = 0; i < pat_len - 1; i++) {
        uint8_t idx = (uint8_t)pat[i];
        delta1[idx] = pat_len - 1 - i;
    }
}

/* Generate table of next possible full match from mismatch at pat[p] */
static void bm_make_delta2(int* delta2, const char* pat, size_t pat_len) {
    int p;
    uint32_t last_prefix = pat_len - 1;

    for (p = pat_len - 1; p >= 0; p--) {
        /* Compare whether pat[p-pat_len] is suffix of pat */
        if (strncmp(pat + p, pat, pat_len - p) == 0) {
            last_prefix = p + 1;
        }
        delta2[p] = last_prefix + (pat_len - 1 - p);
    }

    for (p = 0; p < (int)pat_len - 1; p++) {
        /* Get longest suffix of pattern ending on character pat[p] */
        int suf_len = max_suffix_len(pat, pat_len, p);
        if (pat[p - suf_len] != pat[pat_len - 1 - suf_len]) {
            delta2[pat_len - 1 - suf_len] = pat_len - 1 - p + suf_len;
        }
    }
}

static char* bm_search(const char* str, size_t str_len, const char* pat, size_t pat_len) {
    int delta1[ALPHABET_LEN];
    int delta2[pat_len];
    int i;

    bm_make_delta1(delta1, pat, pat_len);
    bm_make_delta2(delta2, pat, pat_len);

    if (pat_len == 0) {
        return (char*)str;
    }

    i = pat_len - 1;
    while (i < (int)str_len) {
        int j = pat_len - 1;
        while (j >= 0 && (str[i] == pat[j])) {
            i--;
            j--;
        }
        if (j < 0) {
            return (char*)(str + i + 1);
        }
        i += MAX(delta1[(uint8_t)str[i]], delta2[j]);
    }

    return NULL;
}

static int get_info(char* str, size_t len, char* lookup_str, size_t lookup_str_len,
                    char* part_path) {
    int ret = 0;
    int fd;
    off64_t size;
    char* data = NULL;
    char* offset = NULL;

    fd = open(part_path, O_RDONLY);
    if (fd < 0) {
        ret = errno;
        goto err_ret;
    }

    size = lseek64(fd, 0, SEEK_END);
    if (size == -1) {
        ret = errno;
        goto err_fd_close;
    }

    data = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == (char*)-1) {
        ret = errno;
        goto err_fd_close;
    }

    /* Do Boyer-Moore search across data */
    offset = bm_search(data, size, lookup_str, lookup_str_len);
    if (offset != NULL) {
        snprintf(str, len, "%s", offset + lookup_str_len);
    } else {
        ret = -ENOENT;
    }

    munmap(data, size);
err_fd_close:
    close(fd);
err_ret:
    return ret;
}

/* verify_modem("MODEM_VERSION", "MODEM_VERSION", ...) */
Value* VerifyModemFn(const char* name, State* state,
                     const std::vector<std::unique_ptr<Expr>>& argv) {
    char current_modem_version[MODEM_VER_BUF_LEN];
    int ret;
    struct tm tm1, tm2;

    ret = get_info(current_modem_version, MODEM_VER_BUF_LEN, MODEM_VER_STR, MODEM_VER_STR_LEN,
                   MODEM_PART_PATH);
    if (ret) {
        return ErrorAbort(state, kVendorFailure,
                          "%s() failed to read current MODEM build time-stamp: %d", name, ret);
    }

    std::vector<std::string> args;
    if (!ReadArgs(state, argv, &args)) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() error parsing arguments", name);
    }

    memset(&tm1, 0, sizeof(tm));
    strptime(current_modem_version, "%Y-%m-%d %H:%M:%S", &tm1);

    for (auto& modem_version : args) {
        memset(&tm2, 0, sizeof(tm));
        strptime(modem_version.c_str(), "%Y-%m-%d %H:%M:%S", &tm2);

        if (mktime(&tm1) >= mktime(&tm2)) {
            ret = 1;
            break;
        }
    }

    return StringValue(strdup(ret ? "1" : "0"));
}

/* verify_vendor("VENDOR_DATE", "VNDK_VERSION") */
Value* VerifyVendorFn(const char* name, State* state,
                      const std::vector<std::unique_ptr<Expr>>& argv) {
    char current_vendor_date[VENDOR_DATE_BUF_LEN];
    char current_vndk_version[VNDK_VER_BUF_LEN];
    int ret;

    // Check for UTC build date
    ret = get_info(current_vendor_date, VENDOR_DATE_BUF_LEN, VENDOR_DATE_STR, VENDOR_DATE_STR_LEN,
                   VENDOR_PART_PATH);
    if (ret) {
        return ErrorAbort(state, kVendorFailure,
                          "%s() failed to read current vendor UTC build date: %d", name, ret);
    }

    std::vector<std::string> args;
    if (!ReadArgs(state, argv, &args)) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() error parsing arguments", name);
    }

    if (std::stoi(current_vendor_date) < std::stoi(args[0])) {
        // Fail immediately if build is older than required
        return StringValue(strdup("0"));
    }

    // Check for VNDK version
    ret = get_info(current_vndk_version, VNDK_VER_BUF_LEN, VNDK_VER_STR, VNDK_VER_STR_LEN,
                   VENDOR_PART_PATH);
    if (ret) {
        return ErrorAbort(state, kVendorFailure,
                          "%s() failed to read current vendor VNDK version: %d", name, ret);
    }

    if (std::stoi(current_vndk_version) == std::stoi(args[1])) {
        ret = 1;
    }
    return StringValue(strdup(ret ? "1" : "0"));
}

void Register_librecovery_updater_xiaomi() {
    RegisterFunction("xiaomi.verify_modem", VerifyModemFn);
    RegisterFunction("xiaomi.verify_vendor", VerifyVendorFn);
}
