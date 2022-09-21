#pragma once

#include <doctest/doctest.h>

#include "mpicxx/mpi.h"

TEST_CASE_TEMPLATE("VectorSingleGatherFirst", T, int, unsigned, char, long long, float, double)
{
    const mpi::id_type root = 0;
    constexpr T send_v{1};
    std::vector<T> recieved{};
 
    Mpi::gather(root, send_v, recieved);

    if (Mpi::rank() == root) {
        REQUIRE_EQ(recieved.size(), Mpi::size());
        for(auto& r: recieved) {
            CHECK_EQ(r, send_v);
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("VectorSingleGatherLast", T, int, unsigned, char, long long, float, double)
{
    const mpi::id_type root = Mpi::size() - 1;
    constexpr T send_v{1};
    std::vector<T> recieved{};
 
    Mpi::gather(root, send_v, recieved);

    if (Mpi::rank() == root) {
        REQUIRE_EQ(recieved.size(), Mpi::size());
        for(auto& r: recieved) {
            CHECK_EQ(r, send_v);
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("VectorSingleGatherMiddle", T, int, unsigned, char, long long, float, double)
{
    if (Mpi::size() < 3 ) {
        return; // Skipping
    }

    const Mpi::id_type root = 2;
    constexpr T send_v{1};
    std::vector<T> recieved{};
 
    Mpi::gather(root, send_v, recieved);

    if (Mpi::rank() == root) {
        REQUIRE_EQ(recieved.size(), Mpi::size());
        for(auto& r: recieved) {
            CHECK_EQ(r, send_v);
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("VectorVectorGatherFirst", T, int, unsigned, char, long long, float, double)
{
    const mpi::id_type root = 0;
    const auto sent_value = static_cast<T>(Mpi::rank());
    const std::vector<T> send_v{sent_value, sent_value, sent_value};
    std::vector<T> recieved{};
 
    Mpi::gather(root, send_v, recieved);

    if (Mpi::rank() == root) {
        REQUIRE_EQ(recieved.size(), Mpi::size() * 3);
        // Checking vector looks like [0,0,0,1,1,1,2,2,2,3,3, ...]
        for(mpi::size_type i=0; i<Mpi::size(); ++i) {
            for(auto j: {0, 1, 2}) {
                const auto idx = static_cast<std::size_t>(3*i + j);
                CHECK_EQ(recieved[idx], static_cast<T>(i));
            }
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("VectorVectorGatherLast", T, int, unsigned, char, long long, float, double)
{
    const mpi::id_type root = Mpi::size() - 1;
    const auto sent_value = static_cast<T>(Mpi::rank());
    const std::vector<T> send_v{sent_value, sent_value, sent_value};
    std::vector<T> recieved{};
 
    Mpi::gather(root, send_v, recieved);

    if (Mpi::rank() == root) {
        REQUIRE_EQ(recieved.size(), Mpi::size() * 3);
        // Checking vector looks like [0,0,0,1,1,1,2,2,2,3,3, ...]
        for(mpi::size_type i=0; i<Mpi::size(); ++i) {
            for(auto j: {0, 1, 2}) {
                const auto idx = static_cast<std::size_t>(3*i + j);
                CHECK_EQ(recieved[idx], static_cast<T>(i));
            }
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("VectorVectorGatherMiddle", T, int, unsigned, char, long long, float, double)
{
    if (Mpi::size() < 3 ) {
        return; // Skipping
    }

    const Mpi::id_type root = 2;
    const auto sent_value = static_cast<T>(Mpi::rank());
    const std::vector<T> send_v{sent_value, sent_value, sent_value};
    std::vector<T> recieved{};
 
    Mpi::gather(root, send_v, recieved);

    if (Mpi::rank() == root) {
        REQUIRE_EQ(recieved.size(), Mpi::size() * 3);
        // Checking vector looks like [0,0,0,1,1,1,2,2,2,3,3, ...]
        for(mpi::size_type i=0; i<Mpi::size(); ++i) {
            for(auto j: {0, 1, 2}) {
                const auto idx = static_cast<std::size_t>(3*i + j);
                CHECK_EQ(recieved[idx], static_cast<T>(i));
            }
        }
    } else {
        REQUIRE(recieved.empty());
    }
}