#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"
#undef INFO
#undef WARNING
#undef ERROR

#include "log.hpp"

#include <exception>
#include <sstream>
#include <string>

using namespace utils::log;

// Helper struct to capture and restore std::cerr.
struct StderrRedirect {
    std::streambuf* old;
    std::ostringstream oss;
    StderrRedirect() : old(std::cerr.rdbuf(oss.rdbuf())) {}
    ~StderrRedirect() {
        std::cerr.rdbuf(old);
    }
};

TEST_CASE("Basic INFO logging") {
    detail::logger::instance().set_log_level(LogLevel::INFO);

    {
        const StderrRedirect redirect;
        INFO("Test info message: {}", 42);
        const std::string output = redirect.oss.str();

        CHECK(output.find("[INFO]") != std::string::npos);
        CHECK(output.find("Test info message: 42") != std::string::npos);
    }
}

TEST_CASE("Filter out lower debug level messages") {
    detail::logger::instance().set_log_level(LogLevel::INFO);
    {
        const StderrRedirect redirect;
        DEBUG("This debug should be skipped");
        const std::string output = redirect.oss.str();
        CHECK(output.empty());
    }
}

TEST_CASE("WARNING and ERROR logging output") {
    detail::logger::instance().set_log_level(LogLevel::DEBUG);
    {
        const StderrRedirect redirect;
        WARNING("Warning: {}", "check");
        ERROR("Error: code {}", 99);
        const std::string output = redirect.oss.str();

        CHECK(output.find("[WARNING]") != std::string::npos);
        CHECK(output.find("Warning: check") != std::string::npos);
        CHECK(output.find("[ERROR]") != std::string::npos);
        CHECK(output.find("Error: code 99") != std::string::npos);
    }
}

TEST_CASE("Log level filtering") {
    detail::logger::instance().set_log_level(LogLevel::WARNING);
    {
        const StderrRedirect redirect;
        INFO("This should be skipped");
        DEBUG("This should be skipped");
        WARNING("This should be shown");
        ERROR("This should be shown");
        const std::string output = redirect.oss.str();

        CHECK(output.find("INFO") == std::string::npos);
        CHECK(output.find("DEBUG") == std::string::npos);
        CHECK(output.find("WARNING") != std::string::npos);
        CHECK(output.find("ERROR") != std::string::npos);
    }
}

TEST_CASE("ASSERT") {
    detail::logger::instance().set_log_level(LogLevel::DEBUG);
    {
        const StderrRedirect redirect;
        ASSERT(true);
        const std::string output = redirect.oss.str();
        CHECK(output.empty());
    }
    {
        // This is pointless but whatever
        const StderrRedirect redirect;
        if constexpr (1 == 0) {
            UNREACHABLE("This should not trigger");
        }
        const std::string output = redirect.oss.str();
        CHECK(output.empty());
    }
}

TEST_CASE("Print example log messages") {
    detail::logger::instance().set_log_level(LogLevel::DEBUG);
    {
        DEBUG("This is a debug message");
        INFO("This is an info message");
        WARNING("This is a warning message");
        ERROR("This is an error message");
    }
}
