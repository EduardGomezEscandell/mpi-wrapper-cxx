#pragma once
#include <type_traits>

namespace mpi {

enum class Os {
    Windows, Linux
};

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define PLATFORM_IS_WINDOWS
    #define MPI_ENABLED false
#elif __linux__
    #define PLATFORM_IS_LINUX
    #ifndef MPI_ENABLED
        #define MPI_ENABLED false
    #endif
#else
    error "Unsupported platform"
#endif

static_assert(std::is_same_v<bool, decltype(MPI_ENABLED)>, "MPI_ENABLED should be either 'true', 'false' or not defined");


constexpr Os os() noexcept {
    #ifdef PLATFORM_IS_WINDOWS
        return Os::Windows;
    #else
        return Os::Linux;
    #endif
}

constexpr bool mpi_enabled() noexcept {
    return MPI_ENABLED;
}

}