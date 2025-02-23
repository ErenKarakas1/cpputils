#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"

#include "string.hpp"

using namespace utils::string;

TEST_CASE("Trimming") {
    std::string str = "  Hello, World!  ";
    CHECK(trim_left(str) == "Hello, World!  ");
    CHECK(str == "  Hello, World!  ");

    CHECK(trim_right(str) == "  Hello, World!");
    CHECK(str == "  Hello, World!  ");

    CHECK(trim(str) == "Hello, World!");
    CHECK(str == "  Hello, World!  ");

    trim_left_in_place(str);
    CHECK(str == "Hello, World!  ");

    trim_right_in_place(str);
    CHECK(str == "Hello, World!");

    std::string str2 = "  Hello, World!  ";
    trim_in_place(str2);
    CHECK(str2 == "Hello, World!");

    std::string str3 = "  Hello,   World!  ";
    CHECK(trim_and_reduce(str3) == "Hello, World!");
    CHECK(str3 == "  Hello,   World!  ");

    trim_and_reduce_in_place(str3);
    CHECK(str3 == "Hello, World!");

    // Edge cases
    std::string str4 = "  ";
    CHECK(trim_left(str4).empty());
    CHECK(trim_right(str4).empty());
    CHECK(trim(str4).empty());
    CHECK(trim_and_reduce(str4).empty());

    trim_left_in_place(str4);
    CHECK(str4.empty());

    std::string str5 = "  ";
    trim_right_in_place(str5);
    CHECK(str5.empty());

    std::string str6 = "  ";
    trim_in_place(str6);
    CHECK(str6.empty());

    std::string str7 = "  ";
    trim_and_reduce_in_place(str7);
    CHECK(str7.empty());

    std::string str8 = "Hello, World!";
    CHECK(trim_left(str8) == str8);
    CHECK(trim_right(str8) == str8);
    CHECK(trim(str8) == str8);
    CHECK(trim_and_reduce(str8) == str8);
    trim_left_in_place(str8);
    CHECK(str8 == "Hello, World!");
    trim_right_in_place(str8);
    CHECK(str8 == "Hello, World!");
    trim_in_place(str8);
    CHECK(str8 == "Hello, World!");
    trim_and_reduce_in_place(str8);
    CHECK(str8 == "Hello, World!");

    std::string str9 = "Hello,    World!";
    trim_and_reduce_in_place(str9);
    CHECK(str9 == "Hello, World!");

    std::string str10 = "Hello,    Wor   ld!    ";
    trim_and_reduce_in_place(str10);
    CHECK(str10 == "Hello, Wor ld!");
}

TEST_CASE("Replacing") {
    std::string str = "Hello, World!";
    CHECK(replace_all(str, "Hello", "Hi") == "Hi, World!");
    CHECK(str == "Hello, World!");

    replace_all_in_place(str, "Hello", "Hi");
    CHECK(str == "Hi, World!");

    std::string str2 = "Hi, World!";
    CHECK(replace_all(str2, "Hi, World!", "Hello, World!") == "Hello, World!");
    CHECK(str2 == "Hi, World!");

    replace_all_in_place(str2, "Hi, World!", "Hello, World!");
    CHECK(str2 == "Hello, World!");

    std::string str3 = "Hello, World!";
    CHECK(replace_all(str3, "Hi", "Hello") == "Hello, World!");
    CHECK(str3 == "Hello, World!");

    replace_all_in_place(str3, "Hi", "Hello");
    CHECK(str3 == "Hello, World!");
}

TEST_CASE("Splitting") {
    const std::string str = "Hello, World!";

    CHECK(split(str, ",") == std::vector<std::string>{"Hello", " World!"});
    CHECK(split(str, "") == std::vector<std::string>{"Hello, World!"});
    CHECK(split(str, " ") == std::vector<std::string>{"Hello,", "World!"});
    CHECK(split(str, "o") == std::vector<std::string>{"Hell", ", W", "rld!"});

    CHECK(split(str, "Hello, World!") == std::vector<std::string>{});
    CHECK(split(str, "Hello, World! ") == std::vector<std::string>{"Hello, World!"});

    const std::string str2 = "aaa,AAA,bbb,BBB,ccc,CCC";
    CHECK(split(str2, ",") == std::vector<std::string>{"aaa", "AAA", "bbb", "BBB", "ccc", "CCC"});
    CHECK(split(str2, "a") == std::vector<std::string>{",AAA,bbb,BBB,ccc,CCC"});
    CHECK(split(str2, "A") == std::vector<std::string>{"aaa,", ",bbb,BBB,ccc,CCC"});
    CHECK(split(str2, "C") == std::vector<std::string>{"aaa,AAA,bbb,BBB,ccc,"});
    CHECK(split(str2, "D") == std::vector<std::string>{"aaa,AAA,bbb,BBB,ccc,CCC"});

    const std::string str3 = "aaaaBaaaBBBaaCaa";
    CHECK(split(str3, "a") == std::vector<std::string>{"B", "BBB", "C"});
    CHECK(split(str3, "B") == std::vector<std::string>{"aaaa", "aaa", "aaCaa"});
    CHECK(split(str3, "aa") == std::vector<std::string>{"B", "aBBB", "C"});
    CHECK(split(str3, "aaa") == std::vector<std::string>{"aB", "BBBaaCaa"});

    // Is this unexpected behavior?
    const std::string str4;
    CHECK(split(str4, ",") == std::vector<std::string>{});
    CHECK(split(str4, " ") == std::vector<std::string>{});
    CHECK(split(str4, "") == std::vector<std::string>{""});

    const std::string str5 = "aaaaaaaaaaaaaaaaaaaaa"; // 21
    CHECK(split(str5, "a") == std::vector<std::string>{});
    CHECK(split(str5, " ") == std::vector<std::string>{"aaaaaaaaaaaaaaaaaaaaa"});
    CHECK(split(str5, "aaa") == std::vector<std::string>{});
    CHECK(split(str5, "aaaaaaaaaaaaaaaaaaaaa") == std::vector<std::string>{});
    CHECK(split(str5, "aaaaa") == std::vector<std::string>{"a"});
}

