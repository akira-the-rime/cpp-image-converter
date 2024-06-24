#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

#include "bmp_image.h"
#include "pack_defines.h"

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader {
    std::array<char, 2> sign = { 'B', 'M' };
    std::uint32_t total_size = {};
    std::uint32_t reserved_space = 0;
    std::uint32_t header_padding = {};
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    std::uint32_t info_header_size = sizeof(*this);
    std::int32_t image_width = {};
    std::int32_t image_height = {};
    std::uint16_t flat_num = 1;
    std::uint16_t bit_per_pixel = 24;
    std::uint32_t compressing_type = 0;
    std::uint32_t image_size = {};
    std::int32_t h_resolution = 11811;
    std::int32_t v_resolution = 11811;
    std::int32_t palette = 0;
    std::uint32_t significant_colors_num = 0x1000000;
}
PACKED_STRUCT_END

static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

bool SaveBMP(const Path& file, const Image& image) {
    std::ofstream out(file, std::ios::binary);
    if (!out) {
        std::cerr << "Output file was not opened" << std::endl;
        return false;
    }

    const int w = image.GetWidth();
    const int h = image.GetHeight();
    const int step = GetBMPStride(w);

    BitmapFileHeader bitmap_file_header;
    bitmap_file_header.header_padding = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    bitmap_file_header.total_size = bitmap_file_header.header_padding + step * h;

    BitmapInfoHeader bitmap_info_header;
    bitmap_info_header.image_width = w;
    bitmap_info_header.image_height = h;
    bitmap_info_header.image_size = step * h;

    out.write(reinterpret_cast<const char*>(&bitmap_file_header), sizeof(bitmap_file_header));
    out.write(reinterpret_cast<const char*>(&bitmap_info_header), sizeof(bitmap_info_header));

    std::vector<char> buff(step, 0x00000000);

    for (int y = h - 1; y >= 0; --y) {
        const Color* line = image.GetLine(y);

        for (int x = 0; x < w; ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }

        out.write(reinterpret_cast<const char*>(buff.data()), buff.size());
    }

    return out.good();
}

Image LoadBMP(const Path& file) {
    std::ifstream ifs(file, std::ios::binary);
    if (!ifs) {
        std::cerr << "Input file was not opened" << std::endl;
        return {};
    }

    const int skip = sizeof(BitmapFileHeader) + sizeof(std::uint32_t);
    ifs.seekg(skip, std::ios::beg);

    int w = 0, h = 0;
    ifs.read(reinterpret_cast<char*>(&w), sizeof(w));
    ifs.read(reinterpret_cast<char*>(&h), sizeof(h));
    ifs.seekg(sizeof(BitmapInfoHeader) - sizeof(std::uint32_t) - sizeof(std::int32_t) - sizeof(std::int32_t), std::ios::cur);

    Image result(w, h, Color::Black());
    int step = GetBMPStride(w);

    std::vector<char> buff(step);
    for (int y = h - 1; y >= 0; --y) {
        Color* line = result.GetLine(y);
        ifs.read(buff.data(), step);

        for (int x = 0; x < w; ++x) {
            line[x].b = static_cast<std::byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<std::byte>(buff[x * 3 + 1]);
            line[x].r = static_cast<std::byte>(buff[x * 3 + 2]);
        }
    }

    return result;
}

} // namespace img_lib