#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"

#include "cli.hpp"
#include "ext/doctest_extensions.hpp"

#include <string>
#include <vector>

using namespace utils::cli;

TEST_CASE("Arg stores options") {
    Arg opt1 = Arg::flag("all").short_alias('a').about("Show all entries");
    Arg opt2 = Arg::option("name").about("Specify name").value_name("name").default_value("default");

    Command cmd("test", "Test command");
    cmd.arg(opt1);
    cmd.arg(opt2);

    const auto& args = cmd.args();
    CHECK(args.size() == 3);
    CHECK(args[0].short_alias() == 'h');
    CHECK(args[0].name() == "help");
    CHECK(args[1].short_alias() == 'a');
    CHECK(args[1].name() == "all");
    CHECK(args[2].short_alias() == '\0');
    CHECK(args[2].name() == "name");
}

TEST_CASE("Positional and usage string") {
    Arg opt = Arg::flag("verbose").short_alias('v').about("Enable verbose mode");

    Command cmd("myprogram", "My program description");
    cmd.arg(opt);
    cmd.arg(arg("<input_file>"));

    testing::CaptureStdout();
    cmd.print_help();
    const std::string help = testing::GetCapturedStdout();

    CHECK(help.find("myprogram") != std::string::npos);
    CHECK(help.find("[OPTIONS]") != std::string::npos);
    CHECK(help.find("input_file") != std::string::npos);
}

TEST_CASE("print_help outputs formatted option list") {
    Arg opt = Arg::option("execute").short_alias('x').about("Run the command").value_name("cmd");

    Command cmd("cmdprog", "Command program");
    cmd.arg(opt);

    testing::CaptureStdout();
    cmd.print_help();
    const std::string help = testing::GetCapturedStdout();

    CHECK(help.find("-x") != std::string::npos);
    CHECK(help.find("--execute") != std::string::npos);
    CHECK(help.find("cmd") != std::string::npos);
    CHECK(help.find("Run the command") != std::string::npos);
}

TEST_CASE("shift and peek functions work correctly") {
    const char* argv_array[] = {"prog", "arg1", "arg2"};
    int argc = 3;
    char** argv = const_cast<char**>(argv_array);

    auto first_opt = peek(argc, argv);
    CHECK(first_opt.has_value());
    CHECK(first_opt.value() == "prog");

    auto shifted_opt = shift(argc, argv);
    CHECK(shifted_opt.has_value());
    CHECK(shifted_opt.value() == "prog");

    CHECK(argc == 2);

    auto next_opt = peek(argc, argv);
    CHECK(next_opt.has_value());
    CHECK(next_opt.value() == "arg1");
}

TEST_CASE("Print an example program help string") {
    Command cmd("myprogram", "My program description");
    cmd.arg(Arg::option("input").short_alias('i').about("Set input file").value_name("file"));
    cmd.arg(Arg::flag("output").short_alias('o').long_alias("output").about("Set output file"));
    cmd.arg(Arg::flag("verbose").short_alias('v').long_alias("verbose").about("Enable verbose mode").default_value(false));
    cmd.arg(Arg::option("fps").short_alias('f').long_alias("fps").about("Set frames per second").value_name("fps").default_value(60));
    cmd.arg(Arg::option("format").about("Set output format").value_name("format").default_value("mp4"));
    cmd.arg(Arg::positional("FILE").about("Input file to process").required(true));

    cmd.subcommand(Command("subcmd", "Subcommand description"));

    Command subcmd("another", "Another subcommand");
    subcmd.arg(Arg::flag("another-option").short_alias('a').long_alias("another-option").about("Another option"));

    cmd.subcommand(subcmd);

    testing::CaptureStdout();
    cmd.print_help();
    const std::string help = testing::GetCapturedStdout();

    CHECK(help == R"(My program description
Usage: myprogram <FILE> <COMMAND> [OPTIONS]

Commands:
    subcmd     Subcommand description
    another    Another subcommand

Options:
    -h, --help            Show this help message
    -i, --input <file>    Set input file
    -o, --output          Set output file
    -v, --verbose         Enable verbose mode (default: false)
    -f, --fps <fps>       Set frames per second (default: 60)
    --format <format>     Set output format (default: "mp4")
)");

    const char* argv_array[] = {"myprogram", "--output", "-v", "input.txt"};
    int argc = 4;
    char** argv = const_cast<char**>(argv_array);

    auto [matches, err] = cmd.get_matches(argc, argv);
    CHECK(!err.has_error());

    CHECK(matches.get_flag("output") == true);
    CHECK(matches.get_flag("verbose") == true);
    CHECK(matches.get_one("fps").has_value());
    CHECK(matches.get_one("fps").value() == "60");
    CHECK(matches.get_one<int>("fps").value() == 60);
    CHECK(matches.get_one<std::string>("format").value() == "mp4");

    auto input_file = matches.get_one("FILE");
    CHECK(input_file.has_value());
    CHECK(input_file.value() == "input.txt");

    testing::CaptureStdout();
    subcmd.print_help();
    const std::string subcmd_help = testing::GetCapturedStdout();

    CHECK(subcmd_help == R"(Another subcommand
Usage: another [OPTIONS]

Options:
    -h, --help              Show this help message
    -a, --another-option    Another option
)");
}

