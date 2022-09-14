#include <iostream>
#include <sstream>

#include "mpi_interface.h"

template<typename...Args>
void print(Args...args) {
    std::stringstream buffer;
    ( buffer << ... << args );
    std::cout << buffer.str() << std::endl;
}

void demonstrate_multiprocessing() {
    // Showing barrier
    if (Mpi::rank() == Mpi::size()-1) {
        /* We make the last rank print, this way we'll see if the
         barrier fails (as rank 0 tends win data races) */
        print("Using MPI processor ", Mpi::processor_name());
    }
    Mpi::barrier();

    // Showing async prints
    print("Rank ", Mpi::rank(), " says hello!");
}

void demonstrate_broadcast() {
    {
        int data = Mpi::rank() + 5;
        const auto orig = data;
        Mpi::broadcast(0, data);
        print("Rank ", Mpi::rank(), " got ", orig, " -> ", data);
    }
    Mpi::barrier();
    {
        std::vector<double> data {Mpi::rank() + 3.141592, Mpi::rank() + 2.71828, Mpi::rank() + 1.61803};
        const auto orig = data;
        Mpi::broadcast(Mpi::size() - 1, data);
        print("Rank ", Mpi::rank(), " got [", orig[0], ", ", orig[1], ", ", orig[2], "] -> [", data[0], ", ", data[1], ", ", data[2], "]");
    }
}

void demonstrate_sendrecv() {
    Mpi::id_type sender = 0;
    Mpi::id_type reciever = Mpi::size() - 1;
    
    Mpi::tag_type tag = 500;
    Mpi::tag_type mock_tag = -1;
    {
        Mpi::status status;
        status.MPI_TAG = mock_tag;

        int data = Mpi::rank();
        
        if (Mpi::rank() == sender) {
            Mpi::send(reciever, tag, data);
        }
        if (Mpi::rank() == reciever) {
            Mpi::recv(sender, tag, data, status);
        }

        if(status.MPI_TAG == mock_tag) {
            print("Rank ", Mpi::rank(), " got no message.");
        } else {
            print("Rank ", Mpi::rank(), " got a message from ", status.MPI_SOURCE, ": ", data);
        }
    }
}

int main() {

    demonstrate_multiprocessing();
    Mpi::barrier();
    
    demonstrate_broadcast();
    Mpi::barrier();

    demonstrate_sendrecv();
    Mpi::barrier();

}