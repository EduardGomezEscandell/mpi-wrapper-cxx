#pragma once

#include <complex>
#include <filesystem>
#include <iostream>
#include <ostream>

enum class encoding { ascii, binary };

struct settings {
    std::complex<double> center     = {-0.6, 0};
    std::complex<double> span       = {4.0, 2.25};
    std::size_t img_width           = 1920;
    std::size_t img_height          = 1080;
    bool debug                      = false;
    std::filesystem::path output    = {"output.ppm"};
    encoding encode                 = encoding::binary;
    std::string colormap            = "grayscale";
    unsigned max_iter               = 50;
    unsigned min_iter               = 0;

    // Changes span imaginary component to match the image aspect ratio
    void adjust_span() {
        const double aspect_ratio = static_cast<double>(img_height) / static_cast<double>(img_width);
        span.imag(span.real() * aspect_ratio);
    }
};

template<typename...Args>
void logline(settings const& config, bool is_debug, Args...args) {
    if(is_debug && !config.debug) {
        return;
    }

    std::stringstream buffer;
    ( buffer << ... << args );
    std::cout << buffer.str() << std::endl;
}


inline std::ostream& operator<<(std::ostream& os, encoding e) {
    switch (e) {
        case encoding::ascii: return os << "ascii";
        case encoding::binary: return os << "binary";
    }
    return os << "unknown (" << static_cast<int>(e) << ")";
}

inline std::ostream& operator<<(std::ostream& os, settings const& s) {
    return os
        << "# Settings:\n"
        << "center_real: " << s.center.real() << "\n"
        << "center_imag: " << s.center.imag() << "\n"
        << "span:        " << s.span.real() << "\n"
        << "img_width:   " << s.img_width  << "\n"
        << "img_height:  " << s.img_height << "\n"
        << "debug:       " << std::boolalpha << s.debug      << "\n"
        << "output:      " << s.output     << "\n"
        << "encoding:    " << s.encode     << "\n"
        << "colormap:    " << s.colormap   << "\n"
        << "max_iter:    " << s.max_iter   << "\n"
        << "min_iter:    " << s.min_iter   << "\n"
    ;
}
