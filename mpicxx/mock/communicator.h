#pragma once

#include "mpicxx/common/types.h"
#include <cassert>
#include <map>
#include <cstring>
#include <stdexcept>
#include <utility>

#include <mpicxx/common/communicator.h>
#include "environment.h"
#include "types.h"

namespace mpi {

// Mock implementation for MPI_ENABLED = false
template<Os OS>
class basic_communicator<OS, false> {
  public:

    using status = basic_status<OS, false>;
    using environment = basic_environment<OS, false>;
    
    using size_type = typename typedefs<OS, false>::size_type;
    using id_type = typename typedefs<OS, false>::id_type;
    using tag_type = typename typedefs<OS, false>::tag_type;
    using handle_type = void*;

    explicit basic_communicator(handle_type c)
        : communicator_handle(c)
    {
        basic_environment<OS, false>::initialize();
    }
    
    basic_communicator(basic_communicator const& other) 
        : communicator_handle(other.communicator_handle)
    {
    }

    [[nodiscard]]
    static basic_communicator get_default()
    {
        return basic_communicator{0};
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept {
        environment::assert_running(); 
        return 1;
    }

    [[nodiscard]]
    constexpr size_type rank() const noexcept {
        environment::assert_running(); 
        return 0;
    }
    
    [[nodiscard]]
    static std::string processor_name() noexcept {
        return "MockMpiProcessor";
    }

    constexpr void barrier() const noexcept { }

    template<mpi::ValidType T>
    void send([[maybe_unused]] id_type destination, tag_type, T&) {
        assert(destination == 0);
        throw std::runtime_error("A rank cannot send a message to itself");
    }

    template<mpi::ValidContainer T>
    void send([[maybe_unused]] id_type destination, tag_type, T&) const {
        assert(destination == 0);
        throw std::runtime_error("A rank cannot send a message to itself");
    }

    template<mpi::ValidType T>
    void recv([[maybe_unused]] id_type source, tag_type, T&, status&) const  {
        environment::assert_running();
        assert(source == 0);
        throw std::runtime_error("A rank cannot get a message from itself");
    }

    template<mpi::ValidContainer T>
    void recv([[maybe_unused]] id_type source, tag_type, T&, status&) const {
        environment::assert_running();
        assert(source == 0);
        throw std::runtime_error("A rank cannot get a message from itself");
    }

    template<mpi::ValidType T>
    constexpr void broadcast([[maybe_unused]] id_type source, T&) const {
        environment::assert_running();
        assert(source == rank());
    }

    template<mpi::ValidContainer T>
    void broadcast([[maybe_unused]] id_type source, T&) const {
        environment::assert_running();
        assert(source == rank());
    }
    
    template<mpi::ValidContainer C>
    void gather([[maybe_unused]] id_type destination, typename container_traits<C>::data data, C& output) const {        
        environment::assert_running();
        assert (rank() == destination);

        container_traits<C>::try_resize(output, static_cast<std::size_t>(size()));
        container_traits<C>::front(output) = data;
    }
    
    template<mpi::ValidContainer C>
    void gather([[maybe_unused]] id_type destination, C const& data, C& output) const {
        environment::assert_running();
        assert (rank() == destination);

        const std::size_t msg_size = data.size();
        container_traits<C>::try_resize(output, msg_size);
        memcpy(container_traits<C>::pointer(output), container_traits<C>::pointer(data), msg_size * sizeof(typename container_traits<C>::data));
    }

protected:
    handle_type handle() const noexcept {
        return communicator_handle;
    }

private:
    handle_type communicator_handle;
};


enum openmode : int {
    create = 1,
    in = 2,
    out = 4,
    io   = 8,
    tmp = 16,
    unique = 32,
    excl = 64,
    app = 128,
    seq = 256
};

constexpr openmode operator|(openmode L, openmode R) {
    using integer = std::underlying_type_t<openmode>;
    return static_cast<openmode>(static_cast<integer>(L) | static_cast<integer>(R));
}


}

