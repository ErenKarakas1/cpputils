# utils::color

Provides utilities to work with colors.

## Usage

---

### Color formats
```c++
struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct HSV {
    float h;
    float s;
    float v;
};
```

### Constants
```c++
Color BLACK   = {0, 0, 0, 255};
Color WHITE   = {255, 255, 255, 255};
Color RED     = {255, 0, 0, 255};
Color GREEN   = {0, 255, 0, 255};
Color BLUE    = {0, 0, 255, 255};
Color YELLOW  = {255, 255, 0, 255};
Color MAGENTA = {255, 0, 255, 255};
Color CYAN    = {0, 255, 255, 255};
```

### Utility functions
```c++
unsigned int to_hex(const Color& color); // returns hexcode of color in binary
Color from_hex(const unsigned int hex);

float4 normalize_color(const Color& color); // returns color in range [0, 1]
Color to_color(const float4& vec4); // returns color in range [0, 255]

HSV rgb_to_hsv(const Color& rgba);
Color hsv_to_rgb(const HSV& hsv);

Color rgb_to_grayscale(const Color& color); // returns grayscale color
```
