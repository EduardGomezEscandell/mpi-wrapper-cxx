#pragma once

#include <fstream>

#include <doctest/doctest.h>

#include "testutils.h"
#include "mpicxx/mpi.h"

TEST_CASE("Barrier")
{
    const tmpdir directory {"tmp/test/Barrier"};
    auto tmpfile = [&](auto r) { return directory.path() / std::to_string(r); };
    Mpi::barrier();
    
    std::ofstream(tmpfile(Mpi::rank())).flush();
    wait_until([&](){ return std::filesystem::exists(tmpfile(Mpi::rank())); }, std::chrono::milliseconds{100});
    
    Mpi::barrier();
    for(auto i=0; Mpi::rank()==0 && i<Mpi::size(); ++i) {
        CHECK(std::filesystem::exists(tmpfile(i)));
    }
}