#pragma once

#include <mpicxx/common/defines.h>
#include <mpicxx/common/environment.h>

namespace mpi {

template<>
inline void basic_environment<Os::Linux, false>::initialize_impl(){ }

template<>
inline void basic_environment<Os::Windows, false>::initialize_impl(){ }

template<>
inline void basic_environment<Os::Linux, false>::finalize_impl(){ }

template<>
inline void basic_environment<Os::Windows, false>::finalize_impl(){ }

}