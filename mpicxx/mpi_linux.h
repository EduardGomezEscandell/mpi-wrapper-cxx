#pragma once

#include "defines.h"

// Real implementation for MPI_ENABLED==true in Linux
#if defined(PLATFORM_IS_LINUX) && MPI_ENABLED

#include <mpi.h>
#include "mpi_base.h"

namespace mpi {

// RAII class to initialize and finalize MPI
struct mpi_env {
    mpi_env() noexcept {
        MPI_Init(NULL, NULL);
    }

    ~mpi_env() noexcept {
        MPI_Finalize();
    }

    static void init() {
        static mpi_env env{};
    }
};

template<>
struct basic_status<Os::Linux, true> : MPI_Status {
    MPI_Status& base() { return *this; }
};

template<>
class basic_communicator<Os::Linux, true> {
  public:
    using status = basic_status<Os::Linux, true>;

    explicit basic_communicator(MPI_Comm c)
        : communicator_handle(c)
    {
        mpi_env::init();
    }
    
    basic_communicator(basic_communicator& other) 
        : communicator_handle(other.communicator_handle)
    {
    }

    static basic_communicator get_default()
    {
        return basic_communicator{MPI_COMM_WORLD};
    }

    static std::string processor_name() noexcept 
    {
        char name[MPI_MAX_PROCESSOR_NAME];
        int len;
        MPI_Get_processor_name(name, &len);
        return name;
    }

    size_type size() const noexcept
    {
        int world_size;
        MPI_Comm_size(handle(), &world_size);
        return world_size;
    }

    id_type rank() const noexcept
    {
        int world_rank;
        MPI_Comm_rank(handle(), &world_rank);
        return world_rank;
    }

    void barrier() const noexcept
    {
        MPI_Barrier(handle());
    }

    template<mpi::ValidType T>
    void send(id_type destination, tag_type tag, T& data) {
        MPI_Send(&data, 1u, mpi_data_type<T>(), destination, tag, handle());
    }

    template<mpi::ValidContainer T>
    void send(id_type destination, tag_type tag, T& data) const {
        MPI_Send(&container_traits<T>::front(data),
            static_cast<size_type>(container_traits<T>::size(data)),
            mpi_data_type<typename container_traits<T>::data>(),
            destination, tag, handle());
    }

    template<mpi::ValidType T>
    void recv(id_type source, tag_type tag, T& data, status& status) const {
        MPI_Recv(&data, 1u, mpi_data_type<T>(), source, tag, handle(), &status.base());
    }

    template<mpi::ValidContainer T>
    void recv(id_type source, tag_type tag, T& data, status& status) const {
        MPI_Recv(container_traits<T>::pointer(data),
            static_cast<size_type>(container_traits<T>::size(data)),
            mpi_data_type<typename container_traits<T>::data>(),
            source, tag, handle(), &status.base());
    }

    template<mpi::ValidType T>
    void broadcast(id_type source, T& data) const {
        MPI_Bcast(&data, 1u, mpi_data_type<T>(), source, handle());
    }

    template<mpi::ValidContainer T>
    void broadcast(id_type source, T& data) const {
        MPI_Bcast(container_traits<T>::pointer(data),
            static_cast<size_type>(container_traits<T>::size(data)),
            mpi_data_type<typename container_traits<T>::data>(),
            source,
            handle());
    }

    template<mpi::ValidContainer C>
    void gather(id_type destination, typename container_traits<C>::data data, C& output) const noexcept {        
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

template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<char>   ()            noexcept { return MPI_CHAR; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<signed char>()        noexcept { return MPI_SIGNED_CHAR; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<unsigned char>()      noexcept { return MPI_UNSIGNED_CHAR; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<wchar_t>()            noexcept { return MPI_WCHAR; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<short>()              noexcept { return MPI_SHORT; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<unsigned short>()     noexcept { return MPI_UNSIGNED_SHORT; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<int>()                noexcept { return MPI_INT; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<unsigned int>()       noexcept { return MPI_UNSIGNED; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<long>()               noexcept { return MPI_LONG; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<unsigned long>()      noexcept { return MPI_UNSIGNED_LONG; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<long long>()          noexcept { return MPI_LONG_LONG_INT; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<unsigned long long>() noexcept { return MPI_UNSIGNED_LONG_LONG; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<float>()              noexcept { return MPI_FLOAT; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<double>()             noexcept { return MPI_DOUBLE; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<long double>()        noexcept { return MPI_LONG_DOUBLE; }
template<> constexpr MPI_Datatype basic_communicator<Os::Linux, true>::mpi_data_type<bool>()               noexcept { return MPI_C_BOOL; }

}

#endif