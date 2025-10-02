# utils::cli

Provides a similar API to Rust's clap library for parsing command line arguments.

## Usage

### Example usage
```c++
const Command app = Command("lwk", "Kill processes by port number or predefined names")
    .arg(arg("--ss")
        .about("Use 'ss' to find processes (default)"))
    .arg(arg("--lsof")
        .about("Use 'lsof' to find processes"))
    .arg(arg("-p --port <PORT>")
        .about("Kill processes using the specified port"))
    .arg(arg("<NAME>")
        .required(false))
    .subcommand(
        Command("read", "Read port mappings from a file")
            .arg(Arg::positional("name").about("File name").required() // arg() helper can also be used here
    )
    .arg(arg("-h --help")
        .about("Show this help message"));

auto [matches, err] = app.get_matches(argc, argv);

if (err.has_error()) {
    std::println(stderr, "Error parsing arguments: {}", err.message);
    app.print_help();
    return 1;
}

if (matches.get_flag("help")) {
    app.print_help();
    return 0;
}

const PortListener listener = matches.get_flag("lsof") ? PortListener::LSOF : PortListener::SS;

// Check for port flag
if (const auto port_opt = matches.get_one<int>("port"); port_opt.has_value()) {
    return handle_port(*port_opt, listener);
}

// ...

return 0;

// ./lwk -h
//Usage: lwk [NAME] <COMMAND> [OPTIONS]
//Kill processes by port number or predefined names
//
//Commands:
//    read    Read port mappings from a file
//
//Options:
//    --ss                 Use 'ss' to find processes (default)
//    --lsof               Use 'lsof' to find processes
//    -p, --port <PORT>    Kill processes using the specified port
//    -h, --help           Show this help message
```

### Parsing helpers
```c++
// Shifts the arguments by one and returns the shifted argument
std::optional<std::string_view> shift(int& argc, char**& argv);

// Returns the next argument without shifting
std::optional<std::string_view> peek(const int argc, char** argv);
```

### Arg helper
```c++
// Helper that parses a spec and returns an Arg
// Supported forms:
//   "-h"                     -> flag with short alias
//   "--help"                 -> flag with long alias
//   "-h --help"              -> flag with short and long aliases
//   "--output <FILE>"        -> option that takes a value
//   "-o --output <FILE>"     -> option with short and long aliases that takes a value
//   "<name>"                 -> required positional
//   "[count]"                -> optional positional
constexpr Arg arg(std::string_view spec);
```

### Arg class
```c++
// Possible types for default values
using ValueType = std::variant<std::monostate, char, std::string, bool, i64, u64, f64>;

class Arg {
public:
    explicit Arg(std::string_view name);

    // Arg types
    static Arg flag(std::string_view name);
    static Arg option(std::string_view name);
    static Arg positional(std::string_view name);

    // Builder methods
    Arg& short_alias(char c);
    Arg& long_alias(std::string_view name);
    Arg& about(std::string_view description);
    Arg& value_name(std::string_view name);     // only meaningful for options
    Arg& default_value(const ValueType& value);
    Arg& required(bool req = true);
    Arg& multiple(bool multiple = true);

    // Getters
    const std::string& name() const;
    ArgType type() const;
    char short_alias() const;
    const std::string& long_alias() const;
    const std::string& description() const;
    const std::string& value_name() const;
    const ValueType& default_value() const;
    bool is_required() const;
    bool is_multiple() const;
};
```

### Command class
```c++
class Command {
public:
    explicit Command(std::string_view name, std::string_view description = "");

    Command& about(std::string_view description);
    Command& arg(const Arg& arg);                // add arguments
    Command& subcommand(const Command& cmd);     // add subcommand
    Command& subcommand_required(bool required = true);

    // Does the actual parsing
    ParseResult get_matches(int argc, char** argv) const;

    // Getters
    const std::string& name() const;
    const std::string& description() const;
    const std::vector<Command>& subcommands() const;
    const std::vector<Arg>& args() const;

    // Utilities
    void clear();
    void print_help() const;                     // pretty print help message to stdout
};
```

### ParseResult struct
```c++
class ArgMatches {
public:
    bool get_flag(const std::string& name) const;

    // Returns parsed value if present
    // Returns first collected value for multi-value args
    // Supported T:
    //   - std::string
    //   - arithmetic types via std::from_chars
    //   - bool
    //      - "true", "1", "yes", "on"  -> true
    //      - "false", "0", "no", "off" -> false
    template <typename T = std::string>
    std::optional<T> get_one(const std::string& name) const;

    // Returns all collected values for a multi-value arg
    std::vector<std::string> get_many(std::string_view name) const;

    // If a subcommand was matched, returns {name, unique_ptr<ArgMatches>}
    std::optional<std::pair<std::string, std::unique_ptr<ArgMatches>>> subcommand() const;
};

struct ParseError {
    enum class ParseErrorType : u8 {
        None,
        MissingRequiredArgument,
        MissingRequiredSubcommand,
    };

    ParseErrorType type;
    std::string message;

    bool has_error() const;

    static ParseError None();
    static ParseError MissingRequiredArgument(const std::string& cmd_name, const std::string& arg_name);
    static ParseError MissingRequiredSubcommand(const std::string& cmd_name);
};

struct ParseResult {
    ArgMatches matches;
    ParseError error;
};
```

#### Notes
- `--` Unix convention for stopping argument parsing is supported
