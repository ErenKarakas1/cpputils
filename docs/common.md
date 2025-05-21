# utils::common

Provides common type definitions and utilities.

## Usage

### Type definitions

```c++
using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;
```

### Assertions

```c++
int x = 10;
UNUSED(x) // silences unused variable warning
TODO("Not implemented yet");
ASSERT(x != 10, "x is not 10");
UNREACHABLE("This should never happen"); // note that reaching this line is UB

// Output:
// TODO at [<file>:<line>]: Not implemented yet
// ASSERT failed at [<file>:<line>]: x is not 10
```

### MOVE and FORWARD

```c++
#define MOVE(...) // std::move(...) equivalent
#define FORWARD(...) // std::forward(...) equivalent
```
