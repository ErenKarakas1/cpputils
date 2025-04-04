// -------------------------------------------------------------------------------------
//
// docs: https://github.com/ErenKarakas1/cpputils/blob/main/docs/math.md
// src: https://github.com/ErenKarakas1/cpputils
// license: MIT
//
// -------------------------------------------------------------------------------------

#ifndef UTILS_MATH_HPP
#define UTILS_MATH_HPP

#ifndef UTILS_CONSTEXPR
#if defined(_MSC_VER) && !defined(__clang__)
#define UTILS_CONSTEXPR inline
#else
#define UTILS_CONSTEXPR constexpr
#endif
#endif // UTILS_CONSTEXPR

#include <array>
#include <cassert>
#include <cmath>
#include <ostream>

// OpenGL right-handed coordinate system

// Most implementations in this header are either inspired by or
// directly taken from https://webgl2fundamentals.org/webgl/resources/m4.js

namespace utils::math {

inline constexpr float EPSILON = 1e-5F;
inline constexpr float PI = 3.14159265358979323846F;

constexpr bool is_power_of_two(const std::size_t n) {
    return (n != 0) && (n & (n - 1)) == 0;
}

UTILS_CONSTEXPR bool approx_equal(const float a, const float b) {
    return std::abs(a - b) < EPSILON;
}

constexpr float to_radians(const float degrees) {
    return degrees * (PI / 180.0F);
}

constexpr float to_degrees(const float radians) {
    return radians * (180.0F / PI);
}

constexpr float lerp(const float a, const float b, const float t) {
    return a + t * (b - a);
}

// ===========================================================================================
// Generalized vector and matrix math
// ===========================================================================================

template <std::size_t N>
struct Vector {
    std::array<float, N> data{};

    constexpr float& operator[](const std::size_t i) {
        return data[i];
    }

    constexpr const float& operator[](const std::size_t i) const {
        return data[i];
    }
};

template <std::size_t N>
struct Matrix {
    std::array<float, N * N> data{};

    constexpr float& operator[](const std::size_t i) {
        return data[i];
    }

    constexpr const float& operator[](const std::size_t i) const {
        return data[i];
    }
};

template <std::size_t N>
constexpr bool operator==(const Vector<N>& l, const Vector<N>& r) {
    for (std::size_t i = 0; i < N; ++i) {
        if (!approx_equal(l[i], r[i])) {
            return false;
        }
    }
    return true;
}

template <std::size_t N>
constexpr bool operator==(const Matrix<N>& l, const Matrix<N>& r) {
    for (std::size_t i = 0; i < N * N; ++i) {
        if (!approx_equal(l[i], r[i])) {
            return false;
        }
    }
    return true;
}

template <std::size_t N>
constexpr bool operator!=(const Vector<N>& l, const Vector<N>& r) {
    return !(l == r);
}

template <std::size_t N>
constexpr bool operator!=(const Matrix<N>& l, const Matrix<N>& r) {
    return !(l == r);
}

template <std::size_t N>
constexpr Vector<N> add(const Vector<N>& l, const Vector<N>& r) {
    Vector<N> v{};
    for (std::size_t i = 0; i < N; ++i) {
        v[i] = l[i] + r[i];
    }
    return v;
}

template <std::size_t N>
constexpr Vector<N> sub(const Vector<N>& l, const Vector<N>& r) {
    Vector<N> v{};
    for (std::size_t i = 0; i < N; ++i) {
        v[i] = l[i] - r[i];
    }
    return v;
}

template <std::size_t N>
constexpr Vector<N> multiply(const Vector<N>& v, const float scalar) {
    Vector<N> result{};
    for (std::size_t i = 0; i < N; ++i) {
        result[i] = v[i] * scalar;
    }
    return result;
}

template <std::size_t N>
constexpr Vector<N> divide(const Vector<N>& v, const float scalar) {
    Vector<N> result{};
    for (std::size_t i = 0; i < N; ++i) {
        result[i] = v[i] / scalar;
    }
    return result;
}

template <std::size_t N>
constexpr float dot(const Vector<N>& l, const Vector<N>& r) {
    float result = 0.0F;
    for (std::size_t i = 0; i < N; ++i) {
        result += l[i] * r[i];
    }
    return result;
}

template <std::size_t N>
constexpr float length(const Vector<N>& v) {
    return std::sqrt(dot(v, v));
}

template <std::size_t N>
constexpr Vector<N> normalize(const Vector<N>& v) {
    if (const float len = length(v); len > EPSILON) {
        return divide(v, len);
    }
    return v;
}

template <std::size_t N>
constexpr Matrix<N> identity() {
    Matrix<N> m{};
    for (std::size_t i = 0; i < N; ++i) {
        m[i * N + i] = 1.0F;
    }
    return m;
}

template <std::size_t N>
constexpr Matrix<N> add(const Matrix<N>& l, const Matrix<N>& r) {
    Matrix<N> m{};
    for (std::size_t i = 0; i < N * N; ++i) {
        m[i] = l[i] + r[i];
    }
    return m;
}

template <std::size_t N>
constexpr Matrix<N> sub(const Matrix<N>& l, const Matrix<N>& r) {
    Matrix<N> m{};
    for (std::size_t i = 0; i < N * N; ++i) {
        m[i] = l[i] - r[i];
    }
    return m;
}

template <std::size_t N>
constexpr Matrix<N> multiply(const Matrix<N>& l, const Matrix<N>& r) {
    Matrix<N> m{};
    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < N; ++j) {
            for (std::size_t k = 0; k < N; ++k) {
                m[i * N + j] += l[i * N + k] * r[k * N + j];
            }
        }
    }
    return m;
}

