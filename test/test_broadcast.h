#pragma once

#include <doctest/doctest.h>

#include "mpicxx/mpi.h"


#include <iostream>

TEST_CASE_TEMPLATE("BroadcastFirst", T, bool, int, unsigned, char, long long, float, double)
{ 
    constexpr auto root_v = static_cast<T>(1);
    constexpr auto other_v = static_cast<T>(2);

    T data = static_cast<T>(0);
    if (Mpi::rank() == 0) {
        data = root_v;
    } else {
        data = other_v;
    }

    Mpi::broadcast(0, data);

    CHECK_EQ(data, root_v);
}

TEST_CASE_TEMPLATE("BroadcastLast", T, bool, int, unsigned, char, long long, float, double)
{ 
    constexpr auto root_v = static_cast<T>(1);
    constexpr auto other_v = static_cast<T>(2);

    T data = static_cast<T>(0);
    if (Mpi::rank() == Mpi::size() - 1) {
        data = root_v;
    } else {
        data = other_v;
    }

    Mpi::broadcast(0, data);

    CHECK_EQ(data, root_v);
}

TEST_CASE_TEMPLATE("BroadcastMiddle", T, bool, int, unsigned, char, long long, float, double)
{ 
    if (Mpi::size() < 3 ) {
        return; // Skipping
    }

    constexpr auto root_v = static_cast<T>(1);
    constexpr auto other_v = static_cast<T>(2);

    T data = static_cast<T>(0);
    if (Mpi::rank() == 2) {
        data = root_v;
    } else {
        data = other_v;
    }

    Mpi::broadcast(0, data);

    CHECK_EQ(data, root_v);
}

/** 
 * Returns a container with three elements.
 * Elements are valued `value_root` at the root rank
 * Elements are valued `value_others` at all other ranks
 */
template<mpi::ValidContainer C>
auto SetupContainerBroadcast(Mpi::id_type root,
                             typename container_traits<C>::data value_root,
                             typename container_traits<C>::data value_others) -> C
{    
    if (Mpi::rank() == root) {
        return {value_root, value_root, value_root};
    }
    return {value_others, value_others, value_others};
}

TEST_CASE_TEMPLATE("VectorBroadcastFirst", T, int, unsigned, char, long long, float, double)
{
    const Mpi::id_type root = 0;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::vector<T>>(root, 1, 2);

    Mpi::broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}

TEST_CASE_TEMPLATE("VectorBroadcastLast", T, int, unsigned, char, long long, float, double)
{
    const Mpi::id_type root = Mpi::size() - 1;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::vector<T>>(root, 1, 2);

    Mpi::broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}

TEST_CASE_TEMPLATE("VectorBroadcastMiddle", T, int, unsigned, char, long long, float, double)
{
    if (Mpi::size() < 3 ) {
        return; // Skipping
    }

    const Mpi::id_type root = 2;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::vector<T>>(root, 1, 2);

    Mpi::broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}

TEST_CASE_TEMPLATE("ArrayBroadcastFirst", T, bool, int, unsigned, char, long long, float, double)
{
    const Mpi::id_type root = 0;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::array<T, 3>>(root, 1, 2);

    Mpi::broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}

TEST_CASE_TEMPLATE("ArrayBroadcastLast", T, int, unsigned, char, long long, float, double)
{
    const Mpi::id_type root = Mpi::size() - 1;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::array<T, 3>>(root, 1, 2);

    Mpi::broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}

TEST_CASE_TEMPLATE("ArrayBroadcastMiddle", T, int, unsigned, char, long long, float, double)
{
    if (Mpi::size() < 3 ) {
        return; // Skipping
    }

    const Mpi::id_type root = 2;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::array<T, 3>>(root, 1, 2);
    
    Mpi::broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}