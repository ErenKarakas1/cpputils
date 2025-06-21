#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "color.hpp"

#include <array>
#include <cmath>

using namespace utils::color;

namespace {

bool approx_equal(const float a, const float b) {
    return std::abs(a - b) < 1e-5F;
}

} // namespace

TEST_CASE("equality and hex conversion") {
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

TEST_CASE("normalization and to_color conversion") {
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
    const HSV hsv_black = rgb_to_hsv(BLACK);
    // Hue can be arbitrary when chroma == 0, but we set it to 0
    CHECK(approx_equal(hsv_black.h, 0.0F));
    CHECK(approx_equal(hsv_black.s, 0.0F));
    CHECK(approx_equal(hsv_black.v, 0.0F));

    // Saturation should be zero
    const HSV hsv_white = rgb_to_hsv(WHITE);
    CHECK(approx_equal(hsv_white.s, 0.0F));
    CHECK(approx_equal(hsv_white.v, 1.0F));

    // Hue near 120
    const HSV hsv_green = rgb_to_hsv(GREEN);
    CHECK(approx_equal(hsv_green.h, 120.0F));
    CHECK(approx_equal(hsv_green.s, 1.0F));
    CHECK(approx_equal(hsv_green.v, 1.0F));

    // Hue near 240
    const HSV hsv_blue = rgb_to_hsv(BLUE);
    CHECK(approx_equal(hsv_blue.h, 240.0F));
    CHECK(approx_equal(hsv_blue.s, 1.0F));
    CHECK(approx_equal(hsv_blue.v, 1.0F));
}

TEST_CASE("RGB to HSV round-trip") {
    constexpr Color red{.r = 255, .g = 0, .b = 0, .a = 255};
    const HSV hsv_red = rgb_to_hsv(red);

    CHECK(approx_equal(hsv_red.h, 0.0F));
    CHECK(approx_equal(hsv_red.s, 1.0F));
    CHECK(approx_equal(hsv_red.v, 1.0F));

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

        CHECK(approx_equal(hsv.h, h));
        CHECK(approx_equal(hsv.s, s));
        CHECK(approx_equal(hsv.v, v));
    }
}

TEST_CASE("grayscale conversion") {
    constexpr Color sample{.r = 70, .g = 130, .b = 180, .a = 255};
    constexpr Color gray = rgb_to_grayscale(sample);
    constexpr float4 norm_gray = normalize_color(gray);

    CHECK(approx_equal(norm_gray.x, norm_gray.y));
    CHECK(approx_equal(norm_gray.y, norm_gray.z));
    CHECK(approx_equal(norm_gray.z, norm_gray.x));
}
