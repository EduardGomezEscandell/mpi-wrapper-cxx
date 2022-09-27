#pragma once

#include <mpicxx/common/types.h>

namespace mpi {

template<Os OS>
struct basic_status<OS, false>
{
    int count;
    int cancelled;
    int MPI_SOURCE;
    int MPI_TAG;
    int MPI_ERROR;
};

template<Os OS>
struct typedefs<OS, false> {
    using size_type = int;
    using id_type = int;
    using tag_type = int;

    enum class datatype {
        short_,
        unsigned_short_,
        int_,
        unsigned_int_,
        long_,
        unsigned_long_,
        long_long_,
        unsigned_long_long_,
        float_,
        double_,
        long_double_,
        char_,
        signed_char_,
        unsigned_char_,
        wchar_t_,
        bool_
    };
};


template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, char>()               noexcept { return typedefs<Os::Linux, false>::datatype::char_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, signed char>()        noexcept { return typedefs<Os::Linux, false>::datatype::signed_char_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, unsigned char>()      noexcept { return typedefs<Os::Linux, false>::datatype::unsigned_char_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, wchar_t>()            noexcept { return typedefs<Os::Linux, false>::datatype::wchar_t_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, short>()              noexcept { return typedefs<Os::Linux, false>::datatype::short_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, unsigned short>()     noexcept { return typedefs<Os::Linux, false>::datatype::unsigned_short_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, int>()                noexcept { return typedefs<Os::Linux, false>::datatype::int_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, unsigned int>()       noexcept { return typedefs<Os::Linux, false>::datatype::unsigned_int_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, long>()               noexcept { return typedefs<Os::Linux, false>::datatype::long_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, unsigned long>()      noexcept { return typedefs<Os::Linux, false>::datatype::unsigned_long_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, long long>()          noexcept { return typedefs<Os::Linux, false>::datatype::long_long_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, unsigned long long>() noexcept { return typedefs<Os::Linux, false>::datatype::unsigned_long_long_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, float>()              noexcept { return typedefs<Os::Linux, false>::datatype::float_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, double>()             noexcept { return typedefs<Os::Linux, false>::datatype::double_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, long double>()        noexcept { return typedefs<Os::Linux, false>::datatype::long_double_; }
template<> [[nodiscard]] constexpr typedefs<Os::Linux, false>::datatype get_datatype<Os::Linux, false, bool>()               noexcept { return typedefs<Os::Linux, false>::datatype::bool_; }

template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, char>()               noexcept { return typedefs<Os::Windows, false>::datatype::char_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, signed char>()        noexcept { return typedefs<Os::Windows, false>::datatype::signed_char_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, unsigned char>()      noexcept { return typedefs<Os::Windows, false>::datatype::unsigned_char_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, wchar_t>()            noexcept { return typedefs<Os::Windows, false>::datatype::wchar_t_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, short>()              noexcept { return typedefs<Os::Windows, false>::datatype::short_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, unsigned short>()     noexcept { return typedefs<Os::Windows, false>::datatype::unsigned_short_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, int>()                noexcept { return typedefs<Os::Windows, false>::datatype::int_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, unsigned int>()       noexcept { return typedefs<Os::Windows, false>::datatype::unsigned_int_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, long>()               noexcept { return typedefs<Os::Windows, false>::datatype::long_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, unsigned long>()      noexcept { return typedefs<Os::Windows, false>::datatype::unsigned_long_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, long long>()          noexcept { return typedefs<Os::Windows, false>::datatype::long_long_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, unsigned long long>() noexcept { return typedefs<Os::Windows, false>::datatype::unsigned_long_long_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, float>()              noexcept { return typedefs<Os::Windows, false>::datatype::float_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, double>()             noexcept { return typedefs<Os::Windows, false>::datatype::double_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, long double>()        noexcept { return typedefs<Os::Windows, false>::datatype::long_double_; }
template<> [[nodiscard]] constexpr typedefs<Os::Windows, false>::datatype get_datatype<Os::Windows, false, bool>()               noexcept { return typedefs<Os::Windows, false>::datatype::bool_; }


}