TEST_CASE("Clap-like API usage example") {
    const Command cmd = Command("gz", "Description")
        .subcommand(
            Command("sync", "Sync current branch with origin/main")
                .arg(Arg::flag("force").short_alias('f').about("Force reset instead of pull"))
        )
        .subcommand(Command("stash", "Stash local changes including untracked files"))
        .subcommand(
            Command("uncommit", "Uncommit last N commits")
                .arg(Arg::positional("count").about("Number of commits to uncommit").default_value("1"))
        )
        .subcommand(
            Command("branch", "Create and switch to a new branch")
                .arg(Arg::positional("name").about("Branch name").required(true))
        )
        .subcommand(Command("add", "Launch TUI to stage and unstage files"))
        .subcommand(Command("done", "Switch back to main and delete current branch"));

    CHECK(cmd.name() == "gz");
    CHECK(cmd.subcommands().size() == 6);

    const char* argv_sync[] = {"gz", "sync", "--force"};
    int argc_sync = 3;

    auto [matches_sync, errs] = cmd.get_matches(argc_sync, const_cast<char**>(argv_sync));
    CHECK(!errs.has_error());

    auto subcommand_result = matches_sync.subcommand();
    CHECK(subcommand_result.has_value());
    CHECK(subcommand_result->first == "sync");
    CHECK(subcommand_result->second->get_flag("force") == true);

    // Test positional argument parsing
    const char* argv_branch[] = {"gz", "branch", "feature-xyz"};
    int argc_branch = 3;

    auto [matches_branch, errb] = cmd.get_matches(argc_branch, const_cast<char**>(argv_branch));
    CHECK(!errb.has_error());

    auto branch_subcommand = matches_branch.subcommand();
    CHECK(branch_subcommand.has_value());
    CHECK(branch_subcommand->first == "branch");
    auto name = branch_subcommand->second->get_one<std::string>("name");
    CHECK(name.has_value());
    CHECK(name.value() == "feature-xyz");
}

TEST_CASE("Arg helper function tests") {
    Arg flag_arg = arg("-v --verbose");
    CHECK(flag_arg.type() == ArgType::Flag);
    CHECK(flag_arg.name() == "verbose");
    CHECK(flag_arg.short_alias() == 'v');
    CHECK(flag_arg.long_alias() == "verbose");

    Arg pos_required = arg("<filename>");
    CHECK(pos_required.type() == ArgType::Positional);
    CHECK(pos_required.name() == "filename");
    CHECK(pos_required.is_required());

    Arg pos_optional = arg("[count]");
    CHECK(pos_optional.type() == ArgType::Positional);
    CHECK(pos_optional.name() == "count");
    CHECK(!pos_optional.is_required());
}

TEST_CASE("direct ArgMatches functionality") {
    ArgMatches matches;

    matches.set_flag("verbose", true);
    CHECK(matches.get_flag("verbose") == true);
    CHECK(matches.get_flag("quiet") == false);

    matches.add_value("output", "file.txt");
    auto output = matches.get_one<std::string>("output");
    CHECK(output.has_value());
    CHECK(output.value() == "file.txt");

    matches.add_value("files", "file1.txt");
    matches.add_value("files", "file2.txt");
    auto files = matches.get_many("files");
    CHECK(!files.empty());
    CHECK(files.size() == 2);
    CHECK(files[0] == "file1.txt");
    CHECK(files[1] == "file2.txt");
}

TEST_CASE("required argument validation") {
    Command cmd("test", "Test command");
    cmd.arg(Arg::option("name").required(true).about("Required name option"));
    cmd.arg(Arg::positional("file").required(true).about("Required file argument"));

    const char* argv[] = {"test"};
    int argc = 1;

    auto [_, err] = cmd.get_matches(argc, const_cast<char**>(argv));
    CHECK(err.has_error());
    CHECK(err.message == "Missing required argument 'name' for command 'test'");
}

TEST_CASE("subcommand required functionality") {
    Command cmd("app", "Application with required subcommands");
    cmd.subcommand_required(true);
    cmd.subcommand(Command("start", "Start the service"));
    cmd.subcommand(Command("stop", "Stop the service"));

    CHECK(cmd.subcommands().size() == 2);

    // Test with valid subcommand
    const char* argv_valid[] = {"app", "start"};
    int argc_valid = 2;

    auto [matches_valid, err] = cmd.get_matches(argc_valid, const_cast<char**>(argv_valid));
    CHECK(!err.has_error());

    auto subcommand_result = matches_valid.subcommand();
    CHECK(subcommand_result.has_value());
    CHECK(subcommand_result->first == "start");

    // Test with missing subcommand
    const char* argv_missing[] = {"app"};
    int argc_missing = 1;

    auto [_, errm] = cmd.get_matches(argc_missing, const_cast<char**>(argv_missing));
    CHECK(errm.has_error());
    CHECK(errm.message == "Missing required subcommand for command 'app'");
}