template <std::size_t N>
constexpr Matrix<N> multiply(const Matrix<N>& m, const float scalar) {
    Matrix<N> result{};
    for (std::size_t i = 0; i < N * N; ++i) {
        result[i] = m[i] * scalar;
    }
    return result;
}

template <std::size_t N>
constexpr Matrix<N> divide(const Matrix<N>& m, const float scalar) {
    Matrix<N> result{};
    for (std::size_t i = 0; i < N * N; ++i) {
        result[i] = m[i] / scalar;
    }
    return result;
}

template <std::size_t N>
constexpr Matrix<N> transpose(const Matrix<N>& m) {
    Matrix<N> result{};
    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < N; ++j) {
            result[i * N + j] = m[j * N + i];
        }
    }
    return result;
}

template <std::size_t N>
UTILS_CONSTEXPR Matrix<N> inverse(const Matrix<N>& m) {
    Matrix<N> result = identity<N>();
    Matrix<N> temp = m;
    for (std::size_t i = 0; i < N; ++i) {
        float pivot = temp[i * N + i];
        if (std::abs(pivot) < EPSILON) [[unlikely]] {
            assert(false && "Matrix is singular");
            return identity<N>();
        }
        for (std::size_t j = 0; j < N; ++j) {
            temp[i * N + j] /= pivot;
            result[i * N + j] /= pivot;
        }
        for (std::size_t j = 0; j < N; ++j) {
            if (j == i) {
                continue;
            }
            float factor = temp[j * N + i];
            for (std::size_t k = 0; k < N; ++k) {
                temp[j * N + k] -= factor * temp[i * N + k];
                result[j * N + k] -= factor * result[i * N + k];
            }
        }
    }
    return result;
}

// ===========================================================================================
// 3D transformations and projections
// ===========================================================================================

using Mat4 = Matrix<4>;
using Vec2 = Vector<2>;
using Vec3 = Vector<3>;
using Vec4 = Vector<4>;

constexpr Vec3 cross(const Vec3& l, const Vec3& r) {
    return {
        l[1] * r[2] - l[2] * r[1],
        l[2] * r[0] - l[0] * r[2],
        l[0] * r[1] - l[1] * r[0]
    };
}

constexpr Mat4 look_at(const Vec3& eye, const Vec3& center, const Vec3& up) {
    const Vec3 f = normalize(sub(center, eye));
    const Vec3 s = normalize(cross(f, up));
    const Vec3 u = cross(s, f);

    Mat4 result = identity<4>();
    result[0] = s[0];
    result[1] = u[0];
    result[2] = -f[0];
    result[3] = 0.0F;
    result[4] = s[1];
    result[5] = u[1];
    result[6] = -f[1];
    result[7] = 0.0F;
    result[8] = s[2];
    result[9] = u[2];
    result[10] = -f[2];
    result[11] = 0.0F;
    result[12] = -dot(s, eye);
    result[13] = -dot(u, eye);
    result[14] = dot(f, eye);
    result[15] = 1.0F;
    return result;
}

// NOTE: naming variables near and far causes problems with MSVC

UTILS_CONSTEXPR Mat4 perspective(const float fov, const float aspect, const float near_clip, const float far_clip) {
    const float f = 1.0F / std::tan(fov / 2.0F);
    Mat4 result = identity<4>();
    result[0] = f / aspect;
    result[5] = f;
    result[10] = (far_clip + near_clip) / (near_clip - far_clip);
    result[11] = -1.0F;
    result[14] = (2.0F * far_clip * near_clip) / (near_clip - far_clip);
    result[15] = 0.0F;
    return result;
}

