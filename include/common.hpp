// -------------------------------------------------------------------------------------
//
// docs: https://github.com/ErenKarakas1/cpputils/blob/main/docs/common.md
// src: https://github.com/ErenKarakas1/cpputils
// license: MIT
//
// -------------------------------------------------------------------------------------

#ifndef UTILS_COMMON_HPP
#define UTILS_COMMON_HPP

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <source_location>

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;

#define UNUSED(x) (void)(x)

#ifndef NDEBUG
inline void TODO(const char* message = nullptr, const std::source_location loc = std::source_location::current()) {
    if (message == nullptr) {
        std::fprintf(stderr, "TODO at [%s:%d]\n", loc.file_name(), loc.line());
    } else {
        std::fprintf(stderr, "TODO at [%s:%d]: %s\n", loc.file_name(), loc.line(), message);
    }
    std::abort();
}
#else
inline void TODO(const char* message = nullptr, const std::source_location loc = std::source_location::current()) {
    UNUSED(message);
    UNUSED(loc);
}
#endif

#ifndef NDEBUG
inline void ASSERT(const bool condition, const char* message = nullptr,
                   const std::source_location loc = std::source_location::current()) {
    if (!condition) {
        if (message == nullptr) {
            std::fprintf(stderr, "Assert failed at [%s:%d]\n", loc.file_name(), loc.line());
        } else {
            std::fprintf(stderr, "Assert failed at [%s:%d]: %s\n", loc.file_name(), loc.line(), message);
        }
        std::abort();
    }
}
#else
inline void ASSERT(const bool condition, const char* message = nullptr,
                   const std::source_location loc = std::source_location::current()) {
    UNUSED(condition);
    UNUSED(message);
    UNUSED(loc);
}
#endif

#ifndef NDEBUG
[[noreturn]] inline void UNREACHABLE(const char* message = nullptr) {
    UNUSED(message);
#if defined(_MSC_VER) && !defined(__clang__)
    __assume(false);
#else
    __builtin_unreachable();
#endif
}
#else
inline void UNREACHABLE(const char* message = nullptr) {
    UNUSED(message);
}
#endif

#pragma GCC system_header // The only way I found to supress -Wuseless-cast
// https://www.foonathan.net/2020/09/move-forward/
#define MOVE(...) static_cast<std::remove_reference_t<decltype(__VA_ARGS__)>&&>(__VA_ARGS__)
#define FORWARD(...) static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)

#endif // UTILS_COMMON_HPP
