// -------------------------------------------------------------------------------------
//
// docs: https://github.com/ErenKarakas1/cpputils/blob/main/docs/process.md
// src: https://github.com/ErenKarakas1/cpputils
// license: MIT
//
// -------------------------------------------------------------------------------------

#ifndef UTILS_PROCESS_HPP
#define UTILS_PROCESS_HPP

#include <array>
#include <cerrno>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <cctype>
#include <string_view>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#endif // _WIN32

namespace utils::process {

#ifdef _WIN32
using Proc = HANDLE;
using Fd = HANDLE;
inline const Proc INVALID_PROC = INVALID_HANDLE_VALUE;
inline const Fd INVALID_FD = INVALID_HANDLE_VALUE;
#else
using Proc = pid_t;
using Fd = int;
inline constexpr Proc INVALID_PROC = -1;
inline constexpr Fd INVALID_FD = -1;
#endif // _WIN32

inline void reset_fd(Fd& fd) noexcept;
inline void close_fd(Fd fd) noexcept;
inline bool wait_proc(Proc proc);

class ScopedFd {
public:
    explicit ScopedFd(Fd& fd) : fd_ref(fd) {}

    ~ScopedFd() {
        reset_fd(fd_ref);
    }

    ScopedFd(const ScopedFd&) = delete;
    ScopedFd& operator=(const ScopedFd&) = delete;

    ScopedFd(ScopedFd&& other) noexcept : fd_ref(other.fd_ref) {
        other.fd_ref = INVALID_FD;
    }
    ScopedFd& operator=(ScopedFd&& other) noexcept {
        if (this != &other) {
            reset_fd(fd_ref);
            fd_ref = other.fd_ref;
            other.fd_ref = INVALID_FD;
        }
        return *this;
    }

private:
    Fd& fd_ref;
};

struct Redirect {
    Fd fd_in  = INVALID_FD;
    Fd fd_out = INVALID_FD;
    Fd fd_err = INVALID_FD;
};

namespace detail {
#ifdef _WIN32
// The following three functions are used to escape arguments before passing them to cmd.exe. Code is based on:
// https://learn.microsoft.com/en-us/archive/blogs/twistylittlepassagesallalike/everyone-quotes-command-line-arguments-the-wrong-way
constexpr std::string argv_quote(const std::string& argument) {
    constexpr std::string_view whitespace_chars = " \t\n\v\"";
    if (!argument.empty() && argument.find_first_of(whitespace_chars) == std::string::npos) {
        return argument;
    }

    std::string result;
    result.push_back('"');
    for (auto it = argument.begin();; ++it) {
        unsigned int num_backslashes = 0;

        while (it != argument.end() && *it == '\\') {
            ++num_backslashes;
            ++it;
        }

        if (it == argument.end()) {
            result.append(num_backslashes * 2, '\\');
            break;
        }
        if (*it == '"') {
            result.append(num_backslashes * 2 + 1, '\\');
            result.push_back(*it);
        } else {
            result.append(num_backslashes, '\\');
            result.push_back(*it);
        }
    }

    result.push_back('"');
    return result;
}

constexpr std::string cmd_escape(const std::string& text) {
    constexpr std::string_view meta_chars = "()%!^\"<>&|";
    std::string result;
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (meta_chars.find(text[i]) != std::string::npos) result.push_back('^');
        result.push_back(text[i]);
    }
    return result;
}

constexpr std::string build_cmdline(const std::vector<std::string>& args) {
    std::string cmd_line;
    cmd_line.reserve(128);
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (i > 0) cmd_line.push_back(' ');
        std::string quoted = cmd_escape(argv_quote(args[i]));
        cmd_line.append(quoted);
    }
    return cmd_line;
}
#else
constexpr std::vector<char*> build_cmdline(const std::vector<std::string>& src) {
    std::vector<char*> argv;
    argv.reserve(src.size() + 1); // do NOT inline this, we do not want default initialization
    for (const auto& arg : src) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);
    return argv;
}
#endif // _WIN32
} // namespace detail

