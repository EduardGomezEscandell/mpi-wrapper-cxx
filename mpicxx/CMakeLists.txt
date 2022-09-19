add_library(mpicxx INTERFACE)
target_link_libraries(mpicxx PUBLIC ${MPI_C})
target_include_directories(mpicxx INTERFACE ..)
