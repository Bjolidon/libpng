#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <vector>

extern "C" {
#include "png.h"
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
#ifdef PNG_SIMPLIFIED_READ_SUPPORTED
    if (size < 8 || !png_check_sig(data, 8)) {
        // Not a PNG file
        return 0;
    }

    png_image image;
    memset(&image, 0, sizeof image);
    image.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_memory(&image, data, size)) {
        return 0;
    }

    // Force the image format to use a palette/colormap
    image.format = PNG_FORMAT_FLAG_COLORMAP;

    // Allocate output buffer
    size_t image_size = PNG_IMAGE_SIZE(image);
    if (image_size == 0 || image_size > (1 << 20)) { // 1MB max size
        png_image_free(&image);
        return 0;
    }

    std::vector<png_byte> buffer(image_size);
    png_color palette[256]; // Max size allowed by libpng

    // Attempt to read the image and trigger png_image_read_colormap
    png_image_finish_read(&image, nullptr, buffer.data(), 0, palette);

    png_image_free(&image);
#endif
    return 0;
}
