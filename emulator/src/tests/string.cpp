#include "utest.h"

#include <string>
#include "../../include/String.h"

UTEST(string, concat_string) {
    String a = "a";
    String b = "b";

    ASSERT_STREQ("ab", (a + b).c_str());
    ASSERT_STREQ("ba", (b + a).c_str());
}

UTEST(string, concat_std_string) {
    String a = "a";
    std::string b = "b";

    ASSERT_STREQ("ab", (a + b).c_str());
    ASSERT_STREQ("ba", (b + a).c_str());
}

UTEST(string, concat_int) {
    String a = "a";
    int b = 42; // which is also the char '*' in ASCII

    ASSERT_STREQ("a42", (a + b).c_str());
    ASSERT_STREQ("42a", (b + a).c_str());
}

UTEST(string, from_float) {
    float a = 42.0420; // Note, that we only have 3 digits after the comma
    String b(a);
    String c = a;

    ASSERT_STREQ("42.042", b.c_str());
    ASSERT_STREQ("42.042", c.c_str());
}

UTEST(string, to_float) {
    float a = 42.0420;
    String b(a);
    float c = b.toFloat();

    ASSERT_EQ(a, c);
}

UTEST(string, concat_float) {
    String a = "a";
    float b = 42.42;

    ASSERT_STREQ("a42.42", (a + b).c_str());
    ASSERT_STREQ("42.42a", (b + a).c_str());
}