#ifndef STRING_HPP
#define STRING_HPP

#include <array>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

namespace utils::string {

constexpr bool is_space(const int c) {
    return std::isspace(static_cast<unsigned char>(c)) != 0;
}

constexpr void trim_left_in_place(std::string& str) {
    std::size_t start = 0;
    while (start < str.size() && is_space(str[start])) ++start;
    str.erase(0, start);
}

constexpr std::string trim_left(const std::string& str) {
    std::size_t start = 0;
    while (start < str.size() && is_space(str[start])) ++start;
    return str.substr(start);
}

constexpr void trim_right_in_place(std::string& str) {
    std::size_t end = str.size();
    while (end > 0 && is_space(str[end - 1])) --end;
    str.erase(end);
}

constexpr std::string trim_right(const std::string& str) {
    std::size_t end = str.size();
    while (end > 0 && is_space(str[end - 1])) --end;
    return str.substr(0, end);
}

constexpr void trim_in_place(std::string& str) {
    trim_left_in_place(str);
    trim_right_in_place(str);
}

template <class T>
constexpr std::string trim(T&& str) {
    std::string result = std::forward<T>(str);
    trim_in_place(result);
    return result;
}

constexpr void trim_and_reduce_in_place(std::string& str) {
    std::size_t read = 0;
    std::size_t write = 0;
    bool in_ws_seq = false;

    while (read < str.size() && is_space(str[read])) ++read;

    while (read < str.size()) {
        if (is_space(str[read])) {
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

    while (write > 0 && is_space(str[write - 1])) --write;
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

// Does not include empty tokens, i.e. split("a,,b", ",") == {"a", "b"}
// Should it? (with a flag)
constexpr std::vector<std::string> split(const std::string_view str, const std::string_view delimiter) {
    if (delimiter.empty()) return { std::string(str) };

    std::vector<std::string> result;
    std::size_t curr = 0;
    std::size_t token_start = 0;

    while ((curr = str.find(delimiter, curr)) != std::string::npos) {
        if (curr != token_start) {
            result.emplace_back(str.substr(token_start, curr - token_start));
        }
        curr += delimiter.size();
        token_start = curr;
    }

    if (token_start != str.size()) {
        result.emplace_back(str.substr(token_start));
    }

    return result;
}

constexpr std::size_t strnlen(const char* str, const std::size_t max = 1024) {
    std::size_t len = 0;
    while (len < max && str[len] != '\0') ++len;
    return len;
}

namespace detail {

static constexpr std::string_view to_view(const char* str) {
    assert(str != nullptr);
    assert(strnlen(str) < 1024 && std::strlen(str) == strnlen(str));
    return { str, std::strlen(str) };
}

static constexpr std::string_view to_view(const std::string_view& str) {
    return str;
}

// A specialized version of std::copy to avoid including <algorithm>
constexpr char* copy(const char* begin, const char* end, char* result) {
    while (begin != end) {
        *result++ = *begin++;
    }
    return result;
}

} // namespace detail

// Adapted from https://github.com/v8/v8/blob/9e5d8118e2af44b94515db813f5a0aecd8149b7a/src/base/string-format.h
template <const auto&... strs>
struct StrViewBuilder {
    static constexpr auto concat() {
        constexpr auto views = std::array{ detail::to_view(strs)... };

        constexpr std::size_t size = [&views]() constexpr {
            std::size_t result = 1;
            for (const auto& view : views) {
                result += view.size();
            }
            return result;
        }();

        std::array<char, size> result{};
        char* ptr = result.data();

        for (const auto& view : views) {
            ptr = detail::copy(view.data(), view.end(), ptr);
        }

        *ptr = '\0';
        assert(ptr + 1 == result.data() + size);
        return result;
    }

    static constexpr auto array = concat();
    static constexpr std::string_view view{array.data(), array.size() - 1};
};

} // namespace utils::string

#endif // STRING_HPP
