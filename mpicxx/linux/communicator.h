#pragma once

#include <mpicxx/common/defines.h>

// Real implementation for MPI_ENABLED==true in Linux
#if defined(PLATFORM_IS_LINUX) && MPI_ENABLED


#include <ios>
#include <type_traits>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <memory>

#include <mpi.h>

#include <mpicxx/common/extra_type_traits.h>
#include <mpicxx/common/communicator.h>

#include "environment.h"
#include "types.h"

namespace mpi {

template<>
class basic_communicator<Os::Linux, true> {
  public:
    using size_type = typename typedefs<Os::Linux, true>::size_type;
    using id_type = typename typedefs<Os::Linux, true>::id_type;
    using tag_type = typename typedefs<Os::Linux, true>::tag_type;

    using status = basic_status<Os::Linux, true>;
    using environment = basic_environment<Os::Linux, true>;
    using handle_type = MPI_Comm;

    explicit basic_communicator(MPI_Comm c)
        : communicator_handle(c)
    {
        environment::initialize();
    }
    
    basic_communicator(basic_communicator const& other) 
        : communicator_handle(other.communicator_handle)
    {
    }

    [[nodiscard]]
    static basic_communicator get_default()
    {
        return basic_communicator{MPI_COMM_WORLD};
    }

    [[nodiscard]]
    static std::string processor_name() noexcept 
    {
        char name[MPI_MAX_PROCESSOR_NAME];
        int len;
        MPI_Get_processor_name(name, &len);
        return name;
    }

    [[nodiscard]]
    size_type size() const noexcept
    {
        environment::assert_running();
        int world_size;
        MPI_Comm_size(handle(), &world_size);
        return world_size;
    }

    [[nodiscard]]
    id_type rank() const noexcept
    {
        environment::assert_running();
        int world_rank;
        MPI_Comm_rank(handle(), &world_rank);
        return world_rank;
    }

    void barrier() const noexcept
    {
        environment::assert_running();
        MPI_Barrier(handle());
    }

    template<mpi::ValidType T>
    void send(id_type destination, tag_type tag, T& data) {
        environment::assert_running();
        MPI_Send(&data, 1u, get_datatype<Os::Linux, true, T>(), destination, tag, handle());
    }

    template<mpi::ValidContainer T>
    void send(id_type destination, tag_type tag, T& data) const {
        environment::assert_running();
        MPI_Send(container_traits<T>::pointer(data),
            static_cast<size_type>(container_traits<T>::size(data)),
            get_datatype<Os::Linux, true, typename container_traits<T>::data>(),
            destination, tag, handle());
    }

    template<mpi::ValidType T>
    void recv(id_type source, tag_type tag, T& data, status& status) const {
        environment::assert_running();
        MPI_Recv(&data, 1u, get_datatype<Os::Linux, true, T>(), source, tag, handle(), &status.base());
    }

    template<mpi::ValidContainer T>
    void recv(id_type source, tag_type tag, T& data, status& status) const {
        environment::assert_running();
        MPI_Recv(container_traits<T>::pointer(data),
            static_cast<size_type>(container_traits<T>::size(data)),
            get_datatype<Os::Linux, true, typename container_traits<T>::data>(),
            source, tag, handle(), &status.base());
    }

    template<mpi::ValidType T>
    void broadcast(id_type source, T& data) const {
        environment::assert_running();
        MPI_Bcast(&data, 1u, get_datatype<Os::Linux, true, T>(), source, handle());
    }

    template<mpi::ValidContainer T>
    void broadcast(id_type source, T& data) const {
        environment::assert_running();
        MPI_Bcast(container_traits<T>::pointer(data),
            static_cast<size_type>(container_traits<T>::size(data)),
            get_datatype<Os::Linux, true, typename container_traits<T>::data>(),
            source,
            handle());
    }

    template<mpi::ValidContainer C>
    void gather(id_type destination, typename container_traits<C>::data data, C& output) const noexcept {
        environment::assert_running(); 
        using T = typename container_traits<C>::data;
        T* recv = nullptr;
        
        if (rank() == destination) {
            container_traits<C>::try_resize(output, size());
            recv = container_traits<C>::pointer(output);
        }

        MPI_Gather(&data, 1, get_datatype<Os::Linux, true, T>(),
                    recv, 1, get_datatype<Os::Linux, true, T>(),
                    destination, handle());
    }

    template<mpi::ValidContainer C>
    void gather(id_type destination, C const& data, C& output) const noexcept {
        environment::assert_running();
        using T = typename container_traits<C>::data;
        
        const size_type msg_size = static_cast<size_type>(container_traits<C>::size(data));
        T const* send_ptr = container_traits<C>::pointer(data);
        
        if(rank() == destination) {
            container_traits<C>::try_resize(output, msg_size * size());
        }
        T* recv_ptr = container_traits<C>::pointer(output);

        MPI_Gather(send_ptr, msg_size, get_datatype<Os::Linux, true, T>(),
                   recv_ptr, msg_size, get_datatype<Os::Linux, true, T>(),
                   destination, handle());
    }

    handle_type handle() const noexcept {
        return communicator_handle;
    }

  private:
    handle_type communicator_handle;
};

}

#endif