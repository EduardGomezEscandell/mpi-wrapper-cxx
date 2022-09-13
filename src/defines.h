#pragma once

enum class Os {
    windows, linux
};

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define PLATFORM Os::windows
#else
    #define PLATFORM Os::linux
#endif

constexpr Os os() noexcept {
    return PLATFORM;
}