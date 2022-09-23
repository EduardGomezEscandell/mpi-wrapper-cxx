#pragma once

#include <filesystem>
#include <string_view>
#include <optional>

#include "settings.h"
#include "distributed_canvas.h"

struct netbpm_writer {
    netbpm_writer(distributed_canvas& canvas, settings const& config);
    netbpm_writer(netbpm_writer const&) = delete;
    netbpm_writer(netbpm_writer&&) = default;

    std::ofstream file_handle();

    void write();

private:
    distributed_canvas& canvas;
    settings const& config;

    void ppm_header();
    void ppm_body();

    void ppm_body_ascii();
    void ppm_body_binary();
};

struct ini_reader {
    ini_reader(std::filesystem::path filepath);
    settings read();
private:
    std::filesystem::path path;

    struct kv_pair {
        std::string_view key;
        std::string_view value;
    };

    std::optional<kv_pair> parse_line(std::string_view line);

    static const std::map<std::string_view, void(*)(settings&, std::string_view)> field_parser;
};