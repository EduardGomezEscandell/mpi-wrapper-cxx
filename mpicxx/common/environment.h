#pragma once

#include <string>
#include <stdexcept>

#include "defines.h"


namespace mpi {

// RAII class to initialize and finalize MPI
template<Os OS, bool MpiEnabled>
struct basic_environment {
    basic_environment() noexcept {
    }

    ~basic_environment() noexcept {
        finalize();
    }

    enum class stages {
        uninitialized, running, finished
    };

    static void initialize() {
        if(stage() == stages::uninitialized) {
            initialize_impl();
        }
        advance_stage(stages::running);
    }

    static void finalize() {
        if(stage() == stages::running) {    
            finalize_impl();
        }
        advance_stage(stages::finished);
    }

    static std::string stage_string(stages stage) {
        switch (stage) {
        case stages::uninitialized: return "uninitialized";
        case stages::running:       return "running";
        case stages::finished:      return "finished";
        }
        return "undetermined";
    }

#ifdef DNDEBUG
    static constexpr void assert_running() noexcept { }
#else
    static void assert_running() {
        if(stage() == stages::running) {
            return;
        }
        throw std::runtime_error("Failed assertion: Stage is " + stage_string(stage()) + " rather than " + stage_string(stages::running) + "\n");
    }
#endif
    
    [[nodiscard]]
    static stages stage() noexcept { return singleton().stage_; }

private:
    static void advance_stage(stages next_stage) {
        if(next_stage < stage()) {
            throw std::runtime_error("Attempted to regress MPI environment stage from " + stage_string(stage()) + " to " + stage_string(next_stage) + "\n");
        }
        singleton().stage_ = next_stage;
    }

    [[nodiscard]]
    static basic_environment& singleton() {
        static basic_environment singleton_;
        return singleton_;
    }

    stages stage_ = stages::uninitialized;

    // Implemented by template specializations
    static void initialize_impl();
    static void finalize_impl();
};

}