#pragma once

#include <cstdint>
#include <array>

#include "settings.h"

using channel = std::uint8_t;
using pixel = std::array<channel, 3>;


constexpr channel color_depth() noexcept {
    return 255;
}

inline int to_int(channel ch) {
    return static_cast<int>(ch);
}

inline pixel colorize(settings const& config, std::size_t score) {
    channel intensity = color_depth() - static_cast<channel>(static_cast<double>(color_depth()) * static_cast<double>(score) / static_cast<double>(config.max_iter));
    return {intensity, intensity, intensity};
}