#include <iostream>
#include <iterator>
#include <sstream>
#include <algorithm>

#include "mpicxx/mpicxx.h"

auto comm = mpi::communicator::get_default();

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
    if(comm.rank() == 0) {
        std::cout << "\nDemonstrating multiprocessing" << std::endl;
    }
    comm.barrier();
    // Showing barrier
    if (comm.rank() == comm.size()-1) {
        /* We make the last rank print, this way we'll see if the
         barrier fails (as rank 0 tends win data races) */
        print("Using MPI processor ", comm.processor_name());
    }
    comm.barrier();

    // Showing async prints
    print("Rank ", comm.rank(), " says hello!");
}

void demonstrate_broadcast() {
    if(comm.rank() == 0) {
        std::cout << "\nDemonstrating broadcast" << std::endl;
    }
    comm.barrier();
    {
        int data = comm.rank() + 5;
        const auto orig = data;
        comm.broadcast(0, data);
        print("Rank ", comm.rank(), " got ", orig, " -> ", data);
    }
    comm.barrier();
    {
        std::vector<double> data {comm.rank() + 3.141592, comm.rank() + 2.71828, comm.rank() + 1.61803};
        const auto orig = data;
        comm.broadcast(comm.size() - 1, data);
        print("Rank ", comm.rank(), " got ", vector_to_str(orig), " -> ", vector_to_str(data));
    }
}

void demonstrate_sendrecv() {
    if(comm.size() < 2) {
        std::cout << "Skipping sendrecv demo" << std::endl;
        comm.barrier();
        return; // Need diferent sender and reciever!
    }
    if(comm.rank() == 0) {
        std::cout << "\nDemonstrating sendrecv" << std::endl;
    }
    comm.barrier();
    mpi::id_type sender = 0;
    mpi::id_type reciever = comm.size() - 1;
    
    mpi::tag_type tag = 500;
    mpi::tag_type mock_tag = -1;
    {
        mpi::status status{};
        status.MPI_TAG = mock_tag;

        int data = comm.rank();
        
        if (comm.rank() == sender) {
            comm.send(reciever, tag, data);
        }
        if (comm.rank() == reciever) {
            comm.recv(sender, tag, data, status);
        }

        if(status.MPI_TAG == mock_tag) {
            print("Rank ", comm.rank(), " got no message.");
        } else {
            print("Rank ", comm.rank(), " got a message from ", status.MPI_SOURCE, ": tag=", status.MPI_TAG, ", value=", data);
        }
    }
    comm.barrier();
    {
        mpi::status status{};
        status.MPI_TAG = mock_tag;

        std::vector<int> data = {comm.rank() + 20, comm.rank()+40, comm.rank()+60, comm.rank()+80};
        
        if (comm.rank() == sender) {
            comm.send(reciever, tag, data);
        }
        if (comm.rank() == reciever) {
            comm.recv(sender, tag, data, status);
        }

        if(status.MPI_TAG == mock_tag) {
            print("Rank ", comm.rank(), " got no message.");
        } else {
            print("Rank ", comm.rank(), " got a message from ", status.MPI_SOURCE, ": tag=", status.MPI_TAG, ", value=",  vector_to_str(data));
        }
    }
}

void demonstrate_gather() {
    if(comm.rank() == 0) {
        std::cout << "\nDemonstrating gather" << std::endl;
    }
    comm.barrier();
    {
        int data = comm.rank();
        std::vector<int> gathered_data{};
        comm.gather(comm.size()-1, data, gathered_data);
        print("Rank ", comm.rank(), " has data ", vector_to_str(gathered_data));
    }
    comm.barrier();
    {
        std::vector<int> data (5u, comm.rank() + 10);
        std::vector<int> gathered_data{};
        comm.gather(comm.size()-1, data, gathered_data);
        print("Rank ", comm.rank(), " sent ",  vector_to_str(data), " and recieved ", vector_to_str(gathered_data));
    }
}

int main() {
    demonstrate_multiprocessing();
    comm.barrier();
    
    demonstrate_broadcast();
    comm.barrier();

    demonstrate_sendrecv();
    comm.barrier();

    demonstrate_gather();
    comm.barrier();
}