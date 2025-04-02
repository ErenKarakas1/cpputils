# utils::math

Provides support for various mathematical operations on vectors and matrices.
It uses right-handed coordinate system and column-major order for matrices as
the library is intended for use with OpenGL.

\
Vector and Matrix types support pretty printing using `std::ostream`.

## Usage

### Constants
```c++
float EPSILON = 1e-5F;
float PI = 3.14159265358979323846F;
```

### Basic utilities
```c++
bool is_power_of_two(const std::size_t n);
bool approx_equal(const float a, const float b);
float to_radians(const float degrees);
float to_degrees(const float radians);
float lerp(const float a, const float b, const float t);
```

### Generalized vector and matrix operations

#### Definitions
```c++
template <std::size_t N>
struct Vector {
    std::array<float, N> data{};

    float& operator[](const std::size_t i);
    const float& operator[](const std::size_t i) const;
};

template <std::size_t N>
struct Matrix {
    std::array<float, N * N> data{};

    float& operator[](const std::size_t i);
    const float& operator[](const std::size_t i) const;
};
```

#### Vector operations
```c++
Vector<N> add(const Vector<N>& l, const Vector<N>& r);
Vector<N> sub(const Vector<N>& l, const Vector<N>& r);
Vector<N> multiply(const Vector<N>& v, const float scalar);
Vector<N> divide(const Vector<N>& v, const float scalar);
float dot(const Vector<N>& l, const Vector<N>& r);
float length(const Vector<N>& v);
Vector<N> normalize(const Vector<N>& v);
```

#### Matrix operations
```c++
Matrix<N> add(const Matrix<N>& l, const Matrix<N>& r);
Matrix<N> sub(const Matrix<N>& l, const Matrix<N>& r);
Matrix<N> multiply(const Matrix<N>& l, const Matrix<N>& r);
Matrix<N> multiply(const Matrix<N>& m, const float scalar);
Matrix<N> divide(const Matrix<N>& m, const float scalar);

Matrix<N> transpose(const Matrix<N>& m);
Matrix<N> inverse(const Matrix<N>& m);
```

### 3D transformations and projections

#### Definitions
```c++
using Mat4 = Matrix<4>;
using Vec2 = Vector<2>;
using Vec3 = Vector<3>;
using Vec4 = Vector<4>;
```

#### Transformations and projections
```c++
Vec3 cross(const Vec3& l, const Vec3& r);
Mat4 look_at(const Vec3& eye, const Vec3& center, const Vec3& up);
Mat4 perspective(const float fov, const float aspect, const float near, const float far);
Mat4 orthographic(const float left, const float right, const float bottom, const float top, const float near, const float far);

Mat4 translation(const Vec3& v);
Mat4 translate(const Mat4& m, const Vec3& v);
Mat4 scale(const Mat4& m, const Vec3& v);

Mat4 x_rotation(const float angle);
Mat4 y_rotation(const float angle);
Mat4 z_rotation(const float angle);

Mat4 x_rotate(const Mat4& m, const float angle);
Mat4 y_rotate(const Mat4& m, const float angle);
Mat4 z_rotate(const Mat4& m, const float angle);
```

