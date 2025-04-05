// -------------------------------------------------------------------------------------
//
// docs: https://github.com/ErenKarakas1/cpputils/blob/main/docs/process.md
// src: https://github.com/ErenKarakas1/cpputils
// license: MIT
//
// -------------------------------------------------------------------------------------

#ifndef UTILS_PROCESS_HPP
#define UTILS_PROCESS_HPP

#include <cerrno>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
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

struct Redirect {
    Fd fdin = INVALID_FD;
    Fd fdout = INVALID_FD;
    Fd fderr = INVALID_FD;
};

constexpr void close_fd(Fd fd);
constexpr bool wait_proc(Proc proc);

#ifdef _WIN32
namespace detail {

inline std::string win32_error_to_string(DWORD error_code) {
    constexpr int WIN32_ERR_MESSAGE_SIZE = 4096;

    static char error_message[WIN32_ERR_MESSAGE_SIZE];
    ZeroMemory(error_message, sizeof(error_message));

    DWORD error_msg_size =
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_code,
                       LANG_USER_DEFAULT, error_message, WIN32_ERR_MESSAGE_SIZE, nullptr);

    if (error_msg_size == 0) {
        if (GetLastError() != ERROR_MR_MID_NOT_FOUND) {
            return "Could not get error message for error code " + std::to_string(error_code);
        }
        return "Unknown error code " + std::to_string(error_code);
    }

    while (error_msg_size > 0 && std::isspace(static_cast<unsigned char>(error_message[error_msg_size - 1]))) {
        error_message[--error_msg_size] = '\0';
    }

    return std::string(error_message, error_msg_size);
}

// The following three functions are used to escape arguments before passing them to cmd.exe. Code is based on:
// https://learn.microsoft.com/en-us/archive/blogs/twistylittlepassagesallalike/everyone-quotes-command-line-arguments-the-wrong-way
constexpr std::string argv_quote(const std::string& argument) {
    if (!argument.empty() && argument.find_first_of(" \t\n\v\"") == std::string::npos) {
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
    const std::string meta_chars = "()%!^\"<>&|";
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

} // namespace detail
#endif // _WIN32

// Most implementations in this header are either inspired by or
// directly taken from https://github.com/tsoding/nob.h/blob/main/nob.h

constexpr Proc run_async(const std::vector<std::string>& args, const Redirect& redirect = {}) {
    if (args.empty()) return INVALID_PROC;

#ifdef _WIN32
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    if (redirect.fdin != INVALID_FD || redirect.fdout != INVALID_FD || redirect.fderr != INVALID_FD) {
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdInput = redirect.fdin != INVALID_FD ? redirect.fdin : GetStdHandle(STD_INPUT_HANDLE);
        si.hStdOutput = redirect.fdout != INVALID_FD ? redirect.fdout : GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdError = redirect.fderr != INVALID_FD ? redirect.fderr : GetStdHandle(STD_ERROR_HANDLE);
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
        std::cerr << "CreateProcess failed: " << detail::win32_error_to_string(GetLastError()) << '\n';
        return INVALID_PROC;
    }

    // Close the thread handle, but keep the process handle
    CloseHandle(pi.hThread);
    return pi.hProcess;

#else
    const pid_t pid = fork();

    if (pid == -1) {
        // Fork failed
        std::cerr << "Could not fork: " << std::strerror(errno) << '\n';
        return INVALID_PROC;
    }

    if (pid == 0) {
        if (redirect.fdin != INVALID_FD) {
            if (dup2(redirect.fdin, STDIN_FILENO) < 0) {
                std::cerr << "Could not redirect stdin: " << std::strerror(errno) << '\n';
                // _exit as we are in the child process
                _exit(1);
            }
        }
        if (redirect.fdout != INVALID_FD) {
            if (dup2(redirect.fdout, STDOUT_FILENO) < 0) {
                std::cerr << "Could not redirect stdout: " << std::strerror(errno) << '\n';
                _exit(1);
            }
        }
        if (redirect.fderr != INVALID_FD) {
            if (dup2(redirect.fderr, STDERR_FILENO) < 0) {
                std::cerr << "Could not redirect stderr: " << std::strerror(errno) << '\n';
                _exit(1);
            }
        }

        // Convert arguments to C-style array
        std::vector<char*> argv;
        argv.reserve(args.size() + 1); // do NOT inline this, we do not want default initialization
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr); // Null-terminate the array

        if (execvp(argv[0], argv.data()) < 0) {
            std::cerr << "Could not exec child process: " << std::strerror(errno) << '\n';
            exit(1);
        }

        // If we get here, execvp failed
        std::cerr << "Could not exec child process: " << std::strerror(errno) << '\n';
        _exit(1);
    }

    return pid;
#endif // _WIN32
}

constexpr Proc run_async_and_reset(const std::vector<std::string>& args, Redirect& redirect) {
    Proc proc = run_async(args, redirect);

    if (redirect.fdin != INVALID_FD) {
        close_fd(redirect.fdin);
        redirect.fdin = INVALID_FD;
    }
    if (redirect.fdout != INVALID_FD) {
        close_fd(redirect.fdout);
        redirect.fdout = INVALID_FD;
    }
    if (redirect.fderr != INVALID_FD) {
        close_fd(redirect.fderr);
        redirect.fderr = INVALID_FD;
    }

    return proc;
}

constexpr bool run_sync(const std::vector<std::string>& args, const Redirect& redirect = {}) {
    const Proc proc = run_async(args, redirect);
    if (proc == INVALID_PROC) return false;
    return wait_proc(proc);
}

constexpr bool run_sync_and_reset(const std::vector<std::string>& args, Redirect& redirect) {
    const Proc proc = run_async_and_reset(args, redirect);
    if (proc == INVALID_PROC) return false;
    return wait_proc(proc);
}

constexpr bool wait_proc(Proc proc) {
    if (proc == INVALID_PROC) return false;

#ifdef _WIN32
    DWORD result = WaitForSingleObject(proc, INFINITE);

    if (result == WAIT_FAILED) {
        std::cerr << "Could not wait on child process: " << detail::win32_error_to_string(GetLastError()) << '\n';
        return false;
    }

    DWORD exit_status;
    if (!GetExitCodeProcess(proc, &exit_status)) {
        std::cerr << "Could not get exit code: " << detail::win32_error_to_string(GetLastError()) << '\n';
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
            std::cerr << "Could not wait on child process: " << std::strerror(errno) << '\n';
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

constexpr bool wait_procs(const std::vector<Proc>& procs) {
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
        std::cerr << "Could not open file for reading: " << detail::win32_error_to_string(GetLastError()) << '\n';
        return INVALID_FD;
    }
    return fd;
#else
    Fd fd = open(filename.c_str(), O_RDONLY);
    if (fd == INVALID_FD) {
        std::cerr << "Could not open file for reading: " << std::strerror(errno) << '\n';
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
        std::cerr << "Could not open file for writing: " << detail::win32_error_to_string(GetLastError()) << '\n';
        return INVALID_FD;
    }
    return fd;
#else
    Fd fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == INVALID_FD) {
        std::cerr << "Could not open file for writing: " << std::strerror(errno) << '\n';
        return INVALID_FD;
    }
    return fd;
#endif // _WIN32
}

constexpr void close_fd(Fd fd) {
    if (fd == INVALID_FD) return;
#ifdef _WIN32
    CloseHandle(fd);
#else
    close(fd);
#endif // _WIN32
}

} // namespace utils::process

#endif // UTILS_PROCESS_HPP
