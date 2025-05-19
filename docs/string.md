# utils::string

Provides commonly used string utilities.

## Usage

### Utility functions

#### Trimming
```c++
enum class TrimMode : std::uint8_t { Left, Right, Both };

void trim_in_place(std::string& str, const TrimMode mode = TrimMode::Both);
std::string trim(T&& str, const TrimMode mode = TrimMode::Both);

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
enum class SplitBehavior : std::uint8_t {
    // Do not keep empty tokens, e.g. split("a,,b", ",") == {"a", "b"}
    Nothing,

    // Keep empty tokens, e.g. split("a,,b", ",") == {"a", "", "b"}
    KeepEmpty,
};

std::vector<std::string> split(const std::string_view str, const std::string_view delimiter,
                               const SplitBehavior behavior = SplitBehavior::Nothing);
```

#### Misc
```c++
// Locale independent versions of the regular char utilities
namespace ascii {

template <typename CharT>
    requires std::integral<CharT>

bool is_alpha(const CharT c) noexcept;
bool is_digit(const CharT c) noexcept;
bool is_alnum(const CharT c) noexcept;
bool is_space(const CharT c) noexcept;
bool is_hex_digit(const CharT c) noexcept;
bool is_lower(const CharT c) noexcept;
bool is_upper(const CharT c) noexcept;
CharT to_lower(const CharT c) noexcept;
CharT to_upper(const CharT c) noexcept;

} // namespace ascii

// Port of strnlen
std::size_t strnlen(const char* str, const std::size_t max = 1024);
```

### StringViewBuilder
```c++
class StringViewBuilder<...>;

// Example usage
constexpr std::string_view sv = "Hello, World!";
constexpr std::string_view sv2 = "Hello, World!";
constexpr auto cv = "Hello, World!";

int main() {
    static constexpr auto cv2 = "Hello, World!";
    constexpr auto sb = StringViewBuilder<sv, sv2, cv, cv2>();

    constexpr std::string_view view = sb.view();
    constexpr const char* c_str = sb.c_str();
    
    // view == "Hello, World!Hello, World!Hello, World!Hello, World!"
    // strcmp(c_str, "Hello, World!Hello, World!Hello, World!Hello, World!") == 0
}
```

Note that the `StringViewBuilder` is constexpr and can be used to concatenate
various string types at compile time. All strings must have static storage
duration. You can check out the tests for more examples.
