#ifndef CMDLINE_HPP
#define CMDLINE_HPP

#include <cassert>
#include <format>
#include <iostream>
#include <string_view>
#include <variant>
#include <vector>

// TODOs
// - Positional argument handling is very barebones

namespace utils::cmd {

namespace detail {

using DefaultT = std::variant<bool, int, unsigned int, std::int64_t, std::uint64_t, float, double, long double, char,
                              std::string_view, std::monostate>;

constexpr std::string to_formatted(const DefaultT& var) {
    return std::visit([]<typename T>(T&& value) -> std::string {
        using U = std::remove_cvref_t<T>;
        if constexpr (std::is_same_v<U, std::monostate>) {
            return {};
        } else if constexpr (std::is_same_v<U, char>) {
            return std::format("'{}'", std::forward<T>(value));
        } else if constexpr (std::is_same_v<U, std::string_view>) {
            return std::format("\"{}\"", std::forward<T>(value));
        } else {
            return std::format("{}", std::forward<T>(value));
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

class Command {
public:
    explicit Command(const std::string_view name, const std::string_view description = "")
        : name_(name), description_(description) {}

    Command& add_option(const Option& opt) {
        assert((opt.alt != '\0' || !opt.name.empty()) && "Option must have either alt or name");
        assert((opt.alt == '\0' || (opt.alt >= 'a' && opt.alt <= 'z') || (opt.alt >= 'A' && opt.alt <= 'Z')) &&
               "Option alt must be a letter");

        std::size_t curr_opt_len = 0;
        if (opt.alt != '\0') {
            curr_opt_len += 2;                      // "-x"
        }
        if (!opt.name.empty()) {
            if (curr_opt_len > 0) {
                curr_opt_len += 2;                  // ", "
            }
            curr_opt_len += opt.name.size() + 2;    // "--name"
        }
        if (!opt.value.empty()) {
            curr_opt_len += opt.value.size() + 3;   // " <value>"
        }
        if (curr_opt_len > max_opt_len) {
            max_opt_len = curr_opt_len;
        }

        options_.push_back(opt);
        return *this;
    }

    Command& add_positional(const std::string_view value) {
        assert(!value.empty() && "Positional argument must have a name");
        positionals_.push_back(value);
        return *this;
    }

    Command& add_subcommand(const Command& cmd) {
        assert(!cmd.name().empty() && "Subcommand must have a name");
        if (cmd.name().size() > max_cmd_len) {
            max_cmd_len = cmd.name().size();
        }
        subcommands_.push_back(cmd);
        return *this;
    }

    const std::string& name() const {
        return name_;
    }

    const std::string& description() const {
        return description_;
    }

    const std::vector<Option>& options() const {
        return options_;
    }

    const std::vector<std::string_view>& positionals() const {
        return positionals_;
    }

    const std::vector<Command>& subcommands() const {
        return subcommands_;
    }

    void clear() {
        options_.clear();
        subcommands_.clear();
        max_opt_len = 0;
        max_cmd_len = 0;
    }

    void print_help() {
        std::string buffer = std::format("Usage: {}", name_);
        if (!positionals_.empty()) {
            buffer += std::format(" {}", positionals_.front());
            for (std::size_t i = 1; i < positionals_.size(); ++i) {
                buffer += std::format(" [{}]", positionals_[i]);
            }
        }
        if (!subcommands_.empty()) {
            buffer += " <COMMAND>";
        }
        if (!options_.empty()) {
            buffer += " [OPTIONS]";
        }
        buffer += "\n";
        if (!description_.empty()) {
            buffer += description_;
        }
        buffer += '\n';

        if (!subcommands_.empty()) {
            buffer += "\nCommands:\n";
            for (const Command& cmd : subcommands_) {
                buffer += std::format("    {}{}", cmd.name(), std::string(max_cmd_len - cmd.name().size(), ' '));
                if (!cmd.description().empty()) {
                    buffer += std::format("    {}", cmd.description());
                }
                buffer += '\n';
            }
        }

        if (!options_.empty()) {
            buffer += "\nOptions:\n";
            std::string opt_buffer(max_opt_len, ' ');
            for (const Option& opt : options_) {
                opt_buffer.clear(); // doesn't change capacity
                if (opt.alt != '\0') {
                    opt_buffer += std::format("-{}", opt.alt);
                }
                if (!opt.name.empty()) {
                    if (!opt_buffer.empty()) {
                        opt_buffer += ", ";
                    }
                    opt_buffer += std::format("--{}", opt.name);
                }
                if (!opt.value.empty()) {
                    opt_buffer += std::format(" <{}>", opt.value);
                }
                if (opt_buffer.size() < max_opt_len) {
                    opt_buffer += std::string(max_opt_len - opt_buffer.size(), ' ');
                }
                if (std::holds_alternative<std::monostate>(opt.default_value)) {
                    buffer += std::format("    {}    {}\n", opt_buffer, opt.description);
                } else {
                    buffer += std::format("    {}    {} (default: {})\n", opt_buffer, opt.description,
                                          detail::to_formatted(opt.default_value));
                }
            }
        }

        std::cout.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        if (buffer.back() != '\n') {
            std::cout << '\n';
        }
    }

private:
    std::string name_;
    std::string description_;

    std::vector<Option> options_;
    std::vector<std::string_view> positionals_;
    std::vector<Command> subcommands_;

    std::size_t max_opt_len = 0;
    std::size_t max_cmd_len = 0;
};

} // namespace utils::cmd

#endif // CMDLINE_HPP
