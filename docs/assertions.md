# utils::assertions

Provides common assertion and debugging utilities.

## Usage

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

