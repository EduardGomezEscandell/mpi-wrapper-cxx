#include "mpi_base.h"
#include "mpi_mock.h"
#include "mpi_linux.h"

namespace mpi {



using handle_type = handle_selector<os(), mpi_enabled()>::handle;
using communicator = basic_communicator<os(), mpi_enabled()>;
using status = basic_status<os(), mpi_enabled()>;
using environment = basic_environment<os(), mpi_enabled()>;

}