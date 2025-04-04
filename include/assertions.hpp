// -------------------------------------------------------------------------------------
//
// docs: https://github.com/ErenKarakas1/cpputils/blob/main/docs/assertions.md
// src: https://github.com/ErenKarakas1/cpputils
// license: MIT
//
// -------------------------------------------------------------------------------------

#ifndef UTILS_ASSERTIONS_HPP
#define UTILS_ASSERTIONS_HPP

#include <iostream>
#include <source_location>
#include <string_view>

#define UNUSED(x) (void)(x)

#ifndef NDEBUG
inline void TODO(const std::string_view message = "", const std::source_location loc = std::source_location::current()) {
    std::cerr << "TODO at [" << loc.file_name() << ":" << loc.line() << "]: " << message << '\n';
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
        std::cerr << "Assert failed at [" << loc.file_name() << ":" << loc.line() << "]: " << message << '\n';
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
[[noreturn]] inline void UNREACHABLE(const std::string_view message = "") {
    UNUSED(message);
#if defined(_MSC_VER) && !defined(__clang__)
    __assume(false);
#else
    __builtin_unreachable();
#endif
}
#else
inline void UNREACHABLE(const std::string_view message = "") {
    UNUSED(message);
}
#endif

#endif // UTILS_ASSERTIONS_HPP
