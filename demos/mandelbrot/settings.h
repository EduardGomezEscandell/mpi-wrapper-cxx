#pragma once

#include <complex>
#include <filesystem>
#include <iostream>

struct settings {
    std::complex<double> center;
    std::complex<double> span;
    std::size_t img_width;
    std::size_t img_height;
    std::size_t max_iter;
    bool debug = false;
    std::filesystem::path output;

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