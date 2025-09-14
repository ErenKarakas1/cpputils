#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "process.hpp"

#include <array>
#include <filesystem>

using namespace utils::process;

namespace {

bool create_test_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file) return false;
    file << content;
    return file.good();
}

// Helper function to read a file's content
std::string read_file_content(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return "";
    return {std::istreambuf_iterator(file), std::istreambuf_iterator<char>()};
}

} // namespace

TEST_CASE("run_async") {
    const std::string input_file = "test_input.txt";
    const std::string output_file = "test_output.txt";
    const std::string error_file = "test_error.txt";
    const std::string content = "Hello, World!";

    CHECK(create_test_file(input_file, content));

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
    const std::vector<std::string> args = {"cat", input_file};
#endif // _WIN32
    Redirect redirect;
    redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);
    redirect.fd_err = open_fd_for_write(error_file).value_or(INVALID_FD);

    const auto proc = run_async(args, redirect, false);
    CHECK(proc.has_value());
    CHECK(proc != INVALID_PROC);
    CHECK(wait_proc(*proc).has_value());

    CHECK(redirect.fd_err != INVALID_FD);
    CHECK(redirect.fd_out != INVALID_FD);
    CHECK(redirect.fd_in == INVALID_FD);

    reset_fd(redirect.fd_out);
    reset_fd(redirect.fd_err);
    CHECK(redirect.fd_out == INVALID_FD);
    CHECK(redirect.fd_err == INVALID_FD);

    CHECK(read_file_content(output_file) == content);

    std::filesystem::remove(input_file);
    std::filesystem::remove(output_file);
    std::filesystem::remove(error_file);
}

TEST_CASE("run_sync") {
    const std::string input_file = "test_input.txt";
    const std::string output_file = "test_output.txt";
    const std::string error_file = "test_error.txt";
    const std::string content = "Hello, World!";

    CHECK(create_test_file(input_file, content));

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
    const std::vector<std::string> args = {"cat", input_file};
#endif // _WIN32
    Redirect redirect;
    redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);
    redirect.fd_err = open_fd_for_write(error_file).value_or(INVALID_FD);

    CHECK(run_sync(args, redirect, false).has_value());

    CHECK(redirect.fd_err != INVALID_FD);
    CHECK(redirect.fd_out != INVALID_FD);
    CHECK(redirect.fd_in == INVALID_FD);

    reset_fd(redirect.fd_out);
    reset_fd(redirect.fd_err);
    CHECK(redirect.fd_out == INVALID_FD);
    CHECK(redirect.fd_err == INVALID_FD);

    CHECK(read_file_content(output_file) == content);

    std::filesystem::remove(input_file);
    std::filesystem::remove(output_file);
    std::filesystem::remove(error_file);
}

TEST_CASE("run_sync with reset") {
    const std::string input_file = "test_input.txt";
    const std::string output_file = "test_output.txt";
    const std::string error_file = "test_error.txt";
    const std::string content = "Hello, World!";

    CHECK(create_test_file(input_file, content));

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
    const std::vector<std::string> args = {"cat", input_file};
#endif // _WIN32
    Redirect redirect;
    redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);
    redirect.fd_err = open_fd_for_write(error_file).value_or(INVALID_FD);

    CHECK(redirect.fd_out != INVALID_FD);
    CHECK(redirect.fd_err != INVALID_FD);
    CHECK(redirect.fd_in == INVALID_FD);

    CHECK(run_sync(args, redirect).has_value());

    CHECK(redirect.fd_out == INVALID_FD);
    CHECK(redirect.fd_err == INVALID_FD);

    CHECK(read_file_content(output_file) == content);

    std::filesystem::remove(input_file);
    std::filesystem::remove(output_file);
    std::filesystem::remove(error_file);
}

TEST_CASE("run_async with reset") {
    const std::string input_file = "test_input.txt";
    const std::string output_file = "test_output.txt";
    const std::string error_file = "test_error.txt";
    const std::string content = "Hello, World!";

    CHECK(create_test_file(input_file, content));

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
    const std::vector<std::string> args = {"cat", input_file};
#endif // _WIN32
    Redirect redirect;
    redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);
    redirect.fd_err = open_fd_for_write(error_file).value_or(INVALID_FD);

    CHECK(redirect.fd_out != INVALID_FD);
    CHECK(redirect.fd_err != INVALID_FD);
    CHECK(redirect.fd_in == INVALID_FD);

    const auto proc = run_async(args, redirect);
    CHECK(proc.has_value());
    CHECK(proc != INVALID_PROC);

    CHECK(wait_proc(*proc).has_value());

    CHECK(redirect.fd_out == INVALID_FD);
    CHECK(redirect.fd_err == INVALID_FD);

    CHECK(read_file_content(output_file) == content);

    std::filesystem::remove(input_file);
    std::filesystem::remove(output_file);
    std::filesystem::remove(error_file);
}

