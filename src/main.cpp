#include <iostream>
#include <sstream>

#include "defines.h"
#include "mpi_interface.h"

int main() {
    if (Mpi::rank() == Mpi::size()-1) {
        /* We make the last rank print, this way we'll see if the
         barrier fails (as rank 0 tends win data races) */
        std::cout << "Using MPI processor " << Mpi::processor_name() << std::endl;
    }
    Mpi::barrier();

    {
        std::string greeting = (std::stringstream{} << "Rank "<< Mpi::rank() << " says hello!\n").str();
        std::cout << greeting;
    }
    Mpi::barrier();

    {
        int data = Mpi::rank() + 5;
        Mpi::broadcast(0, data);
        std::string msg = (std::stringstream{} << "Rank "<< Mpi::rank() << " got " << data << "\n").str();
        std::cout << msg;
    }
    Mpi::barrier();

}