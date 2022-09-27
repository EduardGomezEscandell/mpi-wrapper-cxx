#pragma once

#include <fstream>

#include "doctest/doctest.h"
#include "mpicxx/mpicxx.h"

#include "testutils.h"

TEST_CASE("Barrier")
{
    auto comm = mpi::communicator::get_default();
    
    const tmpdir directory {"tmp/test/Barrier"};
    auto tmpfile = [&](auto r) { return directory.path() / std::to_string(r); };
    comm.barrier();
    
    std::ofstream(tmpfile(comm.rank())).flush();
    wait_until([&](){ return std::filesystem::exists(tmpfile(comm.rank())); }, std::chrono::milliseconds{100});
    
    comm.barrier();
    for(auto i=0; comm.rank()==0 && i<comm.size(); ++i) {
        CHECK(std::filesystem::exists(tmpfile(i)));
    }
}