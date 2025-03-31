#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"

#include "cmdline.hpp"

#include <sstream>
#include <string>
#include <variant>
#include <vector>

using namespace utils::cmd;

// Helper struct to capture and restore std::cout.
struct ScopedRedirect {
    ScopedRedirect() : original(std::cout.rdbuf()) {
        std::cout.rdbuf(buffer.rdbuf());
    }

    ~ScopedRedirect() {
        std::cout.rdbuf(original);
    }

    std::ostringstream buffer;
    std::streambuf* original;
};

TEST_CASE("add_option stores options") {
    constexpr Option opt1{.alt = 'a', .name = "all", .description = "Show all entries"};
    constexpr Option opt2{
        .alt = '\0', .name = "name", .description = "Specify name", .value = "name", .default_value = "default"};

    Command cmd("test", "Test command");
    cmd.add_option(opt1);
    cmd.add_option(opt2);

    const auto& opts = cmd.options();
    CHECK(opts.size() == 2);
    CHECK(opts[0].alt == 'a');
    CHECK(opts[1].name == "name");
}

TEST_CASE("add_positional and usage string") {
    constexpr Option opt{.alt = 'v', .name = "verbose", .description = "Enable verbose mode"};

    Command cmd("myprogram", "My program description");
    cmd.add_option(opt);
    cmd.add_positional("input_file");

    const ScopedRedirect redirect;
    cmd.print_help();
    const std::string help = redirect.buffer.str();

    CHECK(help.find("myprogram") != std::string::npos);
    CHECK(help.find("[OPTIONS]") != std::string::npos);
    CHECK(help.find("input_file") != std::string::npos);
}

TEST_CASE("print_help outputs formatted option list") {
    constexpr Option opt{.alt = 'x',
                         .name = "execute",
                         .description = "Run the command",
                         .value = "const cmd",
                         .default_value = std::monostate{}};

    Command cmd("cmdprog", "Command program");
    cmd.add_option(opt);

    const ScopedRedirect redirect;
    cmd.print_help();
    const std::string help = redirect.buffer.str();

    CHECK(help.find("-x") != std::string::npos);
    CHECK(help.find("--execute") != std::string::npos);
    CHECK(help.find("cmd") != std::string::npos);
    CHECK(help.find("Run the command") != std::string::npos);
}

TEST_CASE("shift and peek functions work correctly") {
    const char* argv_array[] = {"prog", "arg1", "arg2"};
    int argc = 3;
    char** argv = const_cast<char**>(argv_array);

    const std::string_view first = peek(argc, argv);
    CHECK(first == "prog");

    const std::string_view shifted = shift(argc, argv);
    CHECK(shifted == "prog");

    // After shifting one, argc should be 2 and next argument "arg1"
    CHECK(argc == 2);
    CHECK(peek(argc, argv) == "arg1");
}

TEST_CASE("Print an example program help string") {
    Command cmd("myprogram", "My program description");
    cmd.add_option({.alt = 'i', .description = "Set input file", .value = "file"});
    cmd.add_option({
        .alt = 'o',
        .name = "output",
        .description = "Set output file",
    });
    cmd.add_option({.alt = 'v', .name = "verbose", .description = "Enable verbose mode", .default_value = false});
    cmd.add_option(
        {.alt = 'f', .name = "fps", .description = "Set frames per second", .value = "fps", .default_value = 60});
    cmd.add_option({.name = "format", .description = "Set output format", .value = "format", .default_value = "mp4"});
    cmd.add_option({
        .alt = 'h',
        .name = "help",
        .description = "Print this help message",
    });

    cmd.add_positional("FILE");

    cmd.add_subcommand(Command("subcmd", "Subcommand description"));

    Command subcmd("another", "Another subcommand");
    subcmd.add_option({
        .alt = 'a',
        .name = "another-option",
        .description = "Another option",
    });

    cmd.add_subcommand(subcmd);

    ScopedRedirect redirect;
    cmd.print_help();
    const std::string help = redirect.buffer.str();
    CHECK(help == R"(Usage: myprogram FILE <COMMAND> [OPTIONS]
My program description

Commands:
    subcmd     Subcommand description
    another    Another subcommand

Options:
    -i <file>            Set input file
    -o, --output         Set output file
    -v, --verbose        Enable verbose mode (default: false)
    -f, --fps <fps>      Set frames per second (default: 60)
    --format <format>    Set output format (default: "mp4")
    -h, --help           Print this help message
)");

    redirect.buffer.str("");
    subcmd.print_help();
    const std::string subcmd_help = redirect.buffer.str();
    CHECK(subcmd_help == R"(Usage: another [OPTIONS]
Another subcommand

Options:
    -a, --another-option    Another option
)");
}
