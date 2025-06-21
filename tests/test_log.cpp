#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#undef INFO
#undef WARNING
#undef ERROR

#include "log.hpp"

#include <sstream>
#include <string>

using namespace utils::log;

// Helper struct to capture and restore std::cerr.
struct ScopedRedirect {
    ScopedRedirect() : original(std::cerr.rdbuf()) {
        std::cerr.rdbuf(buffer.rdbuf());
    }

    ~ScopedRedirect() {
        std::cerr.rdbuf(original);
    }

    std::ostringstream buffer;
    std::streambuf* original;
};

TEST_CASE("basic INFO logging") {
    detail::logger::instance().set_log_level(LogLevel::INFO);
    {
        const ScopedRedirect redirect;
        INFO("Test info message: {}", 42);
        const std::string output = redirect.buffer.str();

        CHECK(output.find("[INFO]") != std::string::npos);
        CHECK(output.find("Test info message: 42") != std::string::npos);
    }
}

TEST_CASE("filter out lower debug level messages") {
    detail::logger::instance().set_log_level(LogLevel::INFO);
    {
        const ScopedRedirect redirect;
        DEBUG("This debug should be skipped");
        const std::string output = redirect.buffer.str();
        CHECK(output.empty());
    }
}

TEST_CASE("WARNING and ERROR logging output") {
    detail::logger::instance().set_log_level(LogLevel::DEBUG);
    {
        const ScopedRedirect redirect;
        WARNING("Warning: {}", "check");
        ERROR("Error: code {}", 99);
        const std::string output = redirect.buffer.str();

        CHECK(output.find("[WARNING]") != std::string::npos);
        CHECK(output.find("Warning: check") != std::string::npos);
        CHECK(output.find("[ERROR]") != std::string::npos);
        CHECK(output.find("Error: code 99") != std::string::npos);
    }
}

TEST_CASE("log level filtering") {
    detail::logger::instance().set_log_level(LogLevel::WARNING);
    {
        const ScopedRedirect redirect;
        INFO("This should be skipped");
        DEBUG("This should be skipped");
        WARNING("This should be shown");
        ERROR("This should be shown");
        const std::string output = redirect.buffer.str();

        CHECK(output.find("INFO") == std::string::npos);
        CHECK(output.find("DEBUG") == std::string::npos);
        CHECK(output.find("WARNING") != std::string::npos);
        CHECK(output.find("ERROR") != std::string::npos);
    }
}

TEST_CASE("print example log messages") {
    detail::logger::instance().set_log_level(LogLevel::DEBUG);
    DEBUG("This is a debug message");
    INFO("This is an info message");
    WARNING("This is a warning message");
    ERROR("This is an error message");
}
