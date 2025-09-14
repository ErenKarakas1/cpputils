// -------------------------------------------------------------------------------------
//
// docs: https://github.com/ErenKarakas1/cpputils/blob/main/docs/log.md
// src: https://github.com/ErenKarakas1/cpputils
// license: MIT
//
// -------------------------------------------------------------------------------------

#ifndef UTILS_LOG_HPP
#define UTILS_LOG_HPP

#include "common.hpp"

#include <format>
#include <iostream>
#include <string_view>

// TODOs
// - Thread safety
// - Sink concept and custom sinks
// - Color customization?
// - Datetime?
// - Flush all messages and buffered messages
// - More performance is easily achievable probably
//   - e.g. currently buffer is useless when color is disabled

namespace utils::log {

enum class LogLevel : u8 { DEBUG = 0, INFO = 1, WARNING = 2, ERROR = 3, OFF = 4 };

constexpr std::strong_ordering operator<=>(const LogLevel lhs, const LogLevel rhs) {
    return static_cast<u8>(lhs) <=> static_cast<u8>(rhs);
}

namespace color {

// clang-format off
inline constexpr std::string_view black   = "\033[30m";
inline constexpr std::string_view red     = "\033[31m";
inline constexpr std::string_view green   = "\033[32m";
inline constexpr std::string_view yellow  = "\033[33m";
inline constexpr std::string_view blue    = "\033[34m";
inline constexpr std::string_view magenta = "\033[35m";
inline constexpr std::string_view cyan    = "\033[36m";
inline constexpr std::string_view white   = "\033[37m";

inline constexpr std::string_view reset   = "\033[0m";
// clang-format on

} // namespace color

namespace detail {

struct logger {
    static logger& instance() {
        static logger logger;
        return logger;
    }

    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;

    // Temporarily here, will be moved to sink
    void set_log_level(const LogLevel level) {
        m_level = level;
    }

    [[nodiscard]] LogLevel log_level() const {
        return m_level;
    }

    template <LogLevel L, typename... Args>
    constexpr void log(const std::format_string<Args...> fmt, Args&&... args) {
        if (L < m_level) return;
        write_log(L, std::format(fmt, FORWARD(args)...));
    }

    template <typename... Args>
    constexpr void debug(const std::format_string<Args...> fmt, std::string_view file, int line, Args&&... args) {
        if (LogLevel::DEBUG < m_level) return;
        write_log(LogLevel::DEBUG,
                  std::format("[{}:{}] {}", file, line, std::format(fmt, FORWARD(args)...)));
    }

private:
    logger() = default;
    ~logger() = default;

    static std::ostream& out() {
        return std::cerr;
    }

    static constexpr std::string_view to_string(const LogLevel level) {
        // clang-format off
        switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::OFF:     return "OFF";
        default:                break;
        }
        // clang-format on
        return "UNKNOWN";
    }

    void write_log(const LogLevel level, const std::string_view message) const {
        thread_local std::string buffer;
        buffer.clear();

#ifdef LOG_COLOR
        switch (level) {
        case LogLevel::DEBUG:
            buffer += debug_color;
            break;
        case LogLevel::INFO:
            buffer += info_color;
            break;
        case LogLevel::WARNING:
            buffer += warning_color;
            break;
        case LogLevel::ERROR:
            buffer += error_color;
            break;
        default:
            break;
        }
#endif

        buffer += std::format("[{}] {}", to_string(level), message);
#ifdef LOG_COLOR
        buffer += color::reset;
#endif

        out() << buffer << '\n';
    }

    LogLevel m_level{LogLevel::INFO};

    std::string_view debug_color   = color::white;
    std::string_view info_color    = color::cyan;
    std::string_view warning_color = color::yellow;
    std::string_view error_color   = color::red;
};

} // namespace detail

} // namespace utils::log

// these names are very common but works for me

#define DEBUG(fmt, ...) utils::log::detail::logger::instance().debug(fmt, __FILE__, __LINE__ __VA_OPT__(, ) __VA_ARGS__)

template <typename... Args>
constexpr void INFO(const std::format_string<Args...> fmt, Args&&... args) {
    utils::log::detail::logger::instance().log<utils::log::LogLevel::INFO>(fmt, FORWARD(args)...);
}

template <typename... Args>
constexpr void WARNING(const std::format_string<Args...> fmt, Args&&... args) {
    utils::log::detail::logger::instance().log<utils::log::LogLevel::WARNING>(fmt, FORWARD(args)...);
}

template <typename... Args>
constexpr void ERROR(const std::format_string<Args...> fmt, Args&&... args) {
    utils::log::detail::logger::instance().log<utils::log::LogLevel::ERROR>(fmt, FORWARD(args)...);
}

#endif // UTILS_LOG_HPP
