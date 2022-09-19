#include <iostream>
#include <iterator>
#include <sstream>
#include <algorithm>

#include "mpicxx/mpi.h"

template<typename T>
std::string vector_to_str(std::vector<T> const& container) {
    std::stringstream ss;
    ss << "[ ";
    std::transform(container.cbegin(), container.cend(), std::ostream_iterator<T>(ss, " "), [](auto const& entry) {
        return entry;
    });
    ss << "]";
    return ss.str();
}

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
        print("Rank ", Mpi::rank(), " got ", vector_to_str(orig), " -> ", vector_to_str(data));
    }
}

void demonstrate_sendrecv() {
    Mpi::id_type sender = 0;
    Mpi::id_type reciever = Mpi::size() - 1;
    
    Mpi::tag_type tag = 500;
    Mpi::tag_type mock_tag = -1;
    {
        Mpi::status status{};
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
            print("Rank ", Mpi::rank(), " got a message from ", status.MPI_SOURCE, ": tag=", status.MPI_TAG, ", value=", data);
        }
    }
    Mpi::barrier();
    {
        Mpi::status status{};
        status.MPI_TAG = mock_tag;

        std::vector<int> data = {Mpi::rank() + 20, Mpi::rank()+40, Mpi::rank()+60, Mpi::rank()+80};
        
        if (Mpi::rank() == sender) {
            Mpi::send(reciever, tag, data);
        }
        if (Mpi::rank() == reciever) {
            Mpi::recv(sender, tag, data, status);
        }

        if(status.MPI_TAG == mock_tag) {
            print("Rank ", Mpi::rank(), " got no message.");
        } else {
            print("Rank ", Mpi::rank(), " got a message from ", status.MPI_SOURCE, ": tag=", status.MPI_TAG, ", value=",  vector_to_str(data));
        }
    }
}

void demonstrate_gather() {
    {
        int data = Mpi::rank();
        std::vector<int> gathered_data{};
        Mpi::gather(Mpi::size()-1, data, gathered_data);
        print("Rank ", Mpi::rank(), " has data ", vector_to_str(gathered_data));
    }
}

int main() {
    demonstrate_multiprocessing();
    Mpi::barrier();
    
    demonstrate_broadcast();
    Mpi::barrier();

    demonstrate_sendrecv();
    Mpi::barrier();

    demonstrate_gather();
    Mpi::barrier();
}