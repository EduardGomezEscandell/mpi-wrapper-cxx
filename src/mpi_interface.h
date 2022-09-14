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
    static std::string processor_name() noexcept;
    static std::size_t size() noexcept;
    static std::size_t rank() noexcept;

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

#endif


using Mpi = MpiWrapper<os(), mpi_enabled()>;