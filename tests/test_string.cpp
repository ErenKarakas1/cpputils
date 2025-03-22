#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"

#include "string.hpp"

using namespace utils::string;

TEST_CASE("ASCII checks") {
    CHECK(!ascii::is_space('h'));
    CHECK(ascii::is_space(' '));
    CHECK(ascii::is_space('\t'));
    CHECK(ascii::is_space('\n'));
    CHECK(ascii::is_space('\r'));

    constexpr unsigned char c = 'a';
    CHECK(ascii::is_alpha(c));
    CHECK(ascii::is_alnum(c));
    CHECK(!ascii::is_digit(c));
    CHECK(!ascii::is_space(c));

    constexpr char c2 = '0';
    CHECK(!ascii::is_alpha(c2));
    CHECK(ascii::is_alnum(c2));
    CHECK(ascii::is_digit(c2));
    CHECK(!ascii::is_space(c2));

    constexpr unsigned char c3 = 'z';
    CHECK(ascii::is_alpha(c3));
    CHECK(ascii::is_alnum(c3));
    CHECK(!ascii::is_digit(c3));
    CHECK(!ascii::is_space(c3));

    constexpr char c4 = '9';
    CHECK(!ascii::is_alpha(c4));
    CHECK(ascii::is_alnum(c4));
    CHECK(ascii::is_digit(c4));
    CHECK(!ascii::is_space(c4));

    constexpr char c5 = 'A';
    CHECK(ascii::is_alpha(c5));
    CHECK(ascii::is_alnum(c5));
    CHECK(!ascii::is_digit(c5));
    CHECK(!ascii::is_space(c5));

    constexpr unsigned char c6 = 'B';
    CHECK(ascii::is_alpha(c6));
    CHECK(ascii::is_alnum(c6));
    CHECK(!ascii::is_digit(c6));
    CHECK(!ascii::is_space(c6));

    constexpr char c7 = '!';
    CHECK(!ascii::is_alpha(c7));
    CHECK(!ascii::is_alnum(c7));
    CHECK(!ascii::is_digit(c7));
    CHECK(!ascii::is_space(c7));

    constexpr unsigned char c8 = '@';
    CHECK(!ascii::is_alpha(c8));
    CHECK(!ascii::is_alnum(c8));
    CHECK(!ascii::is_digit(c8));
    CHECK(!ascii::is_space(c8));
}

