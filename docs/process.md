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
redirect.fdout = open_fd_for_write(output_file);
redirect.fderr = open_fd_for_write(error_file);

const auto proc = run_async(args, redirect);
if (!wait_proc(proc)) return 1;

close_fd(redirect.fdout);
close_fd(redirect.fderr);
redirect.fdout = INVALID_FD;
redirect.fderr = INVALID_FD;
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
redirect.fdout = open_fd_for_write(output_file);
redirect.fderr = open_fd_for_write(error_file);

const auto proc = run_async_and_reset(args, redirect);
if (!wait_proc(proc)) return 1;

// No need to close the file descriptors manually
```

### Running multiple asynchronous commands
```c++
using namespace utils::process;

constexpr int count = 3;
std::vector<Proc> procs;
std::vector<std::string> output_files;

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
    Redirect redirect;
    redirect.fdout = open_fd_for_write(output_file);

    const Proc proc = run_async(args, redirect);

    close_fd(redirect.fdout);
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
redirect.fdout = open_fd_for_write(output_file);
redirect.fderr = open_fd_for_write(error_file);

if (!run_sync(args, redirect)) return 1;

close_fd(redirect.fdout);
close_fd(redirect.fderr);
redirect.fdout = INVALID_FD;
redirect.fderr = INVALID_FD;
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
redirect.fdout = open_fd_for_write(output_file);
redirect.fderr = open_fd_for_write(error_file);

if (!run_sync_and_reset(args, redirect)) return 1;

// No need to close the file descriptors manually
```

### Definitions
```c++
// Windows
using Proc = HANDLE;
using Fd = HANDLE;
inline const Proc INVALID_PROC = INVALID_HANDLE_VALUE;
inline const Fd INVALID_FD = INVALID_HANDLE_VALUE;

// POSIX
using Proc = pid_t;
using Fd = int;
inline constexpr Proc INVALID_PROC = -1;
inline constexpr Fd INVALID_FD = -1;

struct Redirect {
    Fd fdin = INVALID_FD;
    Fd fdout = INVALID_FD;
    Fd fderr = INVALID_FD;
};
```

### Functions

#### Running processes
```c++
Proc run_async(const std::vector<std::string>& args, const Redirect& redirect = {});
Proc run_async_and_reset(const std::vector<std::string>& args, Redirect& redirect);
bool run_sync(const std::vector<std::string>& args, const Redirect& redirect = {});
bool run_sync_and_reset(const std::vector<std::string>& args, Redirect& redirect);
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
void close_fd(Fd fd);
```
