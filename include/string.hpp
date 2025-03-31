#ifndef STRING_HPP
#define STRING_HPP

#include <array>
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

namespace utils::string {

enum class TrimMode : std::uint8_t { Left, Right, Both };

enum class SplitBehavior : std::uint8_t {
    // Do not keep empty tokens, e.g. split("a,,b", ",") == {"a", "b"}
    Nothing,

    // Keep empty tokens, e.g. split("a,,b", ",") == {"a", "", "b"}
    KeepEmpty,
};

namespace ascii {

constexpr bool is_alpha(const unsigned char c) noexcept {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

constexpr bool is_alpha(const char c) noexcept {
    return is_alpha(static_cast<unsigned char>(c));
}

constexpr bool is_digit(const unsigned char c) noexcept {
    return c >= '0' && c <= '9';
}

constexpr bool is_digit(const char c) noexcept {
    return is_digit(static_cast<unsigned char>(c));
}

constexpr bool is_alnum(const unsigned char c) noexcept {
    return is_alpha(c) || is_digit(c);
}

constexpr bool is_alnum(const char c) noexcept {
    return is_alnum(static_cast<unsigned char>(c));
}

constexpr bool is_space(const unsigned char c) noexcept {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

constexpr bool is_space(const char c) noexcept {
    return is_space(static_cast<unsigned char>(c));
}

} // namespace ascii

constexpr std::size_t strnlen(const char* str, const std::size_t max = 1024) {
    std::size_t len = 0;
    while (len < max && str[len] != '\0') ++len;
    return len;
}

namespace detail {

constexpr std::string_view to_view(const char* str) {
    assert(str != nullptr);
    assert(strnlen(str) < 1024);
    return { str, strnlen(str) };
}

constexpr std::string_view to_view(const std::string_view& str) {
    return str;
}

// A specialized version of std::copy to avoid including <algorithm>
constexpr char* copy(const char* begin, const char* end, char* result) {
    while (begin != end) {
        *result++ = *begin++;
    }
    return result;
}

// clang-format off
constexpr void trim_left_in_place(std::string& str) {
    std::size_t start = 0;
    while (start < str.size() && ascii::is_space(str[start])) ++start;
    str.erase(0, start);
}

constexpr void trim_right_in_place(std::string& str) {
    std::size_t end = str.size();
    while (end > 0 && ascii::is_space(str[end - 1])) --end;
    str.erase(end);
}
// clang-format on

} // namespace detail

constexpr void trim_in_place(std::string& str, const TrimMode mode = TrimMode::Both) {
    if (mode == TrimMode::Left || mode == TrimMode::Both) {
        detail::trim_left_in_place(str);
    }
    if (mode == TrimMode::Right || mode == TrimMode::Both) {
        detail::trim_right_in_place(str);
    }
}

template <class T>
constexpr std::string trim(T&& str, const TrimMode mode = TrimMode::Both) {
    std::string result = std::forward<T>(str);
    trim_in_place(result, mode);
    return result;
}

constexpr void trim_and_reduce_in_place(std::string& str) {
    std::size_t read = 0;
    std::size_t write = 0;
    bool in_ws_seq = false;

    // clang-format off
    while (read < str.size() && ascii::is_space(str[read])) ++read;
    // clang-format on

    while (read < str.size()) {
        if (ascii::is_space(str[read])) {
            if (!in_ws_seq) {
                str[write++] = ' ';
                in_ws_seq = true;
            }
        } else {
            str[write++] = str[read];
            in_ws_seq = false;
        }
        ++read;
    }

    // clang-format off
    while (write > 0 && ascii::is_space(str[write - 1])) --write;
    // clang-format on
    str.resize(write);
}

template <class T>
constexpr std::string trim_and_reduce(T&& str) {
    std::string result = std::forward<T>(str);
    trim_and_reduce_in_place(result);
    return result;
}

constexpr void replace_all_in_place(std::string& str, const std::string_view from, const std::string_view to) {
    std::size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.size(), to);
        pos += to.size();
    }
}

template <class T>
constexpr std::string replace_all(T&& str, const std::string_view from, const std::string_view to) {
    std::string result = std::forward<T>(str);
    replace_all_in_place(result, from, to);
    return result;
}

constexpr std::vector<std::string> split(const std::string_view str, const std::string_view delimiter,
                                         const SplitBehavior behavior = SplitBehavior::Nothing) {
    if (delimiter.empty()) return { std::string(str) };

    std::vector<std::string> result;
    std::size_t curr = 0;
    std::size_t token_start = 0;

    while ((curr = str.find(delimiter, curr)) != std::string::npos) {
        if (behavior == SplitBehavior::KeepEmpty || curr != token_start) {
            result.emplace_back(str.substr(token_start, curr - token_start));
        }
        curr += delimiter.size();
        token_start = curr;
    }

    if (behavior == SplitBehavior::KeepEmpty || token_start != str.size()) {
        result.emplace_back(str.substr(token_start));
    }

    return result;
}

// Adapted from https://github.com/v8/v8/blob/9e5d8118e2af44b94515db813f5a0aecd8149b7a/src/base/string-format.h
template <const auto&... strs>
class StringViewBuilder {
public:
    constexpr StringViewBuilder() : m_view(array.data(), array.size() - 1), m_c_str(array.data()) {}

    constexpr std::string_view view() const {
        return m_view;
    }

    constexpr const char* c_str() const {
        return m_c_str;
    }

private:
    static constexpr auto concat() {
        constexpr std::array views = { detail::to_view(strs)... };

        constexpr std::size_t size = [&views]() constexpr {
            std::size_t result = 1;
            for (const auto& str_view : views) {
                result += str_view.size();
            }
            return result;
        }();

        std::array<char, size> result{};
        char* ptr = result.data();

        for (const auto& str_view : views) {
            ptr = detail::copy(str_view.data(), str_view.end(), ptr);
        }

        *ptr = '\0';
        assert(ptr + 1 == result.data() + size);
        return result;
    }

    static constexpr auto array = concat();
    std::string_view m_view;
    const char* m_c_str;
};

} // namespace utils::string

#endif // STRING_HPP
