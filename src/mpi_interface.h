#include <string>
#include <cassert>

#include "defines.h"
#include "extra_type_traits.h"

#if MPI_ENABLED==true
    #include <mpi.h>
#endif

template<Os OS, bool MpiEnabled>
class MpiWrapper {
};

template<typename T>
concept ValidMpiType = AnyOf<T,
    short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long,
    float, double, long double, char, signed char, unsigned char,   wchar_t, bool>;

template<typename C>
concept ValidMpiContainer = ContiguousContainer<C> && ValidMpiType<typename container_traits<C>::data>;

// Mock implementation for MPI_ENABLED = false
template<Os OS>
class MpiWrapper<OS, false> {
  public:
    using id_type = int;     // It's gross, but it's what MPI uses
    using size_type = int;

    static constexpr size_type size() noexcept {
        return 1;
    }

    static constexpr id_type rank() noexcept {
        return 0;
    }
    
    static constexpr std::string processor_name() noexcept {
        return "MockMpiProcessor";
    }

    static constexpr void barrier() noexcept { }

    template<ValidMpiType T>
    static constexpr void broadcast(id_type source, T&) {
        assert(source == rank());
    }

    template<ValidMpiContainer T>
    static void broadcast(id_type source, T&) {
        assert(source == rank());
    }

};

#if defined(PLATFORM_IS_LINUX) && MPI_ENABLED

// Real implementation for MPI_ENABLED = true in Linux
template<>
class MpiWrapper<Os::Linux, true> {
  public:
    using id_type = int;     // It's gross, but it's what MPI uses
    using size_type = int;

    static std::string processor_name() noexcept;
    static size_type size() noexcept;
    static id_type rank() noexcept;
    static void barrier() noexcept;

    template<ValidMpiType T>
    static void broadcast(id_type source, T& data) {
        MPI_Bcast(&data, 1u, mpi_data_type<T>(), source, MPI_COMM_WORLD);
    }

    template<ValidMpiContainer T>
    static void broadcast(id_type source, T& data) {
        MPI_Bcast(&container_traits<T>::front(data),
            static_cast<int>(container_traits<T>::size(data)),
            mpi_data_type<typename container_traits<T>::data>(),
            source,
            MPI_COMM_WORLD);
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

    template<ValidMpiType T>
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