#include <iostream>
#include <sstream>

#include "mpi_interface.h"

int main() {

    // Showing barrier
    if (Mpi::rank() == Mpi::size()-1) {
        /* We make the last rank print, this way we'll see if the
         barrier fails (as rank 0 tends win data races) */
        std::cout << "Using MPI processor " << Mpi::processor_name() << std::endl;
    }
    Mpi::barrier();

    // Showing async prints
    {
        std::string greeting = (std::stringstream{} << "Rank "<< Mpi::rank() << " says hello!\n").str();
        std::cout << greeting << std::flush;
    }
    Mpi::barrier();

    // Showing broadcast
    {
        int data = Mpi::rank() + 5;
        const auto orig = data;
        Mpi::broadcast(0, data);
        std::string msg = (std::stringstream{} << "Rank "<< Mpi::rank() << " got " << orig << " -> " << data << "\n").str();
        std::cout << msg << std::flush;
    }
    Mpi::barrier();

    // Showing array broadcast
    {
        std::vector<double> data {Mpi::rank() + 3.141592, Mpi::rank() + 2.71828, Mpi::rank() + 1.61803};
        const auto orig = data;
        Mpi::broadcast(Mpi::size() - 1, data);
        std::string msg = (std::stringstream{} << "Rank "<< Mpi::rank() << " got [" 
            << orig[0] << ", " << orig[1] << ", " << orig[2] << "] -> ["
            << data[0] << ", " << data[1] << ", " << data[2] << "]\n").str();
        std::cout << msg << std::flush;
    }
    Mpi::barrier();

}