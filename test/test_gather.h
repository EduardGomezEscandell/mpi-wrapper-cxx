#pragma once

#include "doctest/doctest.h"
#include "mpicxx/mpi.h"

TEST_CASE_TEMPLATE("VectorSingleGatherFirst", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = 0;
    constexpr T send_v{1};
    std::vector<T> recieved{};
 
    comm.gather(root, send_v, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size());
        for(auto& r: recieved) {
            CHECK_EQ(r, send_v);
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("VectorSingleGatherLast", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = comm.size() - 1;
    constexpr T send_v{1};
    std::vector<T> recieved{};
 
    comm.gather(root, send_v, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size());
        for(auto& r: recieved) {
            CHECK_EQ(r, send_v);
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("VectorSingleGatherMiddle", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    if (comm.size() < 3 ) {
        return; // Skipping
    }

    const mpi::id_type root = 2;
    constexpr T send_v{1};
    std::vector<T> recieved{};
 
    comm.gather(root, send_v, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size());
        for(auto& r: recieved) {
            CHECK_EQ(r, send_v);
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("VectorVectorGatherFirst", T, int, unsigned, char, long long, float, double)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = 0;
    const auto sent_value = static_cast<T>(comm.rank());
    const std::vector<T> send_v{sent_value, sent_value, sent_value};
    std::vector<T> recieved{};
 
    comm.gather(root, send_v, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size() * 3);
        // Checking vector looks like [0,0,0,1,1,1,2,2,2,3,3, ...]
        for(mpi::size_type i=0; i<comm.size(); ++i) {
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
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = comm.size() - 1;
    const auto sent_value = static_cast<T>(comm.rank());
    const std::vector<T> send_v{sent_value, sent_value, sent_value};
    std::vector<T> recieved{};
 
    comm.gather(root, send_v, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size() * 3);
        // Checking vector looks like [0,0,0,1,1,1,2,2,2,3,3, ...]
        for(mpi::size_type i=0; i<comm.size(); ++i) {
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
    auto comm = mpi::communicator::get_default();

    if (comm.size() < 3 ) {
        return; // Skipping
    }

    const mpi::id_type root = 2;
    const auto sent_value = static_cast<T>(comm.rank());
    const std::vector<T> send_v{sent_value, sent_value, sent_value};
    std::vector<T> recieved{};
 
    comm.gather(root, send_v, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size() * 3);
        // Checking vector looks like [0,0,0,1,1,1,2,2,2,3,3, ...]
        for(mpi::size_type i=0; i<comm.size(); ++i) {
            for(auto j: {0, 1, 2}) {
                const auto idx = static_cast<std::size_t>(3*i + j);
                CHECK_EQ(recieved[idx], static_cast<T>(i));
            }
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

template<typename T>
constexpr T get_nth_letter(unsigned short, T* = nullptr);
template<> constexpr char get_nth_letter(unsigned short idx, char*) { return 'a' + static_cast<char>(idx); };
template<> constexpr wchar_t get_nth_letter(unsigned short idx, wchar_t*) { return L'a' + static_cast<wchar_t>(idx); };

TEST_CASE_TEMPLATE("StringSingleGatherFirst", T, char, wchar_t)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = 0;
    constexpr T send_v = get_nth_letter<T>(15);
    std::basic_string<T> recieved{};
 
    comm.gather(root, send_v, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size());
        for(auto& r: recieved) {
            CHECK_EQ(r, send_v);
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("StringSingleGatherLast", T, char, wchar_t)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = comm.size() - 1;
    constexpr T send_v = get_nth_letter<T>(15);
    std::basic_string<T> recieved{};
 
    comm.gather(root, send_v, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size());
        for(auto& r: recieved) {
            CHECK_EQ(r, send_v);
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("StringSingleGatherMiddle", T, char, wchar_t)
{
    auto comm = mpi::communicator::get_default();

    if (comm.size() < 3 ) {
        return; // Skipping
    }

    const mpi::id_type root = 2;
    constexpr T send_v = get_nth_letter<T>(15);
    std::basic_string<T> recieved{};
 
    comm.gather(root, send_v, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size());
        for(auto& r: recieved) {
            CHECK_EQ(r, send_v);
        }
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("StringStringGatherFirst", T, char, wchar_t)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = 0;
    const auto sent_value = get_nth_letter<T>(static_cast<unsigned short>(comm.rank()));
    const std::basic_string<T> send_str{sent_value, sent_value, sent_value};
    std::basic_string<T> recieved{};
 
    comm.gather(root, send_str, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size() * 3);
        // Checking basic_string looks like "aaaabbbbccccdddd..."
        std::basic_string<T> expected;
        for(mpi::size_type i=0; i<comm.size(); ++i) {
            const T letter = get_nth_letter<T>(static_cast<unsigned short>(i));
            expected += std::basic_string<T>{letter, letter, letter};
        }
        CHECK_EQ(expected, recieved);
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("StringStringGatherLast", T, char, wchar_t)
{
    auto comm = mpi::communicator::get_default();

    const mpi::id_type root = comm.size() - 1;
    const auto sent_value = get_nth_letter<T>(static_cast<unsigned short>(comm.rank()));
    const std::basic_string<T> send_str{sent_value, sent_value, sent_value};
    std::basic_string<T> recieved{};
 
    comm.gather(root, send_str, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size() * 3);
        // Checking basic_string looks like "aaaabbbbccccdddd..."
        std::basic_string<T> expected;
        for(mpi::size_type i=0; i<comm.size(); ++i) {
            const T letter = get_nth_letter<T>(static_cast<unsigned short>(i));
            expected += std::basic_string<T>{letter, letter, letter};
        }
        CHECK_EQ(expected, recieved);
    } else {
        REQUIRE(recieved.empty());
    }
}

TEST_CASE_TEMPLATE("StringStringGatherMiddle", T, char, wchar_t)
{
    auto comm = mpi::communicator::get_default();

    if (comm.size() < 3 ) {
        return; // Skipping
    }

    const mpi::id_type root = 2;
    const auto sent_value = get_nth_letter<T>(static_cast<unsigned short>(comm.rank()));
    const std::basic_string<T> send_str{sent_value, sent_value, sent_value};
    std::basic_string<T> recieved{};
 
    comm.gather(root, send_str, recieved);

    if (comm.rank() == root) {
        REQUIRE_EQ(recieved.size(), comm.size() * 3);
        // Checking basic_string looks like "aaaabbbbccccdddd..."
        std::basic_string<T> expected;
        for(mpi::size_type i=0; i<comm.size(); ++i) {
            const T letter = get_nth_letter<T>(static_cast<unsigned short>(i));
            expected += std::basic_string<T>{letter, letter, letter};
        }
        CHECK_EQ(expected, recieved);
    } else {
        REQUIRE(recieved.empty());
    }
}