#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"

#include "color.hpp"

#include <array>
#include <cmath>

using namespace utils::color;

TEST_CASE("Equality and hex conversion") {
    constexpr Color col1{.r = 255, .g = 128, .b = 64, .a = 32};
    constexpr Color col2{.r = 255, .g = 128, .b = 64, .a = 32};
    constexpr Color col3{.r = 0,   .g = 0,   .b = 0,  .a = 255};

    // Equality
    CHECK(col1 == col2);
    CHECK(col1 != col3);

    // Convert to hex and back
    constexpr unsigned int hex = to_hex(col1);
    constexpr Color col_roundtrip = from_hex(hex);
    CHECK(col1 == col_roundtrip);
}

TEST_CASE("Normalization and to_color conversion") {
    constexpr float4 norm{.x = 0.5F, .y = 0.25F, .z = 0.75F, .w = 1.0F};
    constexpr Color c = to_color(norm);
    const auto [x, y, z, w] = normalize_color(c);

    // These lose more precision than the others
    CHECK(std::abs(norm.x - x) < 1e-2F);
    CHECK(std::abs(norm.y - y) < 1e-2F);
    CHECK(std::abs(norm.z - z) < 1e-3F);
    CHECK(std::abs(norm.w - w) < 1e-5F);
}

TEST_CASE("RGB to HSV conversion edge cases") {
    // Saturation = 0 and hue = 0
    constexpr HSV hsv_black = rgb_to_hsv(BLACK);

    CHECK(std::abs(hsv_black.v - 0.0F) < 1e-5F);
    CHECK(std::abs(hsv_black.s - 0.0F) < 1e-5F);

    // Hue can be arbitrary when chroma == 0, but we set it to 0
    CHECK(std::abs(hsv_black.h - 0.0F) < 1e-5F);

    // Saturation should be zero
    constexpr HSV hsv_white = rgb_to_hsv(WHITE);

    CHECK(std::abs(hsv_white.v - 1.0F) < 1e-5F);
    CHECK(std::abs(hsv_white.s - 0.0F) < 1e-5F);

    // Hue near 120
    constexpr HSV hsv_green = rgb_to_hsv(GREEN);
    CHECK(std::abs(hsv_green.h - 120.0F) < 1e-5F);
    CHECK(std::abs(hsv_green.s - 1.0F) < 1e-5F);
    CHECK(std::abs(hsv_green.v - 1.0F) < 1e-5F);

    // Hue near 240
    constexpr HSV hsv_blue = rgb_to_hsv(BLUE);
    CHECK(std::abs(hsv_blue.h - 240.0F) < 1e-5F);
    CHECK(std::abs(hsv_blue.s - 1.0F) < 1e-5F);
    CHECK(std::abs(hsv_blue.v - 1.0F) < 1e-5F);
}

TEST_CASE("RGB to HSV round-trip") {
    constexpr Color red{.r = 255, .g = 0, .b = 0, .a = 255};
    const HSV hsv_red = rgb_to_hsv(red);

    CHECK(std::abs(hsv_red.h - 0.0F) < 1e-5F);
    CHECK(std::abs(hsv_red.s - 1.0F) < 1e-5F);
    CHECK(std::abs(hsv_red.v - 1.0F) < 1e-5F);

    // Convert back from HSV to RGB
    const Color red_from_hsv = hsv_to_rgb(hsv_red);
    CHECK(red == red_from_hsv);
}

TEST_CASE("HSV to RGB conversion consistency") {
    // Start with a set of HSV values, convert them to RGB and back to HSV.
    constexpr std::array<HSV, 6> hsv_vals = {{
        {.h = 0.0F,   .s = 1.0F, .v = 1.0F}, // red
        {.h = 60.0F,  .s = 1.0F, .v = 1.0F}, // yellow
        {.h = 120.0F, .s = 1.0F, .v = 1.0F}, // green
        {.h = 180.0F, .s = 1.0F, .v = 1.0F}, // cyan
        {.h = 240.0F, .s = 1.0F, .v = 1.0F}, // blue
        {.h = 300.0F, .s = 1.0F, .v = 1.0F}  // magenta
    }};

    for (const auto& hsv : hsv_vals) {
        const Color c = hsv_to_rgb(hsv);
        auto [h, s, v] = rgb_to_hsv(c);

        float diff = std::abs(hsv.h - h);
        CHECK(diff < 1e-5F);

        CHECK(std::abs(hsv.s - s) < 1e-5F);
        CHECK(std::abs(hsv.v - v) < 1e-5F);
    }
}

TEST_CASE("Grayscale conversion") {
    constexpr Color sample{.r = 70, .g = 130, .b = 180, .a = 255};
    constexpr Color gray = rgb_to_grayscale(sample);
    constexpr float4 norm_gray = normalize_color(gray);

    CHECK(std::abs(norm_gray.x - norm_gray.y) < 1e-5F);
    CHECK(std::abs(norm_gray.y - norm_gray.z) < 1e-5F);
}
