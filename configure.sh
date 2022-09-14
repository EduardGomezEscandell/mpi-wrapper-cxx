#!/bin/bash

mkdir -p build
mkdir -p bin
cd build

export MPI_ENABLED="true"

mpicxx "../src/main.cpp"                                   \
    -Werror -Wall -Wextra -Wpedantic -Wconversion          \
    -O3                                                    \
    -std=c++20                                             \
    -o "../bin/mpi-demo"                                   \
    -DMPI_ENABLED=$MPI_ENABLED