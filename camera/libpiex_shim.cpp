#include <src/piex.h>

namespace piex {

using image_type_recognition::RawImageTypes;

extern "C" void
    _ZN4piex19GetPreviewImageDataEPNS_15StreamInterfaceEPNS_16PreviewImageDataEPNS_22image_type_recognition13RawImageTypesE(
    StreamInterface* data, PreviewImageData* preview_image_data, RawImageTypes* output_type);


extern "C" void _ZN4piex19GetPreviewImageDataEPNS_15StreamInterfaceEPNS_16PreviewImageDataE(
    StreamInterface* data, PreviewImageData* preview_image_data) {
	return _ZN4piex19GetPreviewImageDataEPNS_15StreamInterfaceEPNS_16PreviewImageDataEPNS_22image_type_recognition13RawImageTypesE(data, preview_image_data, nullptr);
}

} // namespace piex
