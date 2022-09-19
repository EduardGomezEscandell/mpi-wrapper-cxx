add_executable(test test.cpp)
target_link_libraries(test PRIVATE doctest mpicxx ${MPI_C})
