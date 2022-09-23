add_executable(mandelbrot mandelbrot.cpp fileIO.cpp)
target_include_directories(mandelbrot INTERFACE ..)
target_link_libraries(mandelbrot mpicxx ${MPI_C})
