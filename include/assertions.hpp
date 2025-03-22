#ifndef ASSERTIONS_HPP
#define ASSERTIONS_HPP

#include <format>
#include <iostream>
#include <source_location>
#include <string_view>
#include <string>

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

#endif // ASSERTIONS_HPP
