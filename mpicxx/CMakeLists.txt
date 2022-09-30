add_library(mpicxx INTERFACE)
target_link_libraries(mpicxx INTERFACE ${MPI_CXX} ${TBBLIB})
target_include_directories(mpicxx INTERFACE ..)

include_directories(common)
include_directories(mock)
include_directories(linux)