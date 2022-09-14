#!/bin/bash

mkdir -p build
mkdir -p bin
cd build

mpicxx "../src/main.cpp" \
    -Werror -Wall -Wextra -Wpedantic -Wconversion \
    -O3 \
    -std=c++20 \
    -o "../bin/mpi-demo"