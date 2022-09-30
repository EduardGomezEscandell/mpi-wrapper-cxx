add_executable(mandelbrot mandelbrot.cpp fileIO.cpp distributed_canvas.cpp maths.cpp)
target_include_directories(mandelbrot INTERFACE ..)
target_link_libraries(mandelbrot mpicxx)