TEST_CASE("multiple asynchronous processes") {
    constexpr int count = 3;
    std::vector<Proc> procs;
    std::vector<std::string> output_files;

    Redirect redirect;
    for (int i = 0; i < count; ++i) {
        std::string output_file = "test_multi_output_";
        output_file.append(std::to_string(i)).append(".txt");
        output_files.push_back(output_file);

        const std::string message = "Process number " + std::to_string(i);
        std::string input_file = "test_multi_input_";
        input_file.append(std::to_string(i)).append(".txt");

        CHECK(create_test_file(input_file, message));

#ifdef _WIN32
        const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
        const std::vector<std::string> args = {"cat", input_file};
#endif // _WIN32
        CHECK(redirect.fd_out == INVALID_FD);
        redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);

        const auto proc = run_async(args, redirect);
        CHECK(proc.has_value());
        CHECK(proc != INVALID_PROC);

        procs.push_back(*proc);
    }

    CHECK(wait_procs(procs).has_value());

    // Check output from every process.
    for (std::size_t i = 0; i < count; ++i) {
        std::string input_file = "test_multi_input_";
        input_file.append(std::to_string(i)).append(".txt");

        CHECK(read_file_content(output_files[i]) == read_file_content(input_file));

        std::filesystem::remove(input_file);
        std::filesystem::remove(output_files[i]);
    }
}

TEST_CASE("error handling with invalid command") {
    const std::vector<std::string> args = {"nonexistent_command"};
    CHECK(!run_sync(args));
}

TEST_CASE("stdin redirection") {
    const std::string input_file = "test_stdin_input.txt";
    const std::string output_file = "test_stdin_output.txt";
    const std::string content = "Hello, World!";

    CHECK(create_test_file(input_file, content));

    Redirect redirect;
    redirect.fd_in = open_fd_for_read(input_file).value_or(INVALID_FD);
    redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);

    CHECK(redirect.fd_in != INVALID_FD);
    CHECK(redirect.fd_out != INVALID_FD);

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "findstr", ".*"};
#else
    const std::vector<std::string> args = {"cat"};
#endif

    CHECK(run_sync(args, redirect).has_value());

    std::string output = read_file_content(output_file);

    if (!output.empty() && output.back() == '\n') {
        output.pop_back();
    }

    CHECK(output == content);
    std::filesystem::remove(input_file);
    std::filesystem::remove(output_file);
}

TEST_CASE("handling spaces in arguments") {
    const std::string input_file = "test input.txt";
    const std::string output_file = "test output.txt";
    const std::string error_file = "test error.txt";
    const std::string content = "Hello, World!";

    CHECK(create_test_file(input_file, content));

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
    const std::vector<std::string> args = {"cat", input_file};
#endif // _WIN32
    Redirect redirect;
    redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);
    redirect.fd_err = open_fd_for_write(error_file).value_or(INVALID_FD);

    CHECK(redirect.fd_out != INVALID_FD);
    CHECK(redirect.fd_err != INVALID_FD);
    CHECK(redirect.fd_in == INVALID_FD);

    const auto proc = run_async(args, redirect);
    CHECK(proc.has_value());
    CHECK(proc != INVALID_PROC);

    CHECK(wait_proc(*proc).has_value());

    CHECK(redirect.fd_out == INVALID_FD);
    CHECK(redirect.fd_err == INVALID_FD);

    CHECK(read_file_content(output_file) == content);

    std::filesystem::remove(input_file);
    std::filesystem::remove(output_file);
    std::filesystem::remove(error_file);
}

TEST_CASE("handling echo command") {
    const std::string expected = "Hello";
    const std::string output_file = "test_spaces_output.txt";

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "echo", expected};
#else
    const std::vector<std::string> args = {"echo", expected};
