#pragma once

#include <mpicxx/common/defines.h>

#if defined(PLATFORM_IS_LINUX) && MPI_ENABLED

#include <mpi.h>
#include <mpicxx/common/types.h>

namespace mpi {

template<>
struct basic_status<Os::Linux, true> : MPI_Status {
    MPI_Status& base() { return *this; }
};

template<>
struct typedefs<Os::Linux, true> {
    using size_type = int;
    using id_type = int;
    using tag_type = int;
    using datatype = MPI_Datatype;
};

template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, char>   ()            noexcept { return MPI_CHAR; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, signed char>()        noexcept { return MPI_SIGNED_CHAR; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, unsigned char>()      noexcept { return MPI_UNSIGNED_CHAR; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, wchar_t>()            noexcept { return MPI_WCHAR; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, short>()              noexcept { return MPI_SHORT; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, unsigned short>()     noexcept { return MPI_UNSIGNED_SHORT; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, int>()                noexcept { return MPI_INT; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, unsigned int>()       noexcept { return MPI_UNSIGNED; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, long>()               noexcept { return MPI_LONG; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, unsigned long>()      noexcept { return MPI_UNSIGNED_LONG; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, long long>()          noexcept { return MPI_LONG_LONG_INT; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, unsigned long long>() noexcept { return MPI_UNSIGNED_LONG_LONG; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, float>()              noexcept { return MPI_FLOAT; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, double>()             noexcept { return MPI_DOUBLE; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, long double>()        noexcept { return MPI_LONG_DOUBLE; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, true>::datatype get_datatype<Os::Linux, true, bool>()               noexcept { return MPI_C_BOOL; }

}

#endif