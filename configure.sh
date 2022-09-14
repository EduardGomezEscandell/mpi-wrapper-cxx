#!/bin/bash

mkdir -p build
mkdir -p bin
cd build

export MPI_ENABLED=${MPI_ENABLED:-"false"}

if [ $MPI_ENABLED==true ]; then
    export CXX=mpicxx
else
    export CXX=g++
fi

${CXX} "../src/main.cpp"                                   \
    -Werror -Wall -Wextra -Wpedantic -Wconversion          \
    -O3                                                    \
    -std=c++20                                             \
    -o "../bin/mpidemo"                                    \
    -DMPI_ENABLED=$MPI_ENABLED

echo "Compilation finished with status $?"
   