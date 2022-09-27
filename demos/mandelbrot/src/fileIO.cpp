#include <fstream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <charconv>
#include <stdexcept>
#include <string>

#include "settings.h"
#include "fileIO.h"

netbpm_writer::netbpm_writer(distributed_canvas& canvas, settings const& config) :
    canvas{canvas}, config{config}
{}

std::ofstream netbpm_writer::file_handle() {
    switch(config.encode) {
        case encoding::ascii:  return std::ofstream(config.output, std::ios::app);
        case encoding::binary: return std::ofstream(config.output, std::ios::app | std::ios::binary);
    }
    throw std::invalid_argument("Unexpected encoding type");
}

void netbpm_writer::write() 
{
    auto comm = canvas.communicator();

    // Setup
    if (comm.rank() == 0) {
        if(std::filesystem::exists(config.output)) {
            std::filesystem::remove(config.output);
        }
    }

    // Header
    ppm_header();

    std::this_thread::sleep_for(std::chrono::milliseconds{100}); // TODO: This is hella ugly, won't scale with larger images
    comm.barrier();

    // Body
    ppm_body();

}

void netbpm_writer::ppm_header()
{
    if (canvas.communicator().rank() != 0) return;
    auto os = file_handle();
    switch(config.encode) {
        case encoding::ascii: os << "P3 "; break;
        case encoding::binary: os << "P6 "; break;
    }
    os << canvas.global_width() << " " << canvas.global_height() << " " 
       << static_cast<int>(colormap_factory(config)->color_depth()) << " " << std::flush;
}

void netbpm_writer::ppm_body() {
    switch(config.encode) {
        case encoding::ascii:  return ppm_body_impl(colorize_ascii);
        case encoding::binary: return ppm_body_impl(colorize_binary);
    }
    throw std::invalid_argument("Unexpected encoding type");
}

std::string netbpm_writer::colorize_ascii(colormap const& cmap, unsigned score) {
    auto px = cmap.colorize(score);
    return std::to_string(static_cast<int>(px[0])) + " "
        + std::to_string(static_cast<int>(px[1])) + " "
        + std::to_string(static_cast<int>(px[2])) + " ";
}

std::string netbpm_writer::colorize_binary(colormap const& cmap, unsigned score) {
    auto px = cmap.colorize(score);
    return {*reinterpret_cast<char*>(&px[0]),
            *reinterpret_cast<char*>(&px[1]),
            *reinterpret_cast<char*>(&px[2])};
}

void netbpm_writer::ppm_body_impl(std::string(*colorizer)(colormap const&, unsigned))
{    
    std::unique_ptr<const colormap> cmap = colormap_factory(config);
    // Stringifying
    const std::string data = [this, colorizer, &cmap]() {
        std::stringstream data;
        auto view = canvas.flat_view();
        std::transform(view.begin(), view.end(), std::ostream_iterator<std::string>(data), [colorizer, &cmap](unsigned score){ return colorizer(*cmap, score); });
        return data.str();
    }();

    // Writing
    auto comm = canvas.communicator();
    for (auto rank: std::ranges::iota_view<int, int>{0, comm.size()})
    {    
        if(comm.rank() == rank) {
            file_handle() << data << std::endl;
            logline(config, true, "Rank ", comm.rank(), " is done writing");
            std::this_thread::sleep_for(std::chrono::milliseconds{100}); // TODO: This is hella ugly, won't scale with larger images
        }
        comm.barrier();
    }
}


ini_reader::ini_reader(std::filesystem::path path) : path{path}
{
}

settings ini_reader::read()
{
    std::ifstream is(path);
    settings config;
    std::string line;
    for(std::size_t ln = 1; std::getline(is, line); ++ln) {
        try 
        {
            auto kv = parse_line(line);
            if(!kv.has_value()) continue;

            auto parsefunc = field_parser.find(kv->key);
            if(parsefunc == field_parser.cend()) throw std::invalid_argument("Unknown key: " + std::string{kv->key});
            parsefunc->second(config, kv->value);
        } catch(...) {
            std::cerr << "Line " + std::to_string(ln) + ":\n";
            throw;
        }
    }

    config.adjust_span();
    return config;
}

struct kv_pair {
    std::string_view key;
    std::string_view value;
};

