#pragma once

#include <mpicxx/common/defines.h>

#if defined(PLATFORM_IS_LINUX) && MPI_ENABLED

#include <ios>
#include <mpi.h>

#include <mpicxx/common/file.h>

#include "types.h"
#include "communicator.h"

namespace mpi {

template<>
class basic_file<Os::Linux, true> {
    auto get_amode(std::ios::openmode m) {
        constexpr int app = std::ios::app;
        constexpr int binary = std::ios::binary;
        constexpr int in = std::ios::in;
        constexpr int out = std::ios::out;
        constexpr int trunc = std::ios::trunc;
        constexpr int ate = std::ios::ate;

        int m_ = m & (~binary) & (~ate);
        switch(m_) {
            // r
            case in:              return MPI_MODE_RDONLY;
            // w
            case out:             return MPI_MODE_WRONLY | MPI_MODE_CREATE;
            // a
            case app:             [[fallthrough]];
            case out|app:         return MPI_MODE_WRONLY | MPI_MODE_CREATE | MPI_MODE_APPEND;
            // r+
            case out|in:          return MPI_MODE_RDWR;
            // w+
            case out|in|trunc:    return MPI_MODE_WRONLY | MPI_MODE_CREATE;
            // a+
            case out|in|app:      [[fallthrough]];
            case in|app:          return MPI_MODE_RDWR | MPI_MODE_CREATE | MPI_MODE_APPEND;
        }
        throw std::runtime_error("Wrong flags for file opening");
    }


public:
    using status = basic_status<Os::Linux, true>;
    using size_type = typedefs<Os::Linux, true>::size_type;

    basic_file(basic_communicator<Os::Linux, true> comm, std::string_view name, std::ios::openmode mode)
    {
        binary_ = mode & std::ios::binary;
        MPI_File_open(comm.handle(), name.data(), get_amode(mode), MPI_INFO_NULL, &handle());
    }

    ~basic_file() {
        MPI_File_close(&handle());
    }

    template<typename T>
    basic_file& operator<<(T const& other) {
        std::string buffer = (std::stringstream{} << other).str();
        status s;
        MPI_File_write(handle(),
            buffer.c_str(),
            static_cast<size_type>(buffer.size()),
            get_datatype<Os::Linux, true, char>(),
            &s);
        return *this;
    }

    template<ValidType T>
    basic_file& operator<<(T const& d) {
        status s;
        MPI_File_write(handle(), d, 1, get_datatype<Os::Linux, true, T>(), &s);
        return *this;
    }

    template<ValidContainer C>
    basic_file& operator<<(C const& c) {
        status s;
        MPI_File_write(handle(),
            typename container_traits<C>::pointer(c),
            typename container_traits<C>::size(c),
            get_datatype<Os::Linux, true, typename container_traits<C>::data>(),
            &s);
        return *this;
    }

private:
    MPI_File& handle() {
        return file_handle_;
    }

    bool binary_;
    MPI_File file_handle_;
};

}

#endif