#pragma once

#include <stdexcept>

#include "defines.h"
#include "extra_type_traits.h"

namespace mpi {

template<Os OS, bool MpiEnabled>
struct comm_handle_selector;

template<Os OS, bool MpiEnabled>
class basic_communicator;


}