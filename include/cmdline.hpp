#ifndef CMDLINE_HPP
#define CMDLINE_HPP

#include <cassert>
#include <format>
#include <iostream>
#include <string_view>
#include <variant>
#include <vector>

// TODOs
// - Subcommands

namespace utils::cmd {

namespace detail {

using DefaultT = std::variant<bool, int, unsigned int, std::int64_t, std::uint64_t, float, double, long double, char,
                              std::string_view, std::monostate>;

constexpr std::string to_formatted(const DefaultT& var) {
    return std::visit(
        []<typename var>(var&& value) -> std::string {
            using T = std::remove_const_t<std::remove_reference_t<var>>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                return "";
            } else if constexpr (std::is_same_v<T, char>) {
                return std::format("'{}'", value);
            } else if constexpr (std::is_same_v<T, std::string_view>) {
                return std::format("\"{}\"", value);
            } else {
                return std::format("{}", value);
            }
        }, var);
}

} // namespace detail

constexpr std::string_view shift(int& argc, char**& argv) {
    // This should be checked by the caller
    if (argc == 0) [[unlikely]] {
        return "";
    }
    --argc;
    return *argv++;
}

constexpr std::string_view peek(const int argc, char** argv) {
    // This should be checked by the caller
    if (argc == 0) [[unlikely]] {
        return "";
    }
    return argv[0];
}

struct Option {
    char alt                        = '\0';
    std::string_view name           = "";
    std::string_view description    = "";
    std::string_view value          = "";
    detail::DefaultT default_value  = std::monostate{};
};

inline std::vector<Option>& options() {
    static std::vector<Option> opts;
    return opts;
}

inline void add_option(const Option& opt) {
    assert((opt.alt != '\0' || !opt.name.empty()) && "Option must have either alt or name");
    assert((opt.alt == '\0' || (opt.alt >= 'a' && opt.alt <= 'z') || (opt.alt >= 'A' && opt.alt <= 'Z'))
           && "Option alt must be a letter");
    options().push_back(opt);
}

inline void add_positional(const std::string_view value) {
    options().push_back({.value = value});
}

namespace detail {

inline std::string get_usage_str(const std::string_view program_name) {
    std::string buffer = std::format("Usage: {}", program_name);
    if (!options().empty()) {
        buffer += " [OPTIONS]";
    }
    for (const auto& opt : options()) {
        if (opt.alt == '\0' && opt.name.empty()) {
            buffer += std::format(" {}", opt.value);
        }
    }
    return buffer;
}

} // namespace detail

inline void print_help(const std::string_view program_name) {
    std::cout << detail::get_usage_str(program_name) << '\n';
    std::cout << "Options:\n";

    std::size_t max_opt_len = 0;
    for (const Option& opt : options()) {
        std::size_t curr_opt_len = 0;
        if (opt.alt == '\0' && opt.name.empty()) {
            continue;
        }
        if (opt.alt != '\0') {
            curr_opt_len += 2;                    // "-x"
        }
        if (!opt.name.empty()) {
            if (curr_opt_len > 0) {
                curr_opt_len += 2;                // ", "
            }
            curr_opt_len += opt.name.size() + 2;  // "--name"
        }
        if (!opt.value.empty()) {
            curr_opt_len += opt.value.size() + 3; // " <value>"
        }
        max_opt_len = std::max(max_opt_len, curr_opt_len);
    }

    std::string buffer(max_opt_len, ' ');

    for (const Option& opt : options()) {
        buffer.clear();
        if (opt.alt == '\0' && opt.name.empty()) {
            continue;
        }
        if (opt.alt != '\0') {
            buffer += std::format("-{}", opt.alt);
        }
        if (!opt.name.empty()) {
            if (!buffer.empty()) {
                buffer += ", ";
            }
            buffer += std::format("--{}", opt.name);
        }
        if (!opt.value.empty()) {
            buffer += std::format(" <{}>", opt.value);
        }
        buffer += std::string(max_opt_len - buffer.size(), ' ');
        if (std::holds_alternative<std::monostate>(opt.default_value)) {
            std::cout << std::format("    {}    {}", buffer, opt.description) << '\n';
        } else {
            std::cout << std::format("    {}    {} (default: {})", buffer, opt.description,
                                     detail::to_formatted(opt.default_value))
                      << '\n';
        }
    }
}

} // namespace utils::cmd

#endif // CMDLINE_HPP
