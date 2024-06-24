#include <array>
#include <fstream>
#include <iostream>
#include <string_view>

#include "ppm_image.h"

namespace img_lib {
    using namespace std::literals;

    static const std::string_view PPM_SIG = "P6"sv;
    static const int PPM_MAX = 255;

    bool SavePPM(const Path& file, const Image& image) {
        std::ofstream out(file, std::ios::binary);

        out << PPM_SIG << '\n' << image.GetWidth() << ' ' << image.GetHeight() << '\n' << PPM_MAX << '\n';

        const int w = image.GetWidth();
        const int h = image.GetHeight();
        std::vector<char> buff(w * 3);

        for (int y = 0; y < h; ++y) {
            const Color* line = image.GetLine(y);
            for (int x = 0; x < w; ++x) {
                buff[x * 3 + 0] = static_cast<char>(line[x].r);
                buff[x * 3 + 1] = static_cast<char>(line[x].g);
                buff[x * 3 + 2] = static_cast<char>(line[x].b);
            }
            out.write(buff.data(), w * 3);
        }

        return out.good();
    }

    Image LoadPPM(const Path& file) {
        std::ifstream ifs(file, std::ios::binary);
        if (!ifs) {
            std::cerr << "Input file was not opened" << std::endl;
            return {};
        }

        std::string sign;
        int w, h, color_max;

        ifs >> sign >> w >> h >> color_max;

        if (sign != PPM_SIG || color_max != PPM_MAX) {
            return {};
        }

        const char next = ifs.get();
        if (next != '\n') {
            return {};
        }

        Image result(w, h, Color::Black());
        std::vector<char> buff(w * 3);

        for (int y = 0; y < h; ++y) {
            Color* line = result.GetLine(y);
            ifs.read(buff.data(), w * 3);

            for (int x = 0; x < w; ++x) {
                line[x].r = static_cast<std::byte>(buff[x * 3 + 0]);
                line[x].g = static_cast<std::byte>(buff[x * 3 + 1]);
                line[x].b = static_cast<std::byte>(buff[x * 3 + 2]);
            }
        }

        return result;
    }

} // namespace img_lib