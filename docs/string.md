# utils::string

Provides commonly used string utilities.

## Usage

---

### Utility functions

#### Trimming
```c++
void trim_left_in_place(std::string& str);
std::string trim_left(const std::string& str);

void trim_right_in_place(std::string& str);
std::string trim_right(const std::string& str);

void trim_in_place(std::string& str);
std::string trim(T&& str);

// Trims and reduces multiple spaces to a single space
trim_and_reduce_in_place(std::string& str);
std::string trim_and_reduce(T&& str);
```

#### Replacing
```c++
void replace_all_in_place(std::string& str, const std::string_view from, const std::string_view to);
std::string replace_all(T&& str, const std::string_view from, const std::string_view to);
```

#### Splitting
```c++
// Note that the result does not include empty tokens, i.e. split("a,,b", ",") == {"a", "b"}
std::vector<std::string> split(const std::string_view str, const std::string_view delimiter);
```

#### Misc
```c++
bool is_space(const int c); // safer reimplementation of std::isspace
std::size_t strnlen(const char* str, const std::size_t max = 1024); // port of strnlen
```

### StrViewBuilder
```c++
struct StrViewBuilder<...>;

// Example usage
constexpr std::string_view sv = "Hello, World!";
constexpr std::string_view sv2 = "Hello, World!";
constexpr auto cv = "Hello, World!";

int main() {
    static constexpr auto cv2 = "Hello, World!";
    // StrViewBuilder<sv, sv2, cv, cv2>::view -> "Hello, World!Hello, World!Hello, World!Hello, World!"
}
```

Note that the `StrViewBuilder` is constexpr and can be used to concatenate
various string types at compile time. All strings must have static storage
duration. You can check out the tests for more examples.
