#pragma once

#include "doctest/doctest.h"
#include "mpicxx/mpicxx.h"

TEST_CASE_TEMPLATE("BroadcastFirst", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = 0;
    constexpr auto root_v = static_cast<T>(1);
    constexpr auto other_v = static_cast<T>(2);

    T data = static_cast<T>(0);
    if (comm.rank() == root) {
        data = root_v;
    } else {
        data = other_v;
    }

    comm.broadcast(root, data);

    CHECK_EQ(data, root_v);
}

TEST_CASE_TEMPLATE("BroadcastLast", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = comm.size() - 1;
    constexpr auto root_v = static_cast<T>(1);
    constexpr auto other_v = static_cast<T>(2);

    T data = static_cast<T>(0);
    if (comm.rank() == root) {
        data = root_v;
    } else {
        data = other_v;
    }

    comm.broadcast(root, data);

    CHECK_EQ(data, root_v);
}

TEST_CASE_TEMPLATE("BroadcastMiddle", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    if (comm.size() < 3 ) {
        return; // Skipping
    }

    const mpi::id_type root = 2;
    constexpr auto root_v = static_cast<T>(1);
    constexpr auto other_v = static_cast<T>(2);

    T data = static_cast<T>(0);
    if (comm.rank() == root) {
        data = root_v;
    } else {
        data = other_v;
    }

    comm.broadcast(root, data);

    CHECK_EQ(data, root_v);
}

/** 
 * Returns a container with three elements.
 * Elements are valued `value_root` at the root rank
 * Elements are valued `value_others` at all other ranks
 */
template<mpi::ValidContainer C>
auto SetupContainerBroadcast(const mpi::communicator& comm,
                             mpi::id_type root,
                             typename mpi::container_traits<C>::data value_root,
                             typename mpi::container_traits<C>::data value_others) -> C
{
    if (comm.rank() == root) {
        return {value_root, value_root, value_root};
    }
    return {value_others, value_others, value_others};
}

TEST_CASE_TEMPLATE("VectorBroadcastFirst", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = 0;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::vector<T>>(comm, root, T{1}, T{2});

    comm.broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}

TEST_CASE_TEMPLATE("VectorBroadcastLast", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = comm.size() - 1;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::vector<T>>(comm, root, T{1}, T{2});

    comm.broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}

TEST_CASE_TEMPLATE("VectorBroadcastMiddle", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    if (comm.size() < 3 ) {
        return; // Skipping
    }

    const mpi::id_type root = 2;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::vector<T>>(comm, root, T{1}, T{2});

    comm.broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}

TEST_CASE_TEMPLATE("ArrayBroadcastFirst", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = 0;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::array<T, 3>>(comm, root, T{1}, T{2});

    comm.broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}

TEST_CASE_TEMPLATE("ArrayBroadcastLast", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = comm.size() - 1;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::array<T, 3>>(comm, root, T{1}, T{2});

    comm.broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}

TEST_CASE_TEMPLATE("ArrayBroadcastMiddle", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    if (comm.size() < 3 ) {
        return; // Skipping
    }

    const mpi::id_type root = 2;
    const T value_root{1};
    auto data = SetupContainerBroadcast<std::array<T, 3>>(comm, root, T{1}, T{2});
    
    comm.broadcast(root, data);

    REQUIRE_EQ(data.size(), 3u);
    CHECK_EQ(data[0], value_root);
    CHECK_EQ(data[1], value_root);
    CHECK_EQ(data[2], value_root);
}