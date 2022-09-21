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
    const std::vector<T> send_v{T{1}, T{1}, T{1}};
    std::vector<T> recieved{};
 
    Mpi::gather(root, send_v, recieved);

    if (Mpi::rank() == root) {
        REQUIRE_EQ(recieved.size(), Mpi::size() * 3);
        for(auto& r: recieved) {
            CHECK_EQ(r, T{1});
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("VectorVectorGatherLast", T, int, unsigned, char, long long, float, double)
{
    const mpi::id_type root = Mpi::size() - 1;
    const std::vector<T> send_v{T{1}, T{1}, T{1}};
    std::vector<T> recieved{};
 
    Mpi::gather(root, send_v, recieved);

    if (Mpi::rank() == root) {
        REQUIRE_EQ(recieved.size(), Mpi::size() * 3);
        for(auto& r: recieved) {
            CHECK_EQ(r, T{1});
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
    const std::vector<T> send_v{T{1}, T{1}, T{1}};
    std::vector<T> recieved{};
 
    Mpi::gather(root, send_v, recieved);

    if (Mpi::rank() == root) {
        REQUIRE_EQ(recieved.size(), Mpi::size() * 3);
        for(auto& r: recieved) {
            CHECK_EQ(r, T{1});
        }
    } else {
        REQUIRE(recieved.empty());
    }
}