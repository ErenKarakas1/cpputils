#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"

#include "common.hpp"

TEST_CASE("correct sizes") {
    CHECK(sizeof(u8)  == 1);
    CHECK(sizeof(u16) == 2);
    CHECK(sizeof(u32) == 4);
    CHECK(sizeof(u64) == 8);

    CHECK(sizeof(i8)  == 1);
    CHECK(sizeof(i16) == 2);
    CHECK(sizeof(i32) == 4);
    CHECK(sizeof(i64) == 8);

    CHECK(sizeof(f32) == 4);
    CHECK(sizeof(f64) == 8);
}

TEST_CASE("correct signedness") {
    CHECK(static_cast<i8>(-1) < static_cast<i8>(0));
    CHECK(static_cast<u8>(-1) > static_cast<u8>(0));
}

TEST_CASE("MOVE with custom class") {
    struct MoveTracker {
        bool moved = false;
        MoveTracker() = default;
        ~MoveTracker() = default;
        MoveTracker(const MoveTracker&) = delete;
        MoveTracker& operator=(const MoveTracker&) = delete;
        MoveTracker(MoveTracker&& other) noexcept {
            other.moved = true;
        }
        MoveTracker& operator=(MoveTracker&& other) noexcept {
            other.moved = true;
            return *this;
        }
    };

    MoveTracker original;
    const MoveTracker destination = MOVE(original);
    UNUSED(destination);
    CHECK(original.moved);
}

TEST_CASE("UNUSED") {
    constexpr int x = 42;
    UNUSED(x);
}

#ifndef NDEBUG
TEST_CASE("ASSERT behavior in debug mode") {
    CHECK_NOTHROW(ASSERT(true, "Should not abort"));
    // can't really test these functions
}
#else
TEST_CASE("ASSERT behavior in release mode") {
    CHECK_NOTHROW(ASSERT(false, "Should not do anything"));
}
#endif
