# pragma once

#include "extra_type_traits.h"
#include "defines.h"

namespace mpi {

template<typename T>
concept ValidType = AnyOf<T,
    short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long,
    float, double, long double, char, signed char, unsigned char, wchar_t, bool>;

template<typename C>
concept ValidContainer = ContiguousContainer<C> && ValidType<typename container_traits<C>::data>;

template<Os OS, bool MpiEnabled>
struct basic_status;

template<Os OS, bool MpiEnabled>
struct typedefs;

template<Os OS, bool MpiEnabled, ValidType T>
constexpr typename typedefs<OS, MpiEnabled>::datatype get_datatype() noexcept;
}