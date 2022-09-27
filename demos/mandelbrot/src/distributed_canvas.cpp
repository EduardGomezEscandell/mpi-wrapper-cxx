#include "distributed_canvas.h"

distributed_canvas::distributed_canvas(std::size_t width, std::size_t height, mpi::communicator comm)
    : width_{width}, height_{height}, comm_{comm}
{
    const auto total_size = local_height()*local_width();
    data_ = std::vector<unsigned>(total_size);
}

std::size_t distributed_canvas::global_row(std::size_t local_row) const {
    return local_height() * static_cast<std::size_t>(comm_.rank()) + local_row;
}

std::size_t distributed_canvas::global_col(std::size_t local_col) const noexcept {
    return local_col;
}

std::size_t distributed_canvas::local_row(std::size_t g_row) const {
    const std::size_t rowsbegin = global_row(0);
    assert(g_row >= rowsbegin);
    assert(g_row < global_row(local_height()));
    return g_row - rowsbegin;
}

std::size_t distributed_canvas::local_col(std::size_t g_col) const noexcept {
    const std::size_t colsbegin = global_col(0);
    assert(g_col >= colsbegin);
    assert(g_col < global_col(local_width()));
    return g_col - colsbegin;
}

std::size_t distributed_canvas::global_width() const noexcept {
    return width_;
}

std::size_t distributed_canvas::local_width() const noexcept {
    return width_;
}

std::size_t distributed_canvas::global_height() const noexcept {
    return height_;
}

std::size_t distributed_canvas::local_height() const noexcept {
    return height_ / static_cast<std::size_t>(comm_.size());
}

auto distributed_canvas::rows() const -> std::ranges::iota_view<std::size_t, std::size_t> {
    return {global_row(0), global_row(local_height())};
}

auto distributed_canvas::cols() const -> std::ranges::iota_view<std::size_t, std::size_t> {
    return {global_col(0), global_col(local_width())};
}

unsigned& distributed_canvas::get(std::size_t g_row, std::size_t g_col) {
    return data_[local_row(g_row)*local_width() + local_col(g_col)];
}

unsigned distributed_canvas::get(std::size_t g_row, std::size_t g_col) const {
    return data_[local_row(g_row)*local_width() + local_col(g_col)];
}

mpi::communicator distributed_canvas::communicator() const {
    return comm_;
}

std::span<unsigned> distributed_canvas::flat_view() {
    return {data_.begin(), data_.end()};
}

std::span<const unsigned> distributed_canvas::flat_view() const {
    return {data_.cbegin(), data_.cend()};
}
