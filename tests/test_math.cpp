#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"

#include "math.hpp"

#include <cmath>
#include <numbers>

using namespace utils::math;

TEST_CASE("is_power_of_two") {
    CHECK(is_power_of_two(1));
    CHECK(is_power_of_two(2));
    CHECK(is_power_of_two(4));
    CHECK(!is_power_of_two(0));
    CHECK(!is_power_of_two(3));
    CHECK(is_power_of_two(128));
}

TEST_CASE("Angle conversions") {
    constexpr float rad = to_radians(180.0F);
    CHECK(approx_equal(rad, std::numbers::pi_v<float>));

    constexpr float deg = to_degrees(std::numbers::pi_v<float>);
    CHECK(approx_equal(deg, 180.0F));
}

TEST_CASE("lerp") {
    constexpr float r = lerp(0.0F, 10.0F, 0.5F);
    CHECK(approx_equal(r, 5.0F));
}

TEST_CASE("Vector operations") {
    constexpr Vec3 v1 = {1.0F, 2.0F, 3.0F};
    constexpr Vec3 v2 = {4.0F, 5.0F, 6.0F};

    CHECK(add(v1, v2) == Vec3{5.0F, 7.0F, 9.0F});
    CHECK(sub(v1, v2) == Vec3{-3.0F, -3.0F, -3.0F});

    constexpr float expectedDot = 1.0F * 4.0F + 2.0F * 5.0F + 3.0F * 6.0F;
    CHECK(approx_equal(dot(v1, v2), expectedDot));

    CHECK(approx_equal(length(v1), std::sqrt(14.0F)));

    const Vec3 nv = normalize(v1);
    CHECK(approx_equal(length(nv), 1.0F));

    constexpr Vec3 a = {1.0F, 0.0F, 0.0F};
    constexpr Vec3 b = {0.0F, 1.0F, 0.0F};
    CHECK(cross(a, b) == Vec3{0.0F, 0.0F, 1.0F});

    constexpr auto v_mul = multiply(v1, 2.0F);
    CHECK(v_mul == Vec3{2.0F, 4.0F, 6.0F});
    CHECK(divide(v_mul, 2.0F) == v1);
}

TEST_CASE("Matrix operations") {
    constexpr Mat4 I = identity<4>();
    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            if (i == j) {
                CHECK(approx_equal(I[i * 4 + j], 1.0F));
            } else {
                CHECK(approx_equal(I[i * 4 + j], 0.0F));
            }
        }
    }

    Mat4 A = identity<4>();
    Mat4 B = multiply(identity<4>(), 2.0F);
    Mat4 expectedAdd;
    for (std::size_t i = 0; i < 16; ++i) {
        expectedAdd[i] = A[i] + B[i];
    }
    CHECK(add(A, B) == expectedAdd);

    Mat4 expectedSub;
    for (std::size_t i = 0; i < 16; ++i) {
        expectedSub[i] = B[i] - A[i];
    }
    CHECK(sub(B, A) == expectedSub);

    // Check that A * I == A
    CHECK(multiply(I, B) == B);

    // Test transpose
    constexpr Mat4 X = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    constexpr Mat4 XT = transpose(X);
    constexpr Mat4 expectedXT = {1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16};
    CHECK(XT == expectedXT);

    // Test inverse with a diagonal matrix
    constexpr Mat4 D = {2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4, 0, 0, 0, 0, 5};
    constexpr Mat4 D_inv = inverse(D);
    constexpr Mat4 expectedInv = {0.5F, 0, 0, 0, 0, 1.0F / 3.0F, 0, 0, 0, 0, 0.25F, 0, 0, 0, 0, 0.2F};
    CHECK(D_inv == expectedInv);

    // Check that D * D_inv == identity
    CHECK(multiply(D, D_inv) == I);
}

TEST_CASE("Transformation functions") {
    constexpr Vec3 eye = {0.0F, 0.0F, 0.0F};
    constexpr Vec3 center = {0.0F, 0.0F, -1.0F};
    constexpr Vec3 up = {0.0F, 1.0F, 0.0F};
    const Mat4 lookAtMat = look_at(eye, center, up);

    CHECK(approx_equal(lookAtMat[0], 1.0F));
    CHECK(approx_equal(lookAtMat[5], 1.0F));
    CHECK(approx_equal(lookAtMat[10], 1.0F));
    CHECK(approx_equal(lookAtMat[12], 0.0F));

    // Test perspective projection
    constexpr float near = 1.0F;
    constexpr float far = 10.0F;
    const Mat4 persp = perspective(to_radians(90.0F), 1.0F, near, far);
    CHECK(approx_equal(persp[11], -1.0F));

    // Test orthographic projection
    constexpr Mat4 ortho = orthographic(-1.0F, 1.0F, -1.0F, 1.0F, near, far);
    CHECK(approx_equal(ortho[0], 1.0F));
    CHECK(approx_equal(ortho[5], 1.0F));

    // Test translation matrix
    constexpr Vec3 transVec = {3.0F, 4.0F, 5.0F};
    constexpr Mat4 transMat = translation(transVec);
    CHECK(approx_equal(transMat[12], 3.0F));
    CHECK(approx_equal(transMat[13], 4.0F));
    CHECK(approx_equal(transMat[14], 5.0F));

    // Test translate
    constexpr Mat4 m = identity<4>();
    constexpr Mat4 mTranslated = translate(m, transVec);
    CHECK(approx_equal(mTranslated[12], 3.0F));
    CHECK(approx_equal(mTranslated[13], 4.0F));
    CHECK(approx_equal(mTranslated[14], 5.0F));

    // Test scale
    constexpr Vec3 scaleVec = {2.0F, 3.0F, 4.0F};
    constexpr Mat4 mScaled = scale(identity<4>(), scaleVec);
    CHECK(approx_equal(mScaled[0], 2.0F));
    CHECK(approx_equal(mScaled[5], 3.0F));
    CHECK(approx_equal(mScaled[10], 4.0F));

    // check that zero rotation equals identity
    const Mat4 rx0 = x_rotation(0.0F);
    CHECK(rx0 == identity<4>());
    const Mat4 ry0 = y_rotation(0.0F);
    CHECK(ry0 == identity<4>());
    const Mat4 rz0 = z_rotation(0.0F);
    CHECK(rz0 == identity<4>());

    constexpr float angle90 = to_radians(90.0F);
    const Mat4 rx90 = x_rotation(angle90);
    CHECK(approx_equal(rx90[5], std::cos(angle90)));
    CHECK(approx_equal(rx90[6], -std::sin(angle90)));
    CHECK(approx_equal(rx90[9], std::sin(angle90)));
}
