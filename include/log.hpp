#ifndef LOG_HPP
#define LOG_HPP

#include <cstdint>
#include <format>
#include <iostream>
#include <source_location>
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

enum class LogLevel : std::uint8_t { DEBUG = 0, INFO = 1, WARNING = 2, ERROR = 3, OFF = 4 };

// I think <=> is not fully/properly supported by compilers yet
constexpr bool operator<(const LogLevel l, const LogLevel r) {
    return static_cast<int>(l) < static_cast<int>(r);
}

constexpr bool operator<=(const LogLevel l, const LogLevel r) {
    return static_cast<int>(l) <= static_cast<int>(r);
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
        write_log(L, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    constexpr void debug(const std::format_string<Args...> fmt, std::string_view file, int line, Args&&... args) {
        if (LogLevel::DEBUG < m_level) return;
        write_log(LogLevel::DEBUG,
                  std::format("[{}:{}] {}", file, line, std::format(fmt, std::forward<Args>(args)...)));
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
    utils::log::detail::logger::instance().log<utils::log::LogLevel::INFO>(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void WARNING(const std::format_string<Args...> fmt, Args&&... args) {
    utils::log::detail::logger::instance().log<utils::log::LogLevel::WARNING>(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void ERROR(const std::format_string<Args...> fmt, Args&&... args) {
    utils::log::detail::logger::instance().log<utils::log::LogLevel::ERROR>(fmt, std::forward<Args>(args)...);
}

#define UNUSED(x) (void)(x)

#ifndef NDEBUG
inline void TODO(const std::string_view message = "", const std::source_location loc = std::source_location::current()) {
    std::cerr << std::format("TODO at [{}:{}]: {}", loc.file_name(), loc.line(), message) << '\n';
    std::abort();
}
#else
inline void TODO(const std::string_view message = "", const std::source_location loc = std::source_location::current()) {
    UNUSED(message);
    UNUSED(loc);
}
#endif

#ifndef NDEBUG
inline void ASSERT(const bool condition, const std::string_view message = "",
                   const std::source_location loc = std::source_location::current()) {
    if (!condition) {
        std::cerr << std::format("Assert failed at [{}:{}]: {}", loc.file_name(), loc.line(), message) << '\n';
        std::abort();
    }
}
#else
inline void ASSERT(const bool condition, const std::string_view message = "",
                   const std::source_location loc = std::source_location::current()) {
    UNUSED(condition);
    UNUSED(message);
    UNUSED(loc);
}
#endif

#ifndef NDEBUG
[[noreturn]] inline void UNREACHABLE(const std::string_view message = "",
                                     const std::source_location loc = std::source_location::current()) {
    std::cerr << std::format("Unreachable code reached at [{}:{}]: {}", loc.file_name(), loc.line(), message) << '\n';
#if defined(_MSC_VER) && !defined(__clang__)
    __assume(false);
#else
    __builtin_unreachable();
#endif
}
#else
inline void UNREACHABLE(const std::string_view message = "",
                        const std::source_location location = std::source_location::current()) {
    UNUSED(message);
    UNUSED(location);
}
#endif

#endif // LOG_HPP