std::optional<ini_reader::kv_pair> ini_reader::parse_line(std::string_view line) {
    auto valid_key_charset = [](char ch) {
        return !std::isspace(ch) && ch != ':' && ch != '=';
    };
    auto valid_val_charset = [](char ch) {
        return !std::isspace(ch);
    };

    auto end = std::find_if(line.begin(), line.end(), [](char ch) { return ch == ';' || ch == '#'; });

    // Finding KEY boundaries
    const auto key_begin = std::find_if(line.begin(), end, valid_key_charset);
    if(key_begin == end) return {}; // Empty line or comment
    
    const auto key_end   = std::find_if_not(key_begin, end, valid_key_charset);
    if(key_end == end) throw std::invalid_argument("Unexpected end of line after key");
    
    // Finsing separator

    auto separator = std::find_if(key_end, end, [](char ch) {
        if (ch == ':' || ch == '=') return true;
        if (std::isspace(ch)) return false;
        throw std::invalid_argument("Unexpected string after key");
    });
    if(separator == end) throw std::invalid_argument("Unexpected end of line after key");

    // Finding VALUE boundaries
    const auto value_begin = std::find_if(separator+1, end, valid_val_charset);
    const auto value_end   = std::find_if(std::reverse_iterator(end), std::reverse_iterator(value_begin), valid_val_charset).base();  

    return kv_pair {
        std::string_view{key_begin, key_end},
        std::string_view{value_begin, value_end}
    };
}

template<typename T>
T parse_value(std::string_view s) {
    T x = 0;
    if(std::from_chars<T>(s.begin(), s.end(), x).ec == std::errc::invalid_argument) {
        throw std::invalid_argument("Failed to parse integral value: '" + std::string{s} + "'");
    }
    return x;
}

template<>
std::size_t parse_value(std::string_view s) {
    unsigned long x = 0;
    if(std::from_chars(&(*s.begin()), &(*s.end()), x).ec == std::errc::invalid_argument) {
        throw std::invalid_argument("Failed to parse integral value: '" + std::string{s} + "'");
    }
    return static_cast<std::size_t>(x);
}

template<>
double parse_value(std::string_view s) {
    auto str = std::string{s.begin(), s.end()};
    return std::stod(str, 0);
}

template<>
bool parse_value(std::string_view s) {
    if(s == "true") return true;
    if(s == "false") return false;
    throw std::invalid_argument("Failed to parse boolean value: '" + std::string{s} + "'");
}

template<>
std::complex<double> parse_value(std::string_view s) {
    std::stringstream v{std::string(s.begin(), s.end())};
    std::complex<double> z {};
    v >> z;
    return z;
}

template<>
encoding parse_value(std::string_view s) {
    if(s == "ascii") return encoding::ascii;
    if(s == "binary") return encoding::binary;
    throw std::invalid_argument("Failed to parse encoding value: '" + std::string{s} + "'");
}

template<>
std::string parse_value(std::string_view s) {
    if(s.starts_with('"') && s.ends_with('"') && !s.ends_with("\\\"")) {
        return std::string{s.begin()+1, s.end()-1};
    }
    return std::string{s};
}

const std::map<std::string_view, void(*)(settings&, std::string_view)> ini_reader::field_parser {
    {"center_real", [](settings& s, std::string_view v) { s.center.real(parse_value<double>(v)); }},
    {"center_imag", [](settings& s, std::string_view v) { s.center.imag(parse_value<double>(v)); }},
    {"span",        [](settings& s, std::string_view v) { s.span.real(parse_value<double>(v)); }},
    {"img_width",   [](settings& s, std::string_view v) { s.img_width  = parse_value<std::size_t>(v); }},
    {"img_height",  [](settings& s, std::string_view v) { s.img_height = parse_value<std::size_t>(v); }},
    {"max_iter",    [](settings& s, std::string_view v) { s.max_iter   = parse_value<unsigned>(v); }},
    {"debug",       [](settings& s, std::string_view v) { s.debug      = parse_value<bool>(v); }},
    {"output",      [](settings& s, std::string_view v) { s.output     = parse_value<std::string>(v); }},
    {"encoding",    [](settings& s, std::string_view v) { s.encode     = parse_value<encoding>(v); }},
    {"colormap",    [](settings& s, std::string_view v) { s.colormap   = parse_value<std::string>(v); }}
};