#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"

#include "assertions.hpp"

TEST_CASE("UNUSED") {
    constexpr int x = 42;
    UNUSED(x);
}

#ifdef NDEBUG
TEST_CASE("NDEBUG") {
    TODO("This should not abort");
    ASSERT(false, "This should not abort");
    UNREACHABLE("This should not trigger");
}
#endif
