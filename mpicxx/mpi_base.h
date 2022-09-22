#pragma once

#include "defines.h"
#include "extra_type_traits.h"

namespace mpi {

using id_type = int;
using size_type = int;
using tag_type = int;
using handle_type = int;

template<Os OS, bool MpiEnabled>
class basic_communicator;

template<Os OS, bool MpiEnabled>
struct basic_status;

template<typename T>
concept ValidType = AnyOf<T,
    short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long,
    float, double, long double, char, signed char, unsigned char,   wchar_t, bool>;

template<typename C>
concept ValidContainer = ContiguousContainer<C> && ValidType<typename container_traits<C>::data>;

}