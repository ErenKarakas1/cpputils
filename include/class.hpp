// -------------------------------------------------------------------------------------
//
// docs: https://github.com/ErenKarakas1/cpputils/blob/main/docs/class.md
// src: https://github.com/ErenKarakas1/cpputils
// license: MIT
//
// -------------------------------------------------------------------------------------

#ifndef UTILS_CLASS_HPP
#define UTILS_CLASS_HPP

#define DISABLE_COPY(c)                                                                                                \
private:                                                                                                               \
    c(const c&) = delete;                                                                                              \
    c& operator=(const c&) = delete;

#define DISABLE_MOVE(c)                                                                                                \
private:                                                                                                               \
    c(c&&) = delete;                                                                                                   \
    c& operator=(c&&) = delete;

#define DEFAULT_COPYABLE(c)                                                                                            \
public:                                                                                                                \
    c(const c&) = default;                                                                                             \
    c& operator=(const c&) = default;

#define DEFAULT_MOVABLE(c)                                                                                             \
public:                                                                                                                \
    c(c&&) = default;                                                                                                  \
    c& operator=(c&&) = default;

#endif // UTILS_CLASS_HPP
