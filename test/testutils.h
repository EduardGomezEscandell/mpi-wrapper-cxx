#pragma once

#include <filesystem>
#include <memory>
#include <chrono>
#include <thread>

#include "mpicxx/mpi.h"

class tmpdir {
    bool valid_;
    std::filesystem::path path_;
public:

    tmpdir(std::string const& path) : valid_{true}, path_{path} {
        if(mpi::communicator::get_default().rank() != 0) {
            return;
        }
        if (std::filesystem::exists(path_)) {
            valid_ = false;
            throw std::runtime_error("Temp folder already exists. Aborting.");
        }
        std::filesystem::create_directories(path_);
    }

    auto const& path() const noexcept { return path_; }

    ~tmpdir() {
        if(mpi::communicator::get_default().rank() != 0) {
            return;
        }
        if (valid_) {
            std::filesystem::remove_all(path_);
        }
    }
};

template<typename Predicate>
void wait_until(Predicate condition, std::chrono::milliseconds sleepytime) {
    while(!condition()) {
        std::this_thread::sleep_for(sleepytime);
    }
}