constexpr std::string_view sv = "Hello, World!";
constexpr std::string_view sv2 = "Hello, World!";
constexpr std::string_view sv3 = "Hello, World!";
constexpr std::string_view sv4;

constexpr auto cv = "Hello, World!";
constexpr auto cv2 = "Hello, World!";
constexpr auto cv3 = "Hello, World!";
constexpr auto cv4 = "";

TEST_CASE("StrViewBuilder") {
    {
        constexpr auto arr = StrViewBuilder<sv, sv2>::array;
        constexpr auto view = StrViewBuilder<sv, sv2>::view;

        constexpr std::size_t x = 2 * sv.size() + 1;

        CHECK(arr.size() == x);
        CHECK(arr.back() == '\0');
        CHECK(view == "Hello, World!Hello, World!");
        CHECK(view.size() == arr.size() - 1);
        CHECK(view == arr.data());
    }

    {
        constexpr auto arr = StrViewBuilder<sv, sv2, sv3, sv4>::array;
        constexpr auto view = StrViewBuilder<sv, sv2, sv3, sv4>::view;

        constexpr std::size_t x = 3 * sv.size() + 1;

        CHECK(arr.size() == x);
        CHECK(arr.back() == '\0');
        CHECK(view == "Hello, World!Hello, World!Hello, World!");
        CHECK(view.size() == arr.size() - 1);
        CHECK(view == arr.data());
    }

    {
        constexpr auto arr = StrViewBuilder<cv, cv2>::array;
        constexpr auto view = StrViewBuilder<cv, cv2>::view;

        constexpr std::size_t x = 2 * std::char_traits<char>::length(cv) + 1;

        CHECK(arr.size() == x);
        CHECK(arr.back() == '\0');
        CHECK(view == "Hello, World!Hello, World!");
        CHECK(view.size() == arr.size() - 1);
        CHECK(view == arr.data());
    }

    {
        constexpr auto arr = StrViewBuilder<cv, cv2, cv3, cv4>::array;
        constexpr auto view = StrViewBuilder<cv, cv2, cv3, cv4>::view;

        constexpr std::size_t x = 3 * std::char_traits<char>::length(cv) + 1;

        CHECK(arr.size() == x);
        CHECK(arr.back() == '\0');
        CHECK(view == "Hello, World!Hello, World!Hello, World!");
        CHECK(view.size() == arr.size() - 1);
        CHECK(view == arr.data());
    }

    {
        // Strings do not have to be outside the function, but they need static storage duration
        static constexpr std::string str = "Hello, World!";

        constexpr auto arr = StrViewBuilder<sv, cv, sv2, cv2, sv3, cv3, sv4, cv4, str>::array;
        constexpr auto view = StrViewBuilder<sv, cv, sv2, cv2, sv3, cv3, sv4, cv4, str>::view;

        constexpr std::size_t x = 4 * sv.size() + 3 * std::char_traits<char>::length(cv) + 1;

        CHECK(arr.size() == x);
        CHECK(arr.back() == '\0');
        CHECK(view == "Hello, World!Hello, World!Hello, World!Hello, World!Hello, World!Hello, World!Hello, World!");
        CHECK(view.size() == arr.size() - 1);
        CHECK(view == arr.data());
    }

    {
        constexpr auto arr = StrViewBuilder<sv4, cv4>::array;
        constexpr auto view = StrViewBuilder<sv4, cv4>::view;

        constexpr std::size_t x = 1;

        CHECK(arr.size() == x);
        CHECK(arr.back() == '\0');
        CHECK(view == "");
        CHECK(view.size() == arr.size() - 1);
        CHECK(view == arr.data());
    }
}