#ifdef _WIN32
inline std::string win32_error_to_string(DWORD error_code) noexcept {
    constexpr int WIN32_ERR_MESSAGE_SIZE = 4096;
    std::array<char, WIN32_ERR_MESSAGE_SIZE> error_message = {};

    DWORD error_msg_size =
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_code,
                       LANG_USER_DEFAULT, error_message.data(), WIN32_ERR_MESSAGE_SIZE, nullptr);

    if (error_msg_size == 0) {
        if (GetLastError() != ERROR_MR_MID_NOT_FOUND) {
            return "Could not get error message for error code " + std::to_string(error_code);
        }
        return "Unknown error code " + std::to_string(error_code);
    }

    while (error_msg_size > 0 && std::isspace(static_cast<unsigned char>(error_message[error_msg_size - 1]))) {
        error_message[--error_msg_size] = '\0';
    }

    return std::string(error_message.data(), error_msg_size);
}
#else
inline std::string posix_error_to_string(int error_code) noexcept {
    std::array<char, 256> error_message = {};
    // Source: https://linux.die.net/man/3/strerror_r
#if ((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE)
    if (strerror_r(error_code, error_message.data(), error_message.size()) != 0) {
        return "Unknown error code " + std::to_string(error_code);
    }
    return error_message;
#else
    return strerror_r(error_code, error_message.data(), error_message.size());
#endif // _POSIX_C_SOURCE
}
#endif // _WIN32

// Most implementations in this header are either inspired by or
// directly taken from https://github.com/tsoding/nob.h/blob/main/nob.h

