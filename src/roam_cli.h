#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string_view>
#include <variant>

namespace Cli{
    namespace stdfs = std::filesystem;
    struct Cli{
        std::optional<stdfs::path> config_file;
        // Enable debug mode with additional logs and metrics.
        bool debug{};
        // Only use ANSI supported graph symbols adn colors.
        std::optional<bool> force_tty;
        bool force_utf{};
        bool low_color{};
        std::optional<std::uint32_t> preset;
        std::optional<std::uint32_t> updates;
    };
    // a vairiant of Cli or int
    using OrRetCode = std::variant<Cli, std::int32_t>;
    // parse cli args
    [[nodiscard]] auto parse(std::span<const std::string_view> args)noexcept -> OrRetCode;
    // print a usage header    
    void usage() noexcept;
    // print a help message
    void help() noexcept;
    void help_hint() noexcept;
}// namespace Cli