TEST_CASE("mixed required arguments and subcommands") {
    Command cmd("deploy", "Deployment tool");
    cmd.arg(Arg::option("env").required(true).about("Environment to deploy to"));
    cmd.subcommand_required(true);
    cmd.subcommand(
        Command("web", "Deploy web application")
            .arg(Arg::positional("version").required(true).about("Version to deploy"))
    );

    const char* argv[] = {"deploy", "--env", "production", "web", "1.2.3"};
    int argc = 5;

    auto [matches, err] = cmd.get_matches(argc, const_cast<char**>(argv));
    CHECK(!err.has_error());

    auto env = matches.get_one<std::string>("env");
    CHECK(env.has_value());
    CHECK(env.value() == "production");

    auto subcommand_result = matches.subcommand();
    CHECK(subcommand_result.has_value());
    CHECK(subcommand_result->first == "web");

    auto version = subcommand_result->second->get_one<std::string>("version");
    CHECK(version.has_value());
    CHECK(version.value() == "1.2.3");
}

TEST_CASE("Unix -- convention support") {
    Command cmd("app", "Application with -- support");
    cmd.arg(Arg::flag("verbose").short_alias('v').about("Verbose output"));
    cmd.arg(Arg::option("output").short_alias('o').about("Output file"));
    cmd.arg(Arg::positional("files").multiple(true).about("Input files"));

    const char* argv[] = {"app", "-v", "--output", "result.txt", "--", "--weird-file", "-another-file"};
    int argc = 7;

    auto [matches, err] = cmd.get_matches(argc, const_cast<char**>(argv));
    CHECK(!err.has_error());

    CHECK(matches.get_flag("verbose") == true);

    auto output = matches.get_one<std::string>("output");
    CHECK(output.has_value());
    CHECK(output.value() == "result.txt");

    auto files = matches.get_many("files");
    CHECK(files.size() == 2);
    CHECK(files[0] == "--weird-file");
    CHECK(files[1] == "-another-file");
}

TEST_CASE("multiple value arguments") {
    Command cmd("compile", "Compiler tool");
    cmd.arg(Arg::option("include").short_alias('I').multiple(true).about("Include directories"));
    cmd.arg(Arg::option("define").short_alias('D').multiple(true).about("Preprocessor definitions"));
    cmd.arg(Arg::option("optimize").short_alias('O').multiple(true));
    cmd.arg(Arg::positional("sources").multiple(true).about("Source files"));

    const char* argv[] = {"compile", "-I", "/usr/include", "-I", "/opt/include", "-D", "DEBUG", "-D", "VERBOSE", "-O", "2", "-O", "3", "main.cpp", "utils.cpp"};
    int argc = 15;

    auto [matches, err] = cmd.get_matches(argc, const_cast<char**>(argv));
    CHECK(!err.has_error());

    auto includes = matches.get_many("include");
    CHECK(includes.size() == 2);
    CHECK(includes[0] == "/usr/include");
    CHECK(includes[1] == "/opt/include");

    auto defines = matches.get_many("define");
    CHECK(defines.size() == 2);
    CHECK(defines[0] == "DEBUG");
    CHECK(defines[1] == "VERBOSE");

    auto optimizations = matches.get_many<int>("optimize");
    CHECK(optimizations.size() == 2);
    CHECK(optimizations[0] == 2);
    CHECK(optimizations[1] == 3);

    auto sources = matches.get_many("sources");
    CHECK(sources.size() == 2);
    CHECK(sources[0] == "main.cpp");
    CHECK(sources[1] == "utils.cpp");
}

TEST_CASE("mixed -- and multiple values") {
    Command cmd("grep", "Grep-like tool");
    cmd.arg(Arg::option("pattern").short_alias('e').multiple(true).about("Search patterns"));
    cmd.arg(Arg::positional("files").multiple(true).about("Files to search"));

    const char* argv[] = {"grep", "-e", "error", "-e", "warning", "--", "-special-file", "normal.txt"};
    int argc = 8;

    auto [matches, err] = cmd.get_matches(argc, const_cast<char**>(argv));
    CHECK(!err.has_error());

    auto patterns = matches.get_many("pattern");
    CHECK(patterns.size() == 2);
    CHECK(patterns[0] == "error");
    CHECK(patterns[1] == "warning");

    auto files = matches.get_many("files");
    CHECK(files.size() == 2);
    CHECK(files[0] == "-special-file");
    CHECK(files[1] == "normal.txt");
}