inline Proc run_async(const std::vector<std::string>& args, const Redirect& redirect = {}) {
    if (args.empty()) return INVALID_PROC;

#ifdef _WIN32
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    if (redirect.fd_in != INVALID_FD || redirect.fd_out != INVALID_FD || redirect.fd_err != INVALID_FD) {
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdInput = redirect.fd_in != INVALID_FD ? redirect.fd_in : GetStdHandle(STD_INPUT_HANDLE);
        si.hStdOutput = redirect.fd_out != INVALID_FD ? redirect.fd_out : GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdError = redirect.fd_err != INVALID_FD ? redirect.fd_err : GetStdHandle(STD_ERROR_HANDLE);
    }

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // Build command string
    const std::string command_line = detail::build_cmdline(args);
    if (command_line.empty()) {
        std::cerr << "Command line is empty\n";
        return INVALID_PROC;
    }

    char* cmd_buf = const_cast<char*>(command_line.c_str());
    if (!CreateProcessA(nullptr, cmd_buf, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "CreateProcess failed: " << win32_error_to_string(GetLastError()) << '\n';
        return INVALID_PROC;
    }

    // Close the thread handle but keep the process handle
    CloseHandle(pi.hThread);
    return pi.hProcess;

#else
    const pid_t pid = fork();

    if (pid == -1) {
        // Fork failed
        std::cerr << "Could not fork: " << posix_error_to_string(errno) << '\n';
        return INVALID_PROC;
    }

    if (pid == 0) {
        // In child process
        constexpr auto redirect_fd = [](const Fd src, const int dest, const std::string_view stream_name) {
            if (src == INVALID_FD) return;
            if (dup2(src, dest) < 0) {
                std::cerr << "Could not redirect " << stream_name << ": " << posix_error_to_string(errno) << '\n';
                _exit(EXIT_FAILURE);
            }
            close_fd(src);
        };

        redirect_fd(redirect.fd_in, STDIN_FILENO, "stdin");
        redirect_fd(redirect.fd_out, STDOUT_FILENO, "stdout");
        redirect_fd(redirect.fd_err, STDERR_FILENO, "stderr");

        const std::vector<char*> argv = detail::build_cmdline(args);
        if (execvp(argv[0], argv.data()) < 0) {
            std::cerr << "Could not exec '" << argv[0] << "': " << posix_error_to_string(errno) << '\n';
            _exit(EXIT_FAILURE);
        }

        assert(false && "execvp should not return");
        _exit(EXIT_FAILURE);
    }

    return pid;
#endif // _WIN32
}

inline Proc run_async_and_reset(const std::vector<std::string>& args, Redirect& redirect) {
    const ScopedFd in_guard(redirect.fd_in);
    const ScopedFd out_guard(redirect.fd_out);
    const ScopedFd err_guard(redirect.fd_err);
    return run_async(args, redirect);
}

inline bool run_sync(const std::vector<std::string>& args, const Redirect& redirect = {}) {
    const Proc proc = run_async(args, redirect);
    if (proc == INVALID_PROC) return false;
    return wait_proc(proc);
}

inline bool run_sync_and_reset(const std::vector<std::string>& args, Redirect& redirect) {
    const Proc proc = run_async_and_reset(args, redirect);
    if (proc == INVALID_PROC) return false;
    return wait_proc(proc);
}

inline bool wait_proc(Proc proc) {
    if (proc == INVALID_PROC) return false;

#ifdef _WIN32
    DWORD result = WaitForSingleObject(proc, INFINITE);

    if (result == WAIT_FAILED) {
        std::cerr << "Could not wait on child process: " << win32_error_to_string(GetLastError()) << '\n';
        return false;
    }

    DWORD exit_status;
    if (!GetExitCodeProcess(proc, &exit_status)) {
        std::cerr << "Could not get exit code: " << win32_error_to_string(GetLastError()) << '\n';
        return false;
    }

    if (exit_status != 0) {
        std::cerr << "Child process exited with error code: " << exit_status << '\n';
        return false;
    }

    CloseHandle(proc);

    return true;
#else
    for (;;) {
        int wstatus = 0;
        if (waitpid(proc, &wstatus, 0) < 0) {
            std::cerr << "Could not wait on child process: " << posix_error_to_string(errno) << '\n';
            return false;
        }

        if (WIFEXITED(wstatus)) {
            const int exit_status = WEXITSTATUS(wstatus);
            if (exit_status != 0) {
                std::cerr << "Child process exited with error code: " << exit_status << '\n';
                return false;
            }
            break;
        }
        if (WIFSIGNALED(wstatus)) {
            std::cerr << "Child process terminated by signal: " << WTERMSIG(wstatus) << '\n';
            return false;
        }
    }

    return true;
#endif // _WIN32
}

inline bool wait_procs(const std::vector<Proc>& procs) {
    bool result = true;
    for (const Proc& proc : procs) {
        result = wait_proc(proc) && result;
    }
    return result;
}

inline Fd open_fd_for_read(const std::string& filename) {
#ifdef _WIN32
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    Fd fd = CreateFileA(filename.c_str(), GENERIC_READ, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);
    if (fd == INVALID_FD) {
        std::cerr << "Could not open file '" << filename << "' for reading: " << win32_error_to_string(GetLastError()) << '\n';
        return INVALID_FD;
    }
    return fd;
#else
    Fd fd = open(filename.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd == INVALID_FD) {
        std::cerr << "Could not open file '" << filename << "' for reading: " << posix_error_to_string(errno) << '\n';
        return INVALID_FD;
    }
    return fd;
#endif // _WIN32
}

inline Fd open_fd_for_write(const std::string& filename) {
#ifdef _WIN32
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    Fd fd = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fd == INVALID_FD) {
        std::cerr << "Could not open file for writing: " << win32_error_to_string(GetLastError()) << '\n';
        return INVALID_FD;
    }
    return fd;
#else
    Fd fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
    if (fd == INVALID_FD) {
        std::cerr << "Could not open file for writing: " << posix_error_to_string(errno) << '\n';
        return INVALID_FD;
    }
    return fd;
#endif // _WIN32
}

inline void close_fd(Fd fd) noexcept {
    if (fd == INVALID_FD) return;
#ifdef _WIN32
    CloseHandle(fd);
#else
    close(fd);
#endif // _WIN32
}

inline void reset_fd(Fd& fd) noexcept {
    if (fd == INVALID_FD) return;
    close_fd(fd);
    fd = INVALID_FD;
}

inline bool create_pipe(Fd& read_end, Fd& write_end) {
#ifdef _WIN32
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    if (!CreatePipe(&read_end, &write_end, &sa, 0)) {
        std::cerr << "Could not create pipe: " << win32_error_to_string(GetLastError()) << '\n';
        return false;
    }
#else
    std::array<Fd, 2> fds;
    if (pipe2(fds.data(), O_CLOEXEC) < 0) {
        std::cerr << "Could not create pipe: " << posix_error_to_string(errno) << '\n';
        return false;
    }
    read_end = fds[0];
    write_end = fds[1];
#endif // _WIN32
    return true;
}

} // namespace utils::process

#endif // UTILS_PROCESS_HPP
