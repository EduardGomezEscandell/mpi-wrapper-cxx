#include "mpi_mock.h"
#include "mpi_linux.h"

namespace mpi {
    
using communicator = basic_communicator<os(), mpi_enabled()>;
using status = basic_status<os(), mpi_enabled()>;

}