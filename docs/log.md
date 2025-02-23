# utils::log

Provides a simple interface for logging and debugging.

## Usage

---

You have to use 
```utils::log::detail::logger::instance().set_log_level(LogLevel::X);```
to change log levels for now. This will change in the future.

### Logging
```c++
DEBUG("This is a debug message");
INFO("This is an {} message", "info");
WARNING("This is {} warning {}", "a", "message");
ERROR("This is the {}th message", 4);

// Output:
// [DEBUG] [<file>:<line>] This is a debug message
// [INFO] This is an info message
// [WARNING] This is a warning message
// [ERROR] This is the 4th message
```

#### Colored output
```c++
#define LOG_COLOR
#include "utils/log.hpp"
```

### Debugging
```c++
int x = 10;
UNUSED(x) // silences unused variable warning
TODO("Not implemented yet");
ASSERT(x != 10, "x is not 10");
UNREACHABLE("This should never happen"); // note that reaching this line is UB

// Output:
// TODO at [<file>:<line>]: Not implemented yet
// ASSERT failed at [<file>:<line>]: x is not 10
// Unreachable code reached at [<file>:<line>]: This should never happen
```
