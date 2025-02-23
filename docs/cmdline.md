# utils::cmd

Provides utilities for parsing command line arguments.

## Usage

### Example usage
```c++
utils::cmd::add_option(
    {.name = "max-fps", .description = "Set maximum frames per second", .value = "fps", .default_value = 144});
utils::cmd::add_option({.alt = 'h', .name = "help", .description = "Show this help message"});
utils::cmd::add_positional("FILE");

if (argc < 2) {
    std::cerr << "Not enough arguments" << '\n';
    utils::cmd::print_help(utils::cmd::peek(argc, argv));
    return 1;
}

const auto program_name = utils::cmd::shift(argc, argv); // Skip program name

while (argc > 0) {
    const std::string_view arg = utils::cmd::shift(argc, argv);

    if (arg == "-h" || arg == "--help") {
        utils::cmd::print_help(program_name);
        return 0;
    }
    if (arg == "--max-fps") {
        const auto fps_str = utils::cmd::shift(argc, argv);
        // do stuff
    } else {
        // positional argument stored in arg
    }
}

// Example help message
// Usage: ./name [OPTIONS] FILE
// Options:
//     --max-fps <fps>    Set maximum frames per second (default: 144)
//     -h, --help         Show this help message
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

### Adding options
```c++
// You can leave fields empty but you MUST provide either an alt or a name
void add_option(const Option& opt);
void add_positional(const std::string_view value)

std::vector<Option>& options() // if you want access to stored options
```

### Printing help
```c++
void print_help(const std::string_view program_name)
```

