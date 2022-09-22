#pragma once

#include <map>

#include "defines.h"
#include "extra_type_traits.h"

#include <cstring>
#include "mpi_base.h"

namespace mpi {

template<Os OS>
class basic_communicator<OS, false>;

template<Os OS>
struct basic_status<OS, false>
{
    int count;
    int cancelled;
    int MPI_SOURCE;
    int MPI_TAG;
    int MPI_ERROR;
};

namespace mock {
    enum class data_types {
        short_,
        unsigned_short_,
        int_,
        unsigned_int_,
        long_,
        unsigned_long_,
        long_long_,
        unsigned_long_long_,
        float_,
        double_,
        long_double_,
        char_,
        signed_char_,
        unsigned_char_,
        wchar_t_,
        bool_
    };

    template<mpi::ValidType T> constexpr data_types mpi_data_type() noexcept;
    template<> constexpr data_types mpi_data_type<char>()               noexcept { return data_types::char_; }
    template<> constexpr data_types mpi_data_type<signed char>()        noexcept { return data_types::signed_char_; }
    template<> constexpr data_types mpi_data_type<unsigned char>()      noexcept { return data_types::unsigned_char_; }
    template<> constexpr data_types mpi_data_type<wchar_t>()            noexcept { return data_types::wchar_t_; }
    template<> constexpr data_types mpi_data_type<short>()              noexcept { return data_types::short_; }
    template<> constexpr data_types mpi_data_type<unsigned short>()     noexcept { return data_types::unsigned_short_; }
    template<> constexpr data_types mpi_data_type<int>()                noexcept { return data_types::int_; }
    template<> constexpr data_types mpi_data_type<unsigned int>()       noexcept { return data_types::unsigned_int_; }
    template<> constexpr data_types mpi_data_type<long>()               noexcept { return data_types::long_; }
    template<> constexpr data_types mpi_data_type<unsigned long>()      noexcept { return data_types::unsigned_long_; }
    template<> constexpr data_types mpi_data_type<long long>()          noexcept { return data_types::long_long_; }
    template<> constexpr data_types mpi_data_type<unsigned long long>() noexcept { return data_types::unsigned_long_long_; }
    template<> constexpr data_types mpi_data_type<float>()              noexcept { return data_types::float_; }
    template<> constexpr data_types mpi_data_type<double>()             noexcept { return data_types::double_; }
    template<> constexpr data_types mpi_data_type<long double>()        noexcept { return data_types::long_double_; }
    template<> constexpr data_types mpi_data_type<bool>()               noexcept { return data_types::bool_; }


    // Type-erased buffer entry
    class buffer_entry {
        void * data = nullptr;

        using status = basic_status<os(), false>;
        status status_;

        mock::data_types type;

    public:
        template<mpi::ValidType T>
        buffer_entry(
            mpi::id_type source,
            mpi::size_type count,
            mpi::tag_type tag,
            T& first) 
            : data{malloc(count * sizeof(first))}
            , type{mpi_data_type<T>()}
        {
            memcpy(data, &first, count * sizeof(T));
            status_ = status{count, 0, source, tag, 0};
        }

        buffer_entry(buffer_entry const&) = delete;
        buffer_entry& operator=(buffer_entry const&) = delete;

        buffer_entry(buffer_entry&& other) { *this = std::forward<buffer_entry>(other); }

        buffer_entry& operator=(buffer_entry&& other) {
            if(this != &other) {
                free(this->data);
                this->data = std::exchange(other.data, nullptr);
                this->type = other.type;
                this->status_ = other.status_;
            }
            return *this;
        }

        template<typename T>
        std::pair<T*, status const&> get() const {
            assert(mpi_data_type<T>() == type);
            return {static_cast<T*>(data), status_};
        }

        ~buffer_entry() {
            free(data);
        }
    };
} // namespace mock

// Mock implementation for MPI_ENABLED = false
template<Os OS>
class basic_communicator<OS, false> {
  public:

    using status = basic_status<OS, false>;

    explicit basic_communicator(handle_type c)
        : communicator_handle(c)
    {
    }
    
    basic_communicator(basic_communicator& other) 
        : communicator_handle(other.communicator_handle)
    {
    }

    static basic_communicator get_default()
    {
        return basic_communicator{0};
    }

    constexpr mpi::size_type size() const noexcept {
        return 1;
    }

    constexpr mpi::size_type rank() const noexcept {
        return 0;
    }
    
    std::string processor_name() const noexcept {
        return "MockMpiProcessor";
    }

    constexpr void barrier() const noexcept { }

    template<mpi::ValidType T>
    void send([[maybe_unused]] id_type destination, tag_type tag, T& data) {
        assert(destination == rank());
        
        auto it = sendrcv_buffer.find(tag);
        auto entry = mpi::mock::buffer_entry{rank(), 1u, tag, data};
        
        if(it == sendrcv_buffer.end()) {
            sendrcv_buffer.emplace(tag, std::move(entry));
        } else {
            sendrcv_buffer.at(tag) = std::move(entry);
        }
    }

    template<mpi::ValidContainer T>
    void send([[maybe_unused]] id_type destination, tag_type tag, T& data) const {
        assert(destination == rank());
        
        auto it = sendrcv_buffer.find(tag);
        auto entry = mpi::mock::buffer_entry{rank(),
            static_cast<size_type>(container_traits<T>::size(data)),
            tag,
            container_traits<T>::front(data)};
        
        if(it == sendrcv_buffer.end()) {
            sendrcv_buffer.emplace(tag, std::move(entry));
        } else {
            sendrcv_buffer.at(tag) = std::move(entry);
        }
    }

    template<mpi::ValidType T>
    void recv([[maybe_unused]] id_type source, tag_type tag, T& data, status& status) const  {
        assert(source == rank());
        auto it = sendrcv_buffer.find(tag);
        assert(it != sendrcv_buffer.end());
        auto const& [msg, stat] = it->second.get<T>();
        data = *msg;
        status = stat;
        sendrcv_buffer.erase(it);
    }

    template<mpi::ValidContainer T>
    void recv([[maybe_unused]] id_type source, tag_type tag, T& data, status& status) const {
        assert(source == rank());
        auto it = sendrcv_buffer.find(tag);
        assert(it != sendrcv_buffer.end());
        
        auto const& [msg, stat] = it->second.get<typename container_traits<T>::data>();
        status = stat;
        assert(static_cast<std::size_t>(status.count) <= container_traits<T>::size(data));

        container_traits<T>::try_resize(data, status.count);
        memcpy(&container_traits<T>::front(data), msg, status.count * sizeof(T));
        sendrcv_buffer.erase(it);
    }

    template<mpi::ValidType T>
    constexpr void broadcast([[maybe_unused]] id_type source, T&) const {
        assert(source == rank());
    }

    template<mpi::ValidContainer T>
    void broadcast([[maybe_unused]] id_type source, T&) const {
        assert(source == rank());
    }
    
    template<mpi::ValidContainer C>
    void gather([[maybe_unused]] id_type destination, typename container_traits<C>::data data, C& output) const {        
        assert (rank() == destination);
        container_traits<C>::try_resize(output, size());
        container_traits<C>::front(output) = data;
    }
    
    template<mpi::ValidContainer C>
    void gather([[maybe_unused]] id_type destination, C const& data, C& output) const {
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
    mutable std::map<tag_type, mpi::mock::buffer_entry> sendrcv_buffer {};
    handle_type communicator_handle;
};

}

