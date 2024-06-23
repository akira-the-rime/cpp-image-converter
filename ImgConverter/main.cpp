#include <filesystem>
#include <string_view>
#include <iostream>
#include <utility>

#include <bmp_image.h>
#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>

class ImageFormatInterface {
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};

class PPMFormat : public ImageFormatInterface {
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SavePPM(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadPPM(file);
    }
};

class JPEGFormat : public ImageFormatInterface {
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveJPEG(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadJPEG(file);
    }
};

class BMPFormat : public ImageFormatInterface {
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveBMP(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadBMP(file);
    }
};

enum class Format {
    PPM,
    JPEG,
    BMP,
    UNKNOWN,
};

Format GetFormatByExtension(const img_lib::Path& input_file) {
    using namespace std::literals;

    const std::string ext = input_file.extension().string();
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }

    if (ext == ".ppm"sv) {
        return Format::PPM;
    }

    if (ext == ".bmp"sv) {
        return Format::BMP;
    }

    return Format::UNKNOWN;
}

ImageFormatInterface* GetFormatInterface(const img_lib::Path& path) {
    Format format = GetFormatByExtension(path);

    switch (format) {
    case Format::PPM: {
        return new PPMFormat();
    }
    case Format::JPEG: {
        return new JPEGFormat();
    }
    case Format::BMP: {
        return new BMPFormat();
    }
    }

    return nullptr;
}

class ImageFormatInterfaceRAII {
public:
    ImageFormatInterfaceRAII() = default;
    ImageFormatInterfaceRAII(const ImageFormatInterfaceRAII& other) = delete;
    ImageFormatInterfaceRAII& operator=(const ImageFormatInterfaceRAII& other) = delete;

    ImageFormatInterfaceRAII(ImageFormatInterfaceRAII&& other) noexcept {
        this->format_interface = std::exchange(other.format_interface, nullptr);
    }

    ImageFormatInterfaceRAII& operator=(ImageFormatInterfaceRAII&& other) noexcept {
        if (this != &other) {
            this->format_interface = std::exchange(other.format_interface, nullptr);
        }

        return *this;
    }

    ImageFormatInterfaceRAII(ImageFormatInterface* other) noexcept {
        this->format_interface = std::exchange(other, nullptr);
    }

    ImageFormatInterfaceRAII& operator=(ImageFormatInterface* other) noexcept {
        if (this->format_interface != other) {
            this->format_interface = std::exchange(other, nullptr);
        }

        return *this;
    }

    ~ImageFormatInterfaceRAII() noexcept {
        delete format_interface;
    }

    operator bool() const noexcept {
        return format_interface ? true : false;
    }

    const ImageFormatInterface& operator*() const {
        return *format_interface;
    }

    ImageFormatInterface& operator*() {
        return *format_interface;
    }

    const ImageFormatInterface* operator->() const noexcept {
        return format_interface;
    }

    ImageFormatInterface* operator->() noexcept {
        return format_interface;
    }

private:
    ImageFormatInterface* format_interface = nullptr;
};

int main(int argc, const char** argv) {
    using namespace std::literals;

    if (argc != 3) {
        std::cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << std::endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];

    ImageFormatInterfaceRAII in = GetFormatInterface(in_path);
    if (!in) {
        std::cerr << "Unknown format of the input file" << std::endl;
        return 2;
    }

    ImageFormatInterfaceRAII out = GetFormatInterface(out_path);
    if (!out) {
        std::cerr << "Unknown format of the output file" << std::endl;
        return 3;
    }

    img_lib::Image image = in->LoadImage(in_path);
    if (!image) {
        std::cerr << "Loading failed"sv << std::endl;
        return 4;
    }

    if (!out->SaveImage(out_path, image)) {
        std::cerr << "Saving failed"sv << std::endl;
        return 5;
    }

    std::cout << "Successfully converted"sv << std::endl;
}