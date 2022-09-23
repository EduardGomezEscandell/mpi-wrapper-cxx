#!/bin/bash -e

# Settings
export MPI_ENABLED=${MPI_ENABLED:-"false"}
export BUILD_TYPE=${BUILD_TYPE:-Release}

# Chosing compiler
if [ ${MPI_ENABLED} = "true" ]; then
    export CC=mpicc
    export CXX=mpicxx
else
    export CC=${CC:-gcc}
    export CXX=${CXX:-g++}
fi

cmake                                   \
    -S.                                 \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE}    \
    -DPROJECT_ROOT=`pwd`                \
    -B"build/${BUILD_TYPE}"             \
    -DMPI_ENABLED=$MPI_ENABLED          \
    -DCC=${CC}                          \
    -DCXX=${CXX}

cmake --build "build/${BUILD_TYPE}" -- -j$(nproc)
