#pragma once

#include <fstream>
#include <ios>

#include <mpicxx/common/defines.h>
#include <mpicxx/common/file.h>

#include "types.h"

namespace mpi {

template<Os OS>
class basic_file<OS, false> {

public:
    using status = basic_status<OS, false>;
    using handle_type = std::fstream;

    // basic_file(basic_communicator<OS, false>, std::string_view name, std::ios::openmode mode)
    // {
    //     file_handle = std::fstream(name, mode);
    // }

    // ~basic_file() {
    //     MPI_File_close(&handle());
    // }

    // template<typename T>
    // basic_file& operator<<(T const& other) {
    //     std::string buffer = (std::stringstream{} << other).str();
    //     status s;
    //     MPI_File_write(handle(),
    //         buffer.c_str(),
    //         static_cast<size_type>(buffer.size()),
    //         internal::mpi_data_type<char>(),
    //         &s);
    //     return *this;
    // }

    // template<ValidType T>
    // basic_file& operator<<(T const& d) {
    //     status s;
    //     MPI_File_write(handle(), d, 1, internal::mpi_data_type<T>(), &s);
    //     return *this;
    // }

    // template<ValidContainer C>
    // basic_file& operator<<(C const& c) {
    //     status s;
    //     MPI_File_write(handle(),
    //         typename container_traits<C>::pointer(c),
    //         typename container_traits<C>::size(c),
    //         internal::mpi_data_type<typename container_traits<C>::data>(),
    //         &s);
    //     return *this;
    // }

private:
    handle_type& handle() {
        return file_handle;
    }

    handle_type file_handle;
};

}