#include <iostream>
#include "defines.h"
#include "mpi_interface.h"

int main() {
    std::cout << "Rank "<< Mpi::rank() << "/" << Mpi::size() - 1 <<": My name is " << Mpi::processor_name() << std::endl;
}