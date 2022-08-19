/*
 * Copyright (C) 2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <src/piex.h>

namespace piex {

Error GetPreviewImageData(StreamInterface* data,
                          PreviewImageData* preview_image_data) {
    return GetPreviewImageData(data, preview_image_data, nullptr);
}

} // namespace piex
