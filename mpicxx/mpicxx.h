#pragma once

#include "mpicxx/common/defines.h"

#include "mock/communicator.h"
#include "mock/environment.h"
#include "mock/types.h"

#include "linux/communicator.h"
#include "linux/environment.h"
#include "linux/types.h"

namespace mpi {

using communicator = basic_communicator<os(), mpi_enabled()>;
using status = basic_status<os(), mpi_enabled()>;
using environment = basic_environment<os(), mpi_enabled()>;

using size_type = typedefs<os(), mpi_enabled()>::size_type;
using id_type = typedefs<os(), mpi_enabled()>::id_type;
using tag_type = typedefs<os(), mpi_enabled()>::tag_type;
using type_tag = typedefs<os(), mpi_enabled()>::datatype;

}