#pragma once

#include <filesystem>
#include <optional>
#include <vector>
#include <string>

using std::string;
using std::vector;

namespace Config{

    extern std::filesystem::path conf_dir;
    extern std::filesystem::path conf_file;

    void load(const std::filesystem::path& conf_file, vector<string>& load_warnings);
    [[nodiscard]] std::optional<std::filesystem::path> get_config_dir() noexcept;
    auto get_log_file() -> std::optional<std::filesystem::path>;   
}