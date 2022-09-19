add_executable(mpidemo mpidemo.cpp)
target_include_directories(mpidemo INTERFACE ..)
target_link_libraries(mpidemo mpicxx)
