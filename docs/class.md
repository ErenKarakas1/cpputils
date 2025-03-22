# utils::class

Provides convenience macros that define default copy/move operations and
delete copy/move operations.

## Usage

### Define default copy/move

```c++
class MyClass {
public:
    MyClass() ...
    DEFAULT_COPYABLE(MyClass);
    DEFAULT_MOVABLE(MyClass);
};
```

### Delete copy/move

```c++
class MyClass {
public:
    MyClass() ...

private:
    DISABLE_COPY(MyClass);
    DISABLE_MOVE(MyClass);
};
```