#endif // _WIN32
    Redirect redirect;
    redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);
    CHECK(redirect.fd_out != INVALID_FD);

    CHECK(run_sync(args, redirect, false).has_value());

    CHECK(redirect.fd_out != INVALID_FD);
    CHECK(redirect.fd_in == INVALID_FD);
    CHECK(redirect.fd_err == INVALID_FD);

    reset_fd(redirect.fd_out);
    CHECK(redirect.fd_out == INVALID_FD);

    std::string output = read_file_content(output_file);

    if (!output.empty() && output.back() == '\n') {
        output.pop_back();
    }

    CHECK(output == expected);
    std::filesystem::remove(output_file);
}

TEST_CASE("handling spaces in echo command") {
    const std::string expected = "Hello with spaces";
    const std::string output_file = "test_spaces_output.txt";

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "echo", expected};
#else
    const std::vector<std::string> args = {"echo", expected};
#endif // _WIN32
    Redirect redirect;
    redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);
    CHECK(redirect.fd_out != INVALID_FD);

    CHECK(run_sync(args, redirect, false).has_value());

    CHECK(redirect.fd_out != INVALID_FD);
    CHECK(redirect.fd_in == INVALID_FD);
    CHECK(redirect.fd_err == INVALID_FD);

    reset_fd(redirect.fd_out);
    CHECK(redirect.fd_out == INVALID_FD);

    std::string output = read_file_content(output_file);

    if (!output.empty() && output.back() == '\n') {
        output.pop_back();
    }

    // Not the best way to handle this, but I assume Windows users
    // will be familiar with how `cmd echo` works. The previous
    // two tests should show if the escaping is correct.
#ifdef _WIN32
    if (!output.empty() && output.front() == '\"' && output.back() == '\"') {
        output.erase(output.begin());
        output.pop_back();
    }
#endif // _WIN32
    CHECK(output == expected);
    std::filesystem::remove(output_file);
}

TEST_CASE("handling special characters in arguments") {
    const std::string expected = "special$chars!*`";
    const std::string output_file = "test_special_output.txt";

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "echo", expected};
#else
    const std::vector<std::string> args = {"echo", expected};
#endif // _WIN32
    Redirect redirect;
    redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);

    CHECK(run_sync(args, redirect).has_value());

    CHECK(redirect.fd_out == INVALID_FD);

    std::string output = read_file_content(output_file);

    if (!output.empty() && output.back() == '\n') {
        output.pop_back();
    }

    CHECK(output == expected);
    std::filesystem::remove(output_file);
}

TEST_CASE("environment variable in argument") {
#ifdef _WIN32
    const std::string expected = "%PATH%";
#else
    const std::string expected = "$HOME";
#endif // _WIN32
    const std::string output_file = "test_env_output.txt";

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "echo", expected};
#else
    const std::vector<std::string> args = {"echo", expected};
#endif // _WIN32
    Redirect redirect;
    redirect.fd_out = open_fd_for_write(output_file).value_or(INVALID_FD);

    CHECK(run_sync(args, redirect).has_value());

    CHECK(redirect.fd_out == INVALID_FD);

    std::string output = read_file_content(output_file);

    if (!output.empty() && output.back() == '\n') {
        output.pop_back();
    }

    CHECK(output == expected);
    std::filesystem::remove(output_file);
}

TEST_CASE("command injection safety")
{
    const std::string safe_file = "safe.txt";

    CHECK(create_test_file(safe_file, "This is a safe file"));

#ifdef _WIN32
    CHECK(run_sync({"cmd", "/c", "echo", "hello", "&", "del", safe_file}).has_value());
#else
    CHECK(run_sync({"echo", "hello; rm " + safe_file}).has_value());
#endif // _WIN32

    CHECK(std::filesystem::exists(safe_file));
    std::filesystem::remove(safe_file);
}

TEST_CASE("open_fd_for_read") {
    const std::string filename = "test_input.txt";
    const std::string content = "Hello, World!";
    CHECK(create_test_file(filename, content));

    Fd fd = open_fd_for_read(filename).value_or(INVALID_FD);
    CHECK(fd != INVALID_FD);

    std::array<char, 128> buffer;
#ifdef _WIN32
    DWORD bytes_read = 0;
    CHECK(ReadFile(fd, buffer.data(), static_cast<DWORD>(buffer.size() - 1), &bytes_read, nullptr));
    buffer[static_cast<std::size_t>(bytes_read)] = '\0';
#else
    const ssize_t bytes_read = read(fd, buffer.data(), buffer.size() - 1);
    CHECK(bytes_read == static_cast<ssize_t>(content.size()));
    buffer[static_cast<std::size_t>(bytes_read)] = '\0';
#endif // _WIN32
    reset_fd(fd);
    CHECK(fd == INVALID_FD);

    CHECK(std::string(buffer.data()) == content);

    std::filesystem::remove(filename);
}

