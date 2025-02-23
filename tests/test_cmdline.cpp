#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"

#include "cmdline.hpp"

#include <sstream>
#include <string>
#include <variant>
#include <vector>

using namespace utils::cmd;

struct CmdlineTestGuard {
    CmdlineTestGuard() {
        options().clear();
    }
    ~CmdlineTestGuard() {
        options().clear();
    }
};

TEST_CASE("add_option stores options") {
    const CmdlineTestGuard guard;
    constexpr Option opt1{.alt = 'a', .name = "all", .description = "Show all entries"};
    constexpr Option opt2{
        .alt = '\0', .name = "name", .description = "Specify name", .value = "name", .default_value = "default"};

    add_option(opt1);
    add_option(opt2);

    const auto& opts = options();
    CHECK(opts.size() == 2);
    CHECK(opts[0].alt == 'a');
    CHECK(opts[1].name == "name");
}

TEST_CASE("add_positional and usage string") {
    const CmdlineTestGuard guard;
    constexpr Option opt{.alt = 'v', .name = "verbose", .description = "Enable verbose mode"};
    add_option(opt);
    add_positional("input_file");

    const std::string usage = detail::get_usage_str("myprogram");
    CHECK(usage.find("myprogram") != std::string::npos);
    CHECK(usage.find("[OPTIONS]") != std::string::npos);
    CHECK(usage.find("input_file") != std::string::npos);
}

TEST_CASE("print_help outputs formatted option list") {
    const CmdlineTestGuard guard;
    constexpr Option opt{.alt = 'x',
                         .name = "execute",
                         .description = "Run the command",
                         .value = "const cmd",
                         .default_value = std::monostate{}};
    add_option(opt);

    const std::ostringstream oss;
    auto* old_buf = std::cout.rdbuf(oss.rdbuf());
    print_help("cmdprog");
    std::cout.rdbuf(old_buf);

    const std::string help_output = oss.str();
    CHECK(help_output.find("-x") != std::string::npos);
    CHECK(help_output.find("--execute") != std::string::npos);
    CHECK(help_output.find("cmd") != std::string::npos);
    CHECK(help_output.find("Run the command") != std::string::npos);
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
    add_option({.alt = 'i', .description = "Set input file", .value = "file"});
    add_option({
        .alt = 'o',
        .name = "output",
        .description = "Set output file",
    });
    add_option({.alt = 'v', .name = "verbose", .description = "Enable verbose mode", .default_value = false});
    add_option(
        {.alt = 'f', .name = "fps", .description = "Set frames per second", .value = "fps", .default_value = 60});
    add_option({.name = "format", .description = "Set output format", .value = "format", .default_value = "mp4"});
    add_option({
        .alt = 'h',
        .name = "help",
        .description = "Print this help message",
    });

    add_positional("FILE");

    print_help("myprogram");
}
