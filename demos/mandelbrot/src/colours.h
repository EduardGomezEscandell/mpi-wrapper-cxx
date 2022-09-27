#pragma once

#include <algorithm>
#include <cstdint>
#include <array>
#include <stdexcept>
#include <cassert>

#include "settings.h"

using channel = std::uint8_t;
using pixel = std::array<channel, 3>;

constexpr pixel pixel_from_hex(unsigned hex) {
    return pixel{
        static_cast<channel>(hex >> 16 & 0xff),
        static_cast<channel>(hex >> 8 & 0xff),
        static_cast<channel>(hex & 0xff)
    };
}

namespace colors {
    constexpr pixel WHITE = pixel_from_hex(0xffffff);
    constexpr pixel BLACK = pixel_from_hex(0);
    constexpr pixel RED   = pixel_from_hex(0xff0000);
    constexpr pixel GREEN = pixel_from_hex(0x00ff00);
    constexpr pixel BLUE  = pixel_from_hex(0x0000ff);
}



struct colormap {
    virtual pixel colorize(unsigned score) const = 0;
    virtual ~colormap() = default;

    constexpr virtual channel color_depth() const noexcept = 0;

    using create_type = std::unique_ptr<colormap> (*)(settings const& config);
};

struct grayscale : public colormap
{
    grayscale(settings const& config)
    {
        ratio = static_cast<double>(color_depth()) / static_cast<double>(config.max_iter);
    }

    static std::unique_ptr<colormap> create(settings const& config) {
        return std::make_unique<grayscale>(config);
    }

    constexpr channel color_depth() const noexcept override {
        return static_cast<channel>(255);
    }

    pixel colorize(unsigned score) const override {
        channel intensity = color_depth() - static_cast<channel>(ratio * static_cast<double>(score));
        return {intensity, intensity, intensity};
    }

    static constexpr std::string_view name = "grayscale";

protected:
    double ratio;
};

// template<std::size_t N>
// struct lookup_table {
//     lookup_table(std::array<pixel, N> const& t) :
//         table(t)
//     {
//         std::sort(table.begin(), table.end());
//     }

//     std::array<pixel, N> table;

//     pixel lookup(unsigned x) {
//         std::partition_point(table.begin(), table.end(), [x](pixel& reference) { return x < reference; });
//     }

// };


struct pastel : public colormap
{
    /*
var ct_pastel Colortable = Colortable{
	lower_bounds: []float64{0, 0.2, 0.4, 0.6, 0.8, 1},
	colors: []Color{
		ColorFromHex(0xA3CEB1),
		ColorFromHex(0xEBEBD3),
		ColorFromHex(0xE8D3B6),
		ColorFromHex(0xA3AEC0),
		ColorFromHex(0xE0BCC6)},
	max_color: ColorFromHex(0),
	len:       5,
}

func pastelEval(cmap *Colormap, value int) Color {
	if value == cmap.Upper {
		return ct_pastel.max_color
	}
	d := value % ct_pastel.len
	x := float64(d) / float64(ct_pastel.len)

	return ct_pastel.Get(x)
}
    */
    pastel(settings const& config) : maxiter(config.max_iter)
    {
    }

    static std::unique_ptr<colormap> create(settings const& config) {
        return std::make_unique<pastel>(config);
    }

    constexpr channel color_depth() const noexcept override {
        return static_cast<channel>(0xff);
    }

    pixel colorize(unsigned score) const override {
        if(score == maxiter) return colors::BLACK;
        return colortable[score % colortable.size()];
    }

    static constexpr std::string_view name = "pastel";

protected:
    unsigned maxiter;
    constexpr static std::array colortable {
            pixel_from_hex(0xA3CEB1),
            pixel_from_hex(0xEBEBD3),
            pixel_from_hex(0xE8D3B6),
            pixel_from_hex(0xA3AEC0),
            pixel_from_hex(0xE0BCC6),
    };
};

inline std::unique_ptr<colormap> colormap_factory(settings config) {
    using namespace std::literals;
    
    static constexpr std::array<std::pair<std::string_view, colormap::create_type>, 2> colormaps {{
        {grayscale::name, grayscale::create},
        {pastel::name,    pastel::create}
    }};

    const auto it = std::find_if(colormaps.cbegin(), colormaps.cend(), [&](auto const& cmap) { return config.colormap == cmap.first; });
    if (it != colormaps.cend()) {
        return it->second(config);
    }
    throw std::runtime_error("Unknown colormap: " + config.colormap + "\n");
}
