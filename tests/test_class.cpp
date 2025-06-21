#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "class.hpp"

class TestClass {
public:
    TestClass() : data{1, 2, 3, 4, 5} {}
    ~TestClass() = default;

private:
    DISABLE_COPY(TestClass);
    DISABLE_MOVE(TestClass);
    std::vector<int> data;
};

class TestClass2 {
public:
    TestClass2() : data{"Hello, World!"} {}
    ~TestClass2() = default;
    DEFAULT_COPYABLE(TestClass2);
    DEFAULT_MOVABLE(TestClass2);

private:
    std::string data;
};

TEST_CASE("DISABLE_COPY") {
    const TestClass a;
    (void)a;
    // TestClass b(a); // This should not compile
    // TestClass c = a; // This should not compile
}

TEST_CASE("DISABLE_MOVE") {
    const TestClass a;
    (void)a;
    // TestClass b(std::move(a)); // This should not compile
    // TestClass c = std::move(a); // This should not compile
}

TEST_CASE("DEFAULT_COPYABLE") {
    const TestClass2 a;
    const TestClass2 b(a);
    const TestClass2 c = a;
    (void)b;
    (void)c;
}

TEST_CASE("DEFAULT_MOVABLE") {
    TestClass2 a;
    TestClass2 b(std::move(a));
    const TestClass2 c = std::move(b);
    (void)b;
    (void)c;
}
