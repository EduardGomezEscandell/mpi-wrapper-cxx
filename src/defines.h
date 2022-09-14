#pragma once

enum class Os {
    Windows, Linux
};

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define PLATFORM Os::windows
#elif __linux__
    #define PLATFORM Os::Linux
#else
    error "Unsupported platform"
#endif

constexpr Os os() noexcept {
    return PLATFORM;
}