#pragma once

#include <ranges>
#include <span>

#include "mpicxx/mpicxx.h"

#include "colours.h"

class distributed_canvas
{
public:

    distributed_canvas(std::size_t width, std::size_t height, mpi::communicator comm = mpi::communicator::get_default());

    // Converts a local row index into its global index
    std::size_t global_row(std::size_t local_row) const;

    // Converts a local column index into its global index
    std::size_t global_col(std::size_t local_col) const noexcept;

    // Converts a global row index into its local index
    std::size_t local_row(std::size_t g_row) const ;

    // Converts a global column index into its local index
    std::size_t local_col(std::size_t g_col) const noexcept;

    // Returs the width of the whole canvas
    std::size_t global_width() const noexcept;

    // Returs the width of this rank's section of the canvas
    std::size_t local_width() const noexcept;

    // Returs the heigh of the whole canvas
    std::size_t global_height() const noexcept;

    // Returs the height of this rank's section of the canvas
    std::size_t local_height() const noexcept;

    // Range of pixel rows in this section
    auto rows() const -> std::ranges::iota_view<std::size_t, std::size_t>;

    // Range of pixel columns in this section
    auto cols() const -> std::ranges::iota_view<std::size_t, std::size_t>;

    // Pixel located at certain global coordinates.
    // Must be in this rank's section
    unsigned& get(std::size_t g_row, std::size_t g_col);
    unsigned get(std::size_t g_row, std::size_t g_col) const;

    // Getter for comm_
    mpi::communicator communicator() const;

    // Returns an iterable of all pixels
    std::span<unsigned> flat_view();
    std::span<const unsigned> flat_view() const;
   
private:
    std::size_t width_;         // Global width of the canvas
    std::size_t height_;        // Global height of the canvas
    mpi::communicator comm_;    // Communicator to coordinate threads
    std::vector<unsigned> data_;   // Pixel data in this rank
};

