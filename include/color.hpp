// -------------------------------------------------------------------------------------
//
// docs: https://github.com/ErenKarakas1/cpputils/blob/main/docs/color.md
// src: https://github.com/ErenKarakas1/cpputils
// license: MIT
//
// -------------------------------------------------------------------------------------

#ifndef UTILS_COLOR_HPP
#define UTILS_COLOR_HPP

#ifndef UTILS_CONSTEXPR
#if defined(_MSC_VER) && !defined(__clang__)
#define UTILS_CONSTEXPR inline
#else
#define UTILS_CONSTEXPR constexpr
#endif
#endif // UTILS_CONSTEXPR

#include <cmath>
#include <cstdint>

namespace utils::color {

namespace detail {

inline constexpr float EPSILON = 1e-6F;

constexpr float min(const float a, const float b) {
    return a < b ? a : b;
}

constexpr float max(const float a, const float b) {
    return a > b ? a : b;
}

} // namespace detail

struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
};

struct HSV {
    float h;
    float s;
    float v;
};

struct float4 {
    float x;
    float y;
    float z;
    float w;
};

// clang-format off
inline constexpr Color BLACK   = {0, 0, 0, 255};
inline constexpr Color WHITE   = {255, 255, 255, 255};
inline constexpr Color RED     = {255, 0, 0, 255};
inline constexpr Color GREEN   = {0, 255, 0, 255};
inline constexpr Color BLUE    = {0, 0, 255, 255};
inline constexpr Color YELLOW  = {255, 255, 0, 255};
inline constexpr Color MAGENTA = {255, 0, 255, 255};
inline constexpr Color CYAN    = {0, 255, 255, 255};
// clang-format on

constexpr bool operator==(const Color& l, const Color& r) {
    return l.r == r.r && l.g == r.g && l.b == r.b && l.a == r.a;
}

UTILS_CONSTEXPR bool operator==(const HSV& l, const HSV& r) {
    return std::abs(l.h - r.h) < detail::EPSILON && std::abs(l.s - r.s) < detail::EPSILON &&
           std::abs(l.v - r.v) < detail::EPSILON;
}

UTILS_CONSTEXPR bool operator==(const float4& l, const float4& r) {
    return std::abs(l.x - r.x) < detail::EPSILON && std::abs(l.y - r.y) < detail::EPSILON &&
           std::abs(l.z - r.z) < detail::EPSILON && std::abs(l.w - r.w) < detail::EPSILON;
}

// I think compilers implicitly generate these from operator== now
constexpr bool operator!=(const Color& l, const Color& r) {
    return !(l == r);
}

UTILS_CONSTEXPR bool operator!=(const HSV& l, const HSV& r) {
    return !(l == r);
}

UTILS_CONSTEXPR bool operator!=(const float4& l, const float4& r) {
    return !(l == r);
}

constexpr unsigned int to_hex(const Color& color) {
    return (static_cast<unsigned int>(color.r) << 24) | (static_cast<unsigned int>(color.g) << 16) |
           (static_cast<unsigned int>(color.b) << 8)  |  static_cast<unsigned int>(color.a);
}

constexpr Color from_hex(const unsigned int hex) {
    return {
        .r = static_cast<std::uint8_t>((hex >> 24) & 0xFF),
        .g = static_cast<std::uint8_t>((hex >> 16) & 0xFF),
        .b = static_cast<std::uint8_t>((hex >> 8)  & 0xFF),
        .a = static_cast<std::uint8_t>( hex & 0xFF),
    };
}

constexpr float4 normalize_color(const Color& color) {
    return {
        .x = static_cast<float>(color.r) / 255.0F,
        .y = static_cast<float>(color.g) / 255.0F,
        .z = static_cast<float>(color.b) / 255.0F,
        .w = static_cast<float>(color.a) / 255.0F,
    };
}

// TODO: naming
constexpr Color to_color(const float4& vec4) {
    return {
        .r = static_cast<std::uint8_t>(vec4.x * 255.0F),
        .g = static_cast<std::uint8_t>(vec4.y * 255.0F),
        .b = static_cast<std::uint8_t>(vec4.z * 255.0F),
        .a = static_cast<std::uint8_t>(vec4.w * 255.0F),
    };
}

UTILS_CONSTEXPR HSV rgb_to_hsv(const Color& rgba) {
    HSV hsv{};
    const auto [r, g, b, _] = normalize_color(rgba);

    const float max = detail::max(r, detail::max(g, b));
    const float min = detail::min(r, detail::min(g, b));
    const float chroma = max - min;

    hsv.v = max;

    if (std::abs(chroma) < detail::EPSILON) {
        hsv.h = 0.0F;
    } else if (std::abs(max - r) < detail::EPSILON) {
        hsv.h = 60.0F * std::fmodf((g - b) / chroma, 6.0F);
    } else if (std::abs(max - g) < detail::EPSILON) {
        hsv.h = 60.0F * ((b - r) / chroma + 2.0F);
    } else if (std::abs(max - b) < detail::EPSILON) {
        hsv.h = 60.0F * ((r - g) / chroma + 4.0F);
    }

    if (hsv.h < 0.0F) {
        hsv.h += 360.0F;
    }

    if (std::abs(max) < detail::EPSILON) {
        hsv.s = 0.0F;
    } else {
        hsv.s = chroma / max;
    }

    return hsv;
}

UTILS_CONSTEXPR Color hsv_to_rgb(const HSV& hsv) {
    // f(n) = v - v * s * max(0, min(k, 4 - k, 1))
    // where k = (n + h / 60) % 6
    // and rgb = f(5), f(3), f(1)

    const float kr = std::fmodf(5.0F + hsv.h / 60.0F, 6.0F);
    const float fr = hsv.v - hsv.v * hsv.s * detail::max(0.0F, detail::min(kr, detail::min(4.0F - kr, 1.0F)));

    const float kg = std::fmodf(3.0F + hsv.h / 60.0F, 6.0F);
    const float fg = hsv.v - hsv.v * hsv.s * detail::max(0.0F, detail::min(kg, detail::min(4.0F - kg, 1.0F)));

    const float kb = std::fmodf(1.0F + hsv.h / 60.0F, 6.0F);
    const float fb = hsv.v - hsv.v * hsv.s * detail::max(0.0F, detail::min(kb, detail::min(4.0F - kb, 1.0F)));

    return to_color({.x = fr, .y = fg, .z = fb, .w = 1.0F});
}

constexpr Color rgb_to_grayscale(const Color& color) {
    const auto [r, g, b, a] = normalize_color(color);
    const float gray = 0.299F * r + 0.587F * g + 0.114F * b;

    return to_color({.x = gray, .y = gray, .z = gray, .w = a});
}

} // namespace utils::color

#endif // UTILS_COLOR_HPP