constexpr Mat4 orthographic(const float left, const float right, const float bottom, const float top,
                            const float near_clip, const float far_clip) {
    Mat4 result = identity<4>();
    result[0] = 2.0F / (right - left);
    result[5] = 2.0F / (top - bottom);
    result[10] = -2.0F / (far_clip - near_clip);
    result[12] = -(right + left) / (right - left);
    result[13] = -(top + bottom) / (top - bottom);
    result[14] = -(far_clip + near_clip) / (far_clip - near_clip);
    return result;
}

constexpr Mat4 translation(const Vec3& v) {
    Mat4 result = identity<4>();
    result[12] = v[0];
    result[13] = v[1];
    result[14] = v[2];
    return result;
}

constexpr Mat4 translate(const Mat4& m, const Vec3& v) {
    Mat4 result = m;
    const float x = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12];
    const float y = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13];
    const float z = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14];
    const float w = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15];
    result[12] = x;
    result[13] = y;
    result[14] = z;
    result[15] = w;
    return result;
}

constexpr Mat4 scale(const Mat4& m, const Vec3& v) {
    Mat4 result = m;
    result[0] *= v[0];
    result[1] *= v[0];
    result[2] *= v[0];
    result[3] *= v[0];
    result[4] *= v[1];
    result[5] *= v[1];
    result[6] *= v[1];
    result[7] *= v[1];
    result[8] *= v[2];
    result[9] *= v[2];
    result[10] *= v[2];
    result[11] *= v[2];
    return result;
}

UTILS_CONSTEXPR Mat4 x_rotation(const float angle) {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    Mat4 result = identity<4>();
    result[5] = c;
    result[6] = -s;
    result[9] = s;
    result[10] = c;
    return result;
}

UTILS_CONSTEXPR Mat4 y_rotation(const float angle) {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    Mat4 result = identity<4>();
    result[0] = c;
    result[2] = s;
    result[8] = -s;
    result[10] = c;
    return result;
}

UTILS_CONSTEXPR Mat4 z_rotation(const float angle) {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    Mat4 result = identity<4>();
    result[0] = c;
    result[1] = -s;
    result[4] = s;
    result[5] = c;
    return result;
}

UTILS_CONSTEXPR Mat4 x_rotate(const Mat4& m, const float angle) {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    Mat4 result = m;
    const float y1 = m[1];
    const float y5 = m[5];
    const float y9 = m[9];
    const float y13 = m[13];
    result[1] = y1 * c - m[2] * s;
    result[5] = y5 * c - m[6] * s;
    result[9] = y9 * c - m[10] * s;
    result[13] = y13 * c - m[14] * s;
    result[2] = y1 * s + m[2] * c;
    result[6] = y5 * s + m[6] * c;
    result[10] = y9 * s + m[10] * c;
    result[14] = y13 * s + m[14] * c;
    return result;
}

UTILS_CONSTEXPR Mat4 y_rotate(const Mat4& m, const float angle) {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    Mat4 result = m;
    const float x0 = m[0];
    const float x4 = m[4];
    const float x8 = m[8];
    const float x12 = m[12];
    result[0] = x0 * c + m[2] * s;
    result[4] = x4 * c + m[6] * s;
    result[8] = x8 * c + m[10] * s;
    result[12] = x12 * c + m[14] * s;
    result[2] = -x0 * s + m[2] * c;
    result[6] = -x4 * s + m[6] * c;
    result[10] = -x8 * s + m[10] * c;
    result[14] = -x12 * s + m[14] * c;
    return result;
}

UTILS_CONSTEXPR Mat4 z_rotate(const Mat4& m, const float angle) {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    Mat4 result = m;
    const float x0 = m[0];
    const float x4 = m[4];
    const float x8 = m[8];
    const float x12 = m[12];
    result[0] = x0 * c - m[1] * s;
    result[4] = x4 * c - m[5] * s;
    result[8] = x8 * c - m[9] * s;
    result[12] = x12 * c - m[13] * s;
    result[1] = x0 * s + m[1] * c;
    result[5] = x4 * s + m[5] * c;
    result[9] = x8 * s + m[9] * c;
    result[13] = x12 * s + m[13] * c;
    return result;
}

} // namespace utils::math

// Pretty-printing

template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const utils::math::Vector<N>& v) {
    os << '[';
    for (std::size_t i = 0; i < N; ++i) {
        os << v[i];
        if (i != N - 1) {
            os << ", ";
        }
    }
    os << ']' << '\n';
    return os;
}

template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const utils::math::Matrix<N>& m) {
    os << '[';
    for (std::size_t i = 0; i < N; ++i) {
        if (i == 0) {
            os << '[';
        } else {
            os << " [";
        }
        for (std::size_t j = 0; j < N; ++j) {
            os << m[i * N + j];
            if (j != N - 1) {
                os << ", ";
            }
        }
        os << ']';
        if (i != N - 1) {
            os << '\n';
        }
    }
    os << ']' << '\n';
    return os;
}

#endif // UTILS_MATH_HPP
