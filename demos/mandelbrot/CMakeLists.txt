add_executable(mandelbrot_demo mandelbrot.cpp)
target_include_directories(mandelbrot_demo INTERFACE ..)
target_link_libraries(mandelbrot_demo mpicxx ${MPI_C})
