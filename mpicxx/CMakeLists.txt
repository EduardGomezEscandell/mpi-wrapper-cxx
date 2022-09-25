add_library(mpicxx INTERFACE)
target_link_libraries(mpicxx PUBLIC ${MPI_CXXq})
target_include_directories(mpicxx INTERFACE ..)

include_directories(common)
include_directories(mock)
include_directories(linux)