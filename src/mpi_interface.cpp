#include "mpi_interface.h"

#if defined(PLATFORM_IS_LINUX) && MPI_ENABLED

using MpiLinux = MpiWrapper<Os::Linux, true>;


MpiLinux::mpi_env MpiLinux::env {};

std::string MpiLinux::processor_name() noexcept {
    char name[MPI_MAX_PROCESSOR_NAME];
    int len;
    MPI_Get_processor_name(name, &len);
    return name;
}

MpiLinux::size_type MpiLinux::size() noexcept {
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    return static_cast<std::size_t>(world_size);
}

MpiLinux::id_type MpiLinux::rank() noexcept {
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    return world_rank;
}

void MpiLinux::barrier() noexcept
{
    MPI_Barrier(MPI_COMM_WORLD);
}

#endif