TEST_CASE("Trimming") {
    std::string str = "  Hello, World!  ";
    CHECK(trim(str, TrimMode::Left) == "Hello, World!  ");
    CHECK(str == "  Hello, World!  ");

    CHECK(trim(str, TrimMode::Right) == "  Hello, World!");
    CHECK(str == "  Hello, World!  ");

    CHECK(trim(str) == "Hello, World!");
    CHECK(str == "  Hello, World!  ");

    trim_in_place(str, TrimMode::Left);
    CHECK(str == "Hello, World!  ");

    trim_in_place(str, TrimMode::Right);
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
    CHECK(trim(str4, TrimMode::Left).empty());
    CHECK(trim(str4, TrimMode::Right).empty());
    CHECK(trim(str4).empty());
    CHECK(trim_and_reduce(str4).empty());

    trim_in_place(str4, TrimMode::Left);
    CHECK(str4.empty());

    std::string str5 = "  ";
    trim_in_place(str5, TrimMode::Right);
    CHECK(str5.empty());

    std::string str6 = "  ";
    trim_in_place(str6);
    CHECK(str6.empty());

    std::string str7 = "  ";
    trim_and_reduce_in_place(str7);
    CHECK(str7.empty());

    std::string str8 = "Hello, World!";
    CHECK(trim(str8, TrimMode::Left) == str8);
    CHECK(trim(str8, TrimMode::Right) == str8);
    CHECK(trim(str8) == str8);
    CHECK(trim_and_reduce(str8) == str8);
    trim_in_place(str8, TrimMode::Left);
    CHECK(str8 == "Hello, World!");
    trim_in_place(str8, TrimMode::Right);
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

TEST_CASE("Splitting with KeepEmpty") {
    const std::string str = "Hello, World!";
    CHECK(split(str, ",", SplitBehavior::KeepEmpty) == std::vector<std::string>{"Hello", " World!"});
    CHECK(split(str, "", SplitBehavior::KeepEmpty) == std::vector<std::string>{"Hello, World!"});
    CHECK(split(str, " ", SplitBehavior::KeepEmpty) == std::vector<std::string>{"Hello,", "World!"});
    CHECK(split(str, "o", SplitBehavior::KeepEmpty) == std::vector<std::string>{"Hell", ", W", "rld!"});

    CHECK(split(str, "Hello, World!", SplitBehavior::KeepEmpty) == std::vector<std::string>{"", ""});
    CHECK(split(str, "Hello, World! ", SplitBehavior::KeepEmpty) == std::vector<std::string>{"Hello, World!"});

    const std::string str2 = "aaa,AAA,bbb,BBB,ccc,CCC";
    CHECK(split(str2, ",", SplitBehavior::KeepEmpty) == std::vector<std::string>{"aaa", "AAA", "bbb", "BBB", "ccc", "CCC"});
    CHECK(split(str2, "a", SplitBehavior::KeepEmpty) == std::vector<std::string>{"", "", "", ",AAA,bbb,BBB,ccc,CCC"});
    CHECK(split(str2, "A", SplitBehavior::KeepEmpty) == std::vector<std::string>{"aaa,", "", "", ",bbb,BBB,ccc,CCC"});
    CHECK(split(str2, "C", SplitBehavior::KeepEmpty) == std::vector<std::string>{"aaa,AAA,bbb,BBB,ccc,", "", "", ""});
    CHECK(split(str2, "D", SplitBehavior::KeepEmpty) == std::vector<std::string>{"aaa,AAA,bbb,BBB,ccc,CCC"});

    const std::string str3 = "a,,b";
    CHECK(split(str3, ",", SplitBehavior::KeepEmpty) == std::vector<std::string>{"a", "", "b"});
    CHECK(split(str3, ",", SplitBehavior::Nothing) == std::vector<std::string>{"a", "b"});

    const std::string str4 = "aa";
    CHECK(split(str4, "a", SplitBehavior::KeepEmpty) == std::vector<std::string>{"", "", ""});
    CHECK(split(str4, "aaa", SplitBehavior::KeepEmpty) == std::vector<std::string>{"aa"});

    const std::string str5 = "aaaaaaaaa"; // 9
    CHECK(split(str5, "aaa", SplitBehavior::KeepEmpty) == std::vector<std::string>{"", "", "", ""});
}

constexpr std::string_view sv = "Hello, World!";
constexpr std::string_view sv2 = "Hello, World!";
constexpr std::string_view sv3 = "Hello, World!";
constexpr std::string_view sv4;

constexpr auto cv = "Hello, World!";
constexpr auto cv2 = "Hello, World!";
constexpr auto cv3 = "Hello, World!";
constexpr auto cv4 = "";

TEST_CASE("StringViewBuilder") {
    {
        constexpr auto sb = StringViewBuilder<sv, sv2>();

        constexpr std::string_view view = sb.view();
        constexpr const char* c_str = sb.c_str();

        constexpr std::size_t x = 2 * sv.size();

        CHECK(view == "Hello, World!Hello, World!");
        CHECK(view.size() == x);

        CHECK(c_str == view.data());
        CHECK(c_str[x] == '\0');
        CHECK(strnlen(c_str) == x);
        CHECK(strcmp(c_str, "Hello, World!Hello, World!") == 0);
    }

    {
        constexpr auto sb = StringViewBuilder<sv, sv2, sv3, sv4>();

        constexpr std::string_view view = sb.view();
        constexpr const char* c_str = sb.c_str();

        constexpr std::size_t x = 3 * sv.size() + sv4.size();

        CHECK(view == "Hello, World!Hello, World!Hello, World!");
        CHECK(view.size() == x);

        CHECK(c_str == view.data());
        CHECK(c_str[x] == '\0');
        CHECK(strnlen(c_str) == x);
        CHECK(strcmp(c_str, "Hello, World!Hello, World!Hello, World!") == 0);
    }
    
    {
        constexpr auto sb = StringViewBuilder<cv, cv2>();

        constexpr std::string_view view = sb.view();
        constexpr const char* c_str = sb.c_str();

        constexpr std::size_t x = 2 * std::char_traits<char>::length(cv);

        CHECK(view == "Hello, World!Hello, World!");
        CHECK(view.size() == x);

        CHECK(c_str == view.data());
        CHECK(c_str[x] == '\0');
        CHECK(strnlen(c_str) == x);
        CHECK(strcmp(c_str, "Hello, World!Hello, World!") == 0);
    }

    {
        constexpr auto sb = StringViewBuilder<cv, cv2, cv3, cv4>();

        constexpr std::string_view view = sb.view();
        constexpr const char* c_str = sb.c_str();

        constexpr std::size_t x = 3 * std::char_traits<char>::length(cv) + std::char_traits<char>::length(cv4);

        CHECK(view == "Hello, World!Hello, World!Hello, World!");
        CHECK(view.size() == x);

        CHECK(c_str == view.data());
        CHECK(c_str[x] == '\0');
        CHECK(strnlen(c_str) == x);
        CHECK(strcmp(c_str, "Hello, World!Hello, World!Hello, World!") == 0);
    }

    {
        // Strings do not have to be outside the function, but they need static storage duration
        static constexpr std::string hello = "Hello, World!";

        constexpr auto sb = StringViewBuilder<sv, cv, sv2, cv2, sv3, cv3, sv4, cv4, hello>();

        constexpr std::string_view view = sb.view();
        constexpr const char* c_str = sb.c_str();

        constexpr std::size_t x = 3 * sv.size() + sv4.size() + 3 * std::char_traits<char>::length(cv) + std::char_traits<char>::length(cv4) + hello.size();

        CHECK(view == "Hello, World!Hello, World!Hello, World!Hello, World!Hello, World!Hello, World!Hello, World!");
        CHECK(view.size() == x);

        CHECK(c_str == view.data());
        CHECK(c_str[x] == '\0');
        CHECK(strnlen(c_str) == x);
        CHECK(strcmp(c_str, "Hello, World!Hello, World!Hello, World!Hello, World!Hello, World!Hello, World!Hello, World!") == 0);
    }

    {
        constexpr auto sb = StringViewBuilder<sv4, cv4>();

        constexpr std::string_view view = sb.view();
        constexpr const char* c_str = sb.c_str();

        constexpr std::size_t x = 0;

        CHECK(view == "");
        CHECK(view.size() == x);

        CHECK(c_str == view.data());
        CHECK(c_str[x] == '\0');
        CHECK(strnlen(c_str) == x);
        CHECK(strcmp(c_str, "") == 0);
    }
}
