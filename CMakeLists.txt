cmake_minimum_required(VERSION 3.9)

project(mpi_wsl_demo LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_ROOT}/bin/${CMAKE_BUILD_TYPE}")

message("Build type: ${CMAKE_BUILD_TYPE}")
message("Build MPI: ${MPI_ENABLED}")

add_compile_definitions(MPI_ENABLED=${MPI_ENABLED})

# Warnings
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Werror)
endif()

# Sanitizers
if (SANITIZE)
  if(MSVC)
    # TODO
  else()
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
    add_compile_options(-fconcepts)
  endif()
endif()

# Distributed-memory parallelism
if(${MPI_ENABLED} STREQUAL "true")
	find_package(MPI REQUIRED)
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
    add_definitions(-DOMPI_SKIP_MPICXX)
endif()

# Shared-memory standard parallelism
if(MSVC)
  set(TBBLIB "")
else()
  find_package(TBB REQUIRED)
  set(TBBLIB TBB::tbb)
endif()

add_subdirectory(external_libraries)
add_subdirectory(mpicxx)
add_subdirectory(demos)
add_subdirectory(test)

add_custom_target(
    copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}
    )
