#include <string>
#include <cassert>

#include "defines.h"
#include <map>
#include "extra_type_traits.h"

#if MPI_ENABLED==true
    #include <mpi.h>
#endif

template<Os OS, bool MpiEnabled>
class MpiWrapper { };


template<Os OS>
class MpiWrapper<OS, false>;

namespace mpi {
    using id_type = int;
    using size_type = int;
    using tag_type = int;

    template<typename T>
    concept ValidType = AnyOf<T,
        short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long,
        float, double, long double, char, signed char, unsigned char,   wchar_t, bool>;

    template<typename C>
    concept ValidContainer = ContiguousContainer<C> && ValidType<typename container_traits<C>::data>;
} // namespace mpi

#if !MPI_ENABLED
namespace mpi {
namespace mock {

    struct mpi_status {
        int count;
        int cancelled;
        int MPI_SOURCE;
        int MPI_TAG;
        int MPI_ERROR;
    };

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
        mpi_status status;
        mock::data_types type;

    public:
        template<mpi::ValidType T>
        buffer_entry(
            mpi::id_type source,
            mpi::size_type count,
            mpi::tag_type tag,
            T& data) 
            : data{static_cast<void*>(new T{data})}
            , type{mpi_data_type<T>()}
        {
            status = mpi_status{count, 0, source, tag, 0};
        }

        buffer_entry(buffer_entry const&) = delete;
        buffer_entry& operator=(buffer_entry const&) = delete;

        buffer_entry(buffer_entry&& other) { *this = std::forward<buffer_entry>(other); }

        buffer_entry& operator=(buffer_entry&& other) {
            if(this != &other) {
                free(this->data);
                this->data = std::exchange(other.data, nullptr);
                this->type = other.type;
                this->status = other.status;
            }
            return *this;
        }

        template<typename T>
        std::pair<T const&, mpi_status const&> read() const {
            assert(mpi_data_type<T>() == type);
            return std::make_pair(*static_cast<T*>(data), status);
        }

        ~buffer_entry() {
            free(data);
        }
    };
} // namespace mock
} // namespace mpi

// Mock implementation for MPI_ENABLED = false
template<Os OS>
class MpiWrapper<OS, false> {
  public:
    using status = mpi::mock::mpi_status;
    using id_type = mpi::id_type;
    using size_type = mpi::size_type;
    using tag_type = mpi::tag_type;

    static constexpr mpi::size_type size() noexcept {
        return 1;
    }

    static constexpr mpi::size_type rank() noexcept {
        return 0;
    }
    
    static constexpr std::string processor_name() noexcept {
        return "MockMpiProcessor";
    }

    static constexpr void barrier() noexcept { }

    template<mpi::ValidType T>
    static constexpr void broadcast(id_type source, T&) {
        assert(source == rank());
    }

    template<mpi::ValidContainer T>
    static void broadcast(id_type source, T&) {
        assert(source == rank());
    }

    template<mpi::ValidType T>
    static void send(id_type destination, tag_type tag, T& data) {
        assert(destination == rank());
        
        auto it = sendrcv_buffer.find(tag);
        auto entry = mpi::mock::buffer_entry{rank(), 1u, tag, data};
        
        if(it == sendrcv_buffer.end()) {
            sendrcv_buffer.emplace(tag, std::move(entry));
        } else {
            sendrcv_buffer.at(tag) = std::move(entry);
        }
    }

    template<mpi::ValidType T>
    static void recv(id_type source, tag_type tag, T& data, status& status) {
        assert(source == rank());
        auto it = sendrcv_buffer.find(tag);
        assert(it != sendrcv_buffer.end());
        auto const& [msg, stat] = it->second.read<T>();
        data = msg;
        status = stat;
        sendrcv_buffer.erase(it);
    }

    

private:
    static std::map<tag_type, mpi::mock::buffer_entry> sendrcv_buffer;
};

template<Os OS>
inline std::map<typename MpiWrapper<OS, false>::tag_type, mpi::mock::buffer_entry> MpiWrapper<OS, false>::sendrcv_buffer{};


// Real implementation for MPI_ENABLED = true in Linux
#elif defined(PLATFORM_IS_LINUX)

template<>
class MpiWrapper<Os::Linux, true> {
  public:
    using status = MPI_Status;
    using id_type = mpi::id_type;
    using size_type = mpi::size_type;
    using tag_type = mpi::tag_type;

    static std::string processor_name() noexcept;
    static size_type size() noexcept;
    static id_type rank() noexcept;
    static void barrier() noexcept;

    template<mpi::ValidType T>
    static void broadcast(id_type source, T& data) {
        MPI_Bcast(&data, 1u, mpi_data_type<T>(), source, MPI_COMM_WORLD);
    }

    template<mpi::ValidContainer T>
    static void broadcast(id_type source, T& data) {
        MPI_Bcast(&container_traits<T>::front(data),
            static_cast<int>(container_traits<T>::size(data)),
            mpi_data_type<typename container_traits<T>::data>(),
            source,
            MPI_COMM_WORLD);
    }

    template<mpi::ValidType T>
    static void send(id_type destination, tag_type tag, T& data) {
        MPI_Send(&data, 1u, mpi_data_type<T>(), destination, tag, MPI_COMM_WORLD);
    }

    template<mpi::ValidType T>
    static void recv(id_type source, tag_type tag, T& data, status& status) {
        MPI_Recv(&data, 1u, mpi_data_type<T>(), source, tag, MPI_COMM_WORLD, &status);
    }

  protected:
    // RAII class to initialize and finilize MPI
    struct mpi_env {
        mpi_env() noexcept {
            MPI_Init(NULL, NULL);
        }

        ~mpi_env() noexcept {
            MPI_Finalize();
        }
    };

    static mpi_env env;

    template<mpi::ValidType T>
    static constexpr MPI_Datatype mpi_data_type() noexcept;
};

template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<char>   ()            noexcept { return MPI_CHAR; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<signed char>()        noexcept { return MPI_SIGNED_CHAR; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<unsigned char>()      noexcept { return MPI_UNSIGNED_CHAR; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<wchar_t>()            noexcept { return MPI_WCHAR; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<short>()              noexcept { return MPI_SHORT; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<unsigned short>()     noexcept { return MPI_UNSIGNED_SHORT; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<int>()                noexcept { return MPI_INT; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<unsigned int>()       noexcept { return MPI_UNSIGNED; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<long>()               noexcept { return MPI_LONG; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<unsigned long>()      noexcept { return MPI_UNSIGNED_LONG; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<long long>()          noexcept { return MPI_LONG_LONG_INT; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<unsigned long long>() noexcept { return MPI_UNSIGNED_LONG_LONG; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<float>()              noexcept { return MPI_FLOAT; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<double>()             noexcept { return MPI_DOUBLE; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<long double>()        noexcept { return MPI_LONG_DOUBLE; }
template<> constexpr MPI_Datatype MpiWrapper<Os::Linux, true>::mpi_data_type<bool>()               noexcept { return MPI_C_BOOL; }

#endif


using Mpi = MpiWrapper<os(), mpi_enabled()>;