TEST_CASE("open_fd_for_read fail") {
    const std::string filename = "non_existent_file.txt";

    const Fd fd = open_fd_for_read(filename).value_or(INVALID_FD);
    CHECK(fd == INVALID_FD);

#ifdef _WIN32
    const std::string error_message = win32_error_to_string(GetLastError());
    CHECK(error_message == "File not found.");
#else
    const std::string error_message = posix_error_to_string(errno);
    CHECK(error_message == "No such file or directory");
#endif // _WIN32
}

TEST_CASE("open_fd_for_write") {
    const std::string filename = "test_output.txt";
    const std::string content = "Hello, World!";

    {
        Fd fd = open_fd_for_write(filename).value_or(INVALID_FD);
        CHECK(fd != INVALID_FD);

#ifdef _WIN32
        DWORD bytes_written = 0;
        CHECK(WriteFile(fd, content.c_str(), static_cast<DWORD>(content.size()), &bytes_written, nullptr));
        CHECK(bytes_written == content.size());
#else
        CHECK(write(fd, content.c_str(), content.size()) == static_cast<ssize_t>(content.size()));
#endif // _WIN32

        reset_fd(fd);
    }

    CHECK(content == read_file_content(filename));

    std::filesystem::remove(filename);
}

TEST_CASE("error code to string") {
#ifdef _WIN32
    const DWORD error_code = ERROR_FILE_NOT_FOUND;
    const std::string expected = "File not found.";
    const std::string error_message = win32_error_to_string(error_code);

    const DWORD error_code2 = ERROR_ACCESS_DENIED;
    const std::string expected2 = "Access denied.";
    const std::string error_message2 = win32_error_to_string(error_code2);

    const DWORD error_code3 = ERROR_INVALID_PARAMETER;
    const std::string expected3 = "Invalid parameter.";
    const std::string error_message3 = win32_error_to_string(error_code3);
#else
    constexpr int error_code = ENOENT;
    const std::string expected = "No such file or directory";
    const std::string error_message = posix_error_to_string(error_code);

    constexpr int error_code2 = EACCES;
    const std::string expected2 = "Permission denied";
    const std::string error_message2 = posix_error_to_string(error_code2);

    constexpr int error_code3 = EINVAL;
    const std::string expected3 = "Invalid argument";
    const std::string error_message3 = posix_error_to_string(error_code3);
#endif // _WIN32
    CHECK(error_message == expected);
    CHECK(error_message2 == expected2);
    CHECK(error_message3 == expected3);
}

TEST_CASE("creating pipes") {
    Fd read_end = INVALID_FD;
    Fd write_end = INVALID_FD;

    CHECK(create_pipe(read_end, write_end).has_value());
    CHECK(read_end != INVALID_FD);
    CHECK(write_end != INVALID_FD);

    const std::string test_data = "pipe test data";
    std::array<char, 128> buffer;
#ifdef _WIN32
    DWORD bytes_written = 0;
    CHECK(WriteFile(write_end, test_data.data(), static_cast<DWORD>(test_data.size()), &bytes_written, nullptr));
    CHECK(bytes_written == test_data.size());

    DWORD bytes_read = 0;
    CHECK(ReadFile(read_end, buffer.data(), static_cast<DWORD>(buffer.size() - 1), &bytes_read, nullptr));
    buffer[static_cast<std::size_t>(bytes_read)] = '\0';
#else
    CHECK(write(write_end, test_data.data(), test_data.size()) == static_cast<ssize_t>(test_data.size()));
    const ssize_t bytes_read = read(read_end, buffer.data(), buffer.size() - 1);
    CHECK(bytes_read == static_cast<ssize_t>(test_data.size()));
    buffer[static_cast<std::size_t>(bytes_read)] = '\0';
#endif

    CHECK(std::string(buffer.data()) == test_data);

    close_fd(read_end);
    close_fd(write_end);
}
