# utils::process

Provides cross-platform functions for running processes and managing file descriptors.

## Usage

### Running asynchronous commands
```c++
using namespace utils::process;

const std::string input_file = "test_input.txt";
const std::string output_file = "test_output.txt";
const std::string error_file = "test_error.txt";

#ifdef _WIN32
const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
const std::vector<std::string> args = {"cat", input_file};
#endif
Redirect redirect;
redirect.fd_out = open_fd_for_write(output_file);
redirect.fd_err = open_fd_for_write(error_file);

const auto proc = run_async(args, redirect, false);
if (!wait_proc(proc)) return 1;

reset_redirect(redirect);
```

### Running asynchronous commands with automatic cleanup
```c++
using namespace utils::process;

const std::string input_file = "test_input.txt";
const std::string output_file = "test_output.txt";
const std::string error_file = "test_error.txt";

#ifdef _WIN32
const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
const std::vector<std::string> args = {"cat", input_file};
#endif
Redirect redirect;
redirect.fd_out = open_fd_for_write(output_file);
redirect.fd_err = open_fd_for_write(error_file);

const auto proc = run_async(args, redirect);
if (!wait_proc(proc)) return 1;

// No need to close the file descriptors manually
```

### Running multiple asynchronous commands
```c++
using namespace utils::process;

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

#ifdef _WIN32
    const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
    const std::vector<std::string> args = {"cat", input_file};
#endif
    redirect.fd_out = open_fd_for_write(output_file);

    const Proc proc = run_async(args, redirect);

    procs.push_back(proc);
}

if (!wait_procs(procs)) return 1;
```

### Running synchronous commands
```c++
using namespace utils::process;

const std::string input_file = "test_input.txt";
const std::string output_file = "test_output.txt";
const std::string error_file = "test_error.txt";

#ifdef _WIN32
const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
const std::vector<std::string> args = {"cat", input_file};
#endif
Redirect redirect;
redirect.fd_out = open_fd_for_write(output_file);
redirect.fd_err = open_fd_for_write(error_file);

if (!run_sync(args, redirect)) return 1;

reset_redirect(redirect);
```

### Running synchronous commands with automatic cleanup
```c++
using namespace utils::process;

const std::string input_file = "test_input.txt";
const std::string output_file = "test_output.txt";
const std::string error_file = "test_error.txt";

#ifdef _WIN32
const std::vector<std::string> args = {"cmd", "/c", "type", input_file};
#else
const std::vector<std::string> args = {"cat", input_file};
#endif
Redirect redirect;
redirect.fd_out = open_fd_for_write(output_file);
redirect.fd_err = open_fd_for_write(error_file);

if (!run_sync(args, redirect)) return 1;

// No need to close the file descriptors manually
```

### Creating pipes
```c++
using namespace utils::process;

Fd read_end = INVALID_FD;
Fd write_end = INVALID_FD;

if (!create_pipe(read_end, write_end)) return 1; // it will report the error

// Use read_end and write_end as needed
// Don't forget to close the file descriptors when done
close_fd(read_end);
close_fd(write_end);
```

### Definitions
```c++
// Windows
using Proc = void*; // HANDLE
using Fd   = void*;
inline const Proc INVALID_PROC = std::bit_cast<Proc>(static_cast<std::intptr_t>(-1)); // INVALID_HANDLE_VALUE
inline const Fd   INVALID_FD   = std::bit_cast<Fd>(static_cast<std::intptr_t>(-1));

// POSIX
using Proc = pid_t;
using Fd   = int;
inline constexpr Proc INVALID_PROC = -1;
inline constexpr Fd   INVALID_FD   = -1;

struct Redirect {
    Fd fd_in  = INVALID_FD;
    Fd fd_out = INVALID_FD;
    Fd fd_err = INVALID_FD;
};
```

### Functions

#### Running processes
```c++
Proc run_async(const std::vector<std::string>& args);
Proc run_async(const std::vector<std::string>& args, const Redirect& redirect, bool reset_fds = true);
bool run_sync(const std::vector<std::string>& args);
bool run_sync(const std::vector<std::string>& args, const Redirect& redirect, bool reset_fds = true);
```

#### Waiting for processes
```c++
bool wait_proc(Proc proc);
bool wait_procs(const std::vector<Proc>& procs);
```

#### Managing file descriptors
```c++
Fd open_fd_for_read(const std::string& filename);
Fd open_fd_for_write(const std::string& filename);
void close_fd(Fd fd) noexcept;
void reset_fd(Fd& fd) noexcept;
void reset_redirect(Redirect& redirect) noexcept;
```

#### Creating pipes
```c++
bool create_pipe(Fd& read_end, Fd& write_end);
```

#### Error reporting
```c++
std::string win32_error_to_string(unsigned long error_code) noexcept;
std::string posix_error_to_string(int error_code) noexcept;
```
