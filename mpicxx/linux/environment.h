# pragma once

#include <mpicxx/common/defines.h>

#if defined(PLATFORM_IS_LINUX) && MPI_ENABLED

#include <mpi.h>
#include <mpicxx/common/environment.h>

namespace mpi {

template<>
inline void basic_environment<Os::Linux, true>::initialize_impl()
{
    MPI_Init(NULL, NULL);
}

template<>
inline void basic_environment<Os::Linux, true>::finalize_impl()
{
    MPI_Finalize();
}

}

# endif