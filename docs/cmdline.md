# utils::cmd

Provides utilities for parsing command line arguments.

## Usage

### Example usage
```c++
utils::cmd::Command cmd("prog", "Program description");
cmd.add_option(
    {.name = "max-fps", .description = "Set maximum frames per second", .value = "fps", .default_value = 144});
cmd.add_option({.alt = 'h', .name = "help", .description = "Show this help message"});
cmd.add_positional("FILE");

utils::cmd::Command subcmd("subprog", "Subprogram description");
subcmd.add_option({.alt = 'h', .name = "help", .description = "Show this help message"});

cmd.add_subcommand(subcmd);

if (argc < 2) {
    std::cerr << "Not enough arguments" << '\n';
    cmd.print_help();
    return 1;
}

utils::cmd::shift(argc, argv); // Skip program name

while (argc > 0) {
    const std::string_view arg = utils::cmd::shift(argc, argv);

    if (arg == "-h" || arg == "--help") {
        cmd.print_help();
        return 0;
    }
    if (arg == "--max-fps") {
        const auto fps_str = utils::cmd::shift(argc, argv);
        // do stuff
    } else if (arg == "subprog") {
        // Handle subcommand
        while (argc > 0) {
            const std::string_view sub_arg = utils::cmd::shift(argc, argv);
            if (sub_arg == "-h" || sub_arg == "--help") {
                subcmd.print_help();
                return 0;
            }
            // Handle other subcommand arguments
        }
    } else {
        // Handle positional argument
        std::string_view file = arg;
        // do stuff with file
    }
}

// ./prog -h
// Usage: prog FILE <COMMAND> [OPTIONS]
// Program description
// 
// Commands:
//     subprog    Subprogram description
// 
// Options:
//     --max-fps <fps>    Set maximum frames per second (default: 144)
//     -h, --help         Show this help message
//
// ./prog subprog -h
// Usage: subprog [OPTIONS]
// Subprogram description
// 
// Options:
//     -h, --help    Show this help message// 
```

### Parsing arguments
```c++
// Shifts the arguments by one and returns the shifted argument
std::string_view shift(int& argc, char**& argv);

// Returns the next argument without shifting
std::string_view peek(const int argc, char** argv);
```

### Option format
```c++
// All possible types you can use for default values
using DefaultT = std::variant<bool, int, unsigned int, std::int64_t, std::uint64_t, float, double, long double, char,
                              std::string_view, std::monostate>;
                              
struct Option {
    char alt                        = '\0';
    std::string_view name           = "";
    std::string_view description    = "";
    std::string_view value          = "";
    detail::DefaultT default_value  = std::monostate{};
};
```

### Command Class API
```c++
explicit Command(const std::string_view name, const std::string_view description = "");

Command& add_option(const Option& opt); // You can leave fields empty but you MUST provide either an alt or a name
Command& add_positional(const std::string_view value);
Command& add_subcommand(const Command& cmd);

// Get Command properties
const std::string& name() const;
const std::string& description() const;

const std::vector<Option>& options() const;
const std::vector<std::string_view>& positionals() const;
const std::vector<Command>& subcommands() const;

void clear(); // Clears all options, positionals and subcommands

void print_help() const;
```
