#include <string>
#include "defines.h"

#if MPI_ENABLED==true
#include <mpi.h>
#endif

template<Os OS, bool MpiEnabled>
class MpiWrapper {};


template<Os OS>
class MpiWrapper<OS, false> {
  public:

    static constexpr std::size_t size() noexcept {
        return 1;
    }

    static constexpr std::size_t rank() noexcept {
        return 0;
    }
    
    static constexpr std::string processor_name() noexcept {
        return "MockMpiProcessor";
    }

};

#if defined(PLATFORM_IS_LINUX) && MPI_ENABLED

template<>
class MpiWrapper<Os::Linux, true> {
  public:
    static std::string processor_name() noexcept {
        char name[MPI_MAX_PROCESSOR_NAME];
        int len;
        MPI_Get_processor_name(name, &len);
        return name;
    }

    static std::size_t size() noexcept {
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        return static_cast<std::size_t>(world_size);
    }

    static std::size_t rank() noexcept {
        int world_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        return world_rank;
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

};

inline MpiWrapper<Os::Linux, true>::mpi_env MpiWrapper<Os::Linux, true>::env {};

#endif


using Mpi = MpiWrapper<os(), mpi_enabled()>;