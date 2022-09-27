add_executable(helloworld helloworld.cpp)
target_include_directories(helloworld INTERFACE ..)
target_link_libraries(helloworld mpicxx ${MPI_C})