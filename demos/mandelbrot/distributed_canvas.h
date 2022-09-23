#pragma once

#include <chrono>
#include <fstream>
#include <ranges>
#include <span>
#include <unistd.h>

#include "mpicxx/mpi.h"

#include "colours.h"
#include "test/testutils.h"

class distributed_canvas
{
public:

    distributed_canvas(std::size_t width, std::size_t height, mpi::communicator comm = mpi::communicator::get_default())
        : width_{width}, height_{height}, comm_{comm}
    {
        const auto total_size = local_height()*local_width();
        data_ = std::vector<pixel>(total_size);
    }

    // Converts a local row index into its global index
    std::size_t global_row(std::size_t local_row) const {
        return local_height() * static_cast<std::size_t>(comm_.rank()) + local_row;
    }

    // Converts a local column index into its global index
    std::size_t global_col(std::size_t local_col) const noexcept {
        return local_col;
    }

    // Converts a global row index into its local index
    std::size_t local_row(std::size_t g_row) const {
        const std::size_t rowsbegin = global_row(0);
        assert(g_row >= rowsbegin);
        assert(g_row < global_row(local_height()));
        return g_row - rowsbegin;
    }

    // Converts a global column index into its local index
    std::size_t local_col(std::size_t g_col) const noexcept {
        const std::size_t colsbegin = global_col(0);
        assert(g_col >= colsbegin);
        assert(g_col < global_col(local_width()));
        return g_col - colsbegin;
    }

    // Returs the width of the whole canvas
    std::size_t global_width() const noexcept {
        return width_;
    }

    // Returs the width of this rank's section of the canvas
    std::size_t local_width() const noexcept {
        return width_;
    }

    // Returs the heigh of the whole canvas
    std::size_t global_height() const noexcept {
        return height_;
    }

    // Returs the height of this rank's section of the canvas
    std::size_t local_height() const noexcept {
        return height_ / static_cast<std::size_t>(comm_.size());
    }

    // Range of pixel rows in this section
    auto rows() const -> std::ranges::iota_view<std::size_t, std::size_t> {
        return {global_row(0), global_row(local_height())};
    }

    // Range of pixel columns in this section
    auto cols() const -> std::ranges::iota_view<std::size_t, std::size_t> {
        return {global_col(0), global_col(local_width())};
    }

    pixel& get(std::size_t g_row, std::size_t g_col) {
        return data_[local_row(g_row)*local_width() + local_col(g_col)];
    }

    pixel const& get(std::size_t g_row, std::size_t g_col) const {
        return data_[local_row(g_row)*local_width() + local_col(g_col)];
    }

    mpi::communicator communicator() const {
        return comm_;
    }

    std::span<pixel> flat_view() {
        return {data_.begin(), data_.end()};
    }

    std::span<const pixel> flat_view() const {
        return {data_.cbegin(), data_.cend()};
    }
   
private:
    std::size_t width_;
    std::size_t height_;
    mpi::communicator comm_;
    std::vector<pixel> data_;
};

void write_ppm(distributed_canvas const& canvas, settings const& config) 
{
    auto comm = canvas.communicator();

    // Setup
    if (comm.rank() == 0) {
        if(std::filesystem::exists(config.output)) {
            std::filesystem::remove(config.output);
        }
    }

    // Header
    if (comm.rank() == 0) {
        std::ofstream output(config.output, std::ios::app);
        output << "P3\n";
        output << canvas.global_width() << " " << canvas.global_height() << " " << to_int(color_depth()) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds{100}); // TODO: This is hella ugly, won't scale with larger images
    }
    comm.barrier();

    // Stringifying
    const std::string data = [comm, &canvas]() {
        std::stringstream data;
        for(auto const& px: canvas.flat_view()) {
            data << to_int(px[0]) << ' ' << to_int(px[1]) << ' ' << to_int(px[2]) << ' ';
        }
        return data.str();
    }();

    // Writing// Stringifying
    for (auto rank: std::ranges::iota_view<int, int>{0, comm.size()})
    {    
        if(comm.rank() == rank) {
            std::ofstream output(config.output, std::ios::app);
            output << data << std::endl;
            logline(config, true, "Rank ", comm.rank(), " is done writing");
            std::this_thread::sleep_for(std::chrono::milliseconds{100}); // TODO: This is hella ugly, won't scale with larger images
        }
        comm.barrier();
    }
}