#pragma once

#include <cstdint>
#include <array>

#include "settings.h"

using channel = std::uint8_t;
using pixel = std::array<channel, 3>;

inline pixel colorize(settings const& config, std::size_t score) {
    auto intensity = static_cast<channel>(255.0 * static_cast<double>(score) / static_cast<double>(config.max_iter));
    return {intensity, intensity, intensity};
}
