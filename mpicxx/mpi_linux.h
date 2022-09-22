#pragma once

#include "defines.h"
#include <cassert>
#include <stdexcept>

// Real implementation for MPI_ENABLED==true in Linux
#if defined(PLATFORM_IS_LINUX) && MPI_ENABLED

#include <mpi.h>
#include "mpi_base.h"
#include <memory>

namespace mpi {

template<>
inline void basic_environment<Os::Linux, true>::initialize_impl()
{
    MPI_Init(NULL, NULL);
}

template<>
inline void basic_environment<Os::Linux, true>::finalize_impl()
{
    MPI_Finalize();
}

template<>
struct basic_status<Os::Linux, true> : MPI_Status {
    MPI_Status& base() { return *this; }
};

template<>
class basic_communicator<Os::Linux, true> {
  public:
    using status = basic_status<Os::Linux, true>;
    using environment = basic_environment<Os::Linux, true>;

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
        MPI_Send(&data, 1u, mpi_data_type<T>(), destination, tag, handle());
    }

    template<mpi::ValidContainer T>
    void send(id_type destination, tag_type tag, T& data) const {
        environment::assert_running();
        MPI_Send(&container_traits<T>::front(data),
            static_cast<size_type>(container_traits<T>::size(data)),
            mpi_data_type<typename container_traits<T>::data>(),
            destination, tag, handle());
    }

    template<mpi::ValidType T>
    void recv(id_type source, tag_type tag, T& data, status& status) const {
        environment::assert_running();
        MPI_Recv(&data, 1u, mpi_data_type<T>(), source, tag, handle(), &status.base());
    }

    template<mpi::ValidContainer T>
    void recv(id_type source, tag_type tag, T& data, status& status) const {
        environment::assert_running();
        MPI_Recv(container_traits<T>::pointer(data),
            static_cast<size_type>(container_traits<T>::size(data)),
            mpi_data_type<typename container_traits<T>::data>(),
            source, tag, handle(), &status.base());
    }

    template<mpi::ValidType T>
    void broadcast(id_type source, T& data) const {
        environment::assert_running();
        MPI_Bcast(&data, 1u, mpi_data_type<T>(), source, handle());
    }

    template<mpi::ValidContainer T>
    void broadcast(id_type source, T& data) const {
        environment::assert_running();
        MPI_Bcast(container_traits<T>::pointer(data),
            static_cast<size_type>(container_traits<T>::size(data)),
            mpi_data_type<typename container_traits<T>::data>(),
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

        MPI_Gather(&data, 1, mpi_data_type<T>(),
                    recv, 1, mpi_data_type<T>(),
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

        MPI_Gather(send_ptr, msg_size, mpi_data_type<T>(),
                   recv_ptr, msg_size, mpi_data_type<T>(),
                   destination, handle());
    }

  protected:

    handle_type handle() const noexcept {
        return communicator_handle;
    }

    template<mpi::ValidType T>
    static constexpr MPI_Datatype mpi_data_type() noexcept;

  private:
    handle_type communicator_handle;
};

template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<char>   ()            noexcept { return MPI_CHAR; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<signed char>()        noexcept { return MPI_SIGNED_CHAR; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<unsigned char>()      noexcept { return MPI_UNSIGNED_CHAR; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<wchar_t>()            noexcept { return MPI_WCHAR; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<short>()              noexcept { return MPI_SHORT; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<unsigned short>()     noexcept { return MPI_UNSIGNED_SHORT; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<int>()                noexcept { return MPI_INT; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<unsigned int>()       noexcept { return MPI_UNSIGNED; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<long>()               noexcept { return MPI_LONG; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<unsigned long>()      noexcept { return MPI_UNSIGNED_LONG; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<long long>()          noexcept { return MPI_LONG_LONG_INT; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<unsigned long long>() noexcept { return MPI_UNSIGNED_LONG_LONG; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<float>()              noexcept { return MPI_FLOAT; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<double>()             noexcept { return MPI_DOUBLE; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<long double>()        noexcept { return MPI_LONG_DOUBLE; }
template<> [[nodiscard]] constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<bool>()               noexcept { return MPI_C_BOOL; }

}

#endif