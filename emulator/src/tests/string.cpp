#include "utest.h"

#include <string>
#include "../../include/String.h"

UTEST(string, constructors) {
    String a = "a";
    String b = std::string("b");
    String c = (short) 42;
    String d = (int) 42;
    String e = (long) 42;
    String f = (float) 42.042;
    String g = (double) 42.042;

    EXPECT_STREQ("a", a.c_str());
    EXPECT_STREQ("b", b.c_str());
    EXPECT_STREQ("42", c.c_str());
    EXPECT_STREQ("42", d.c_str());
    EXPECT_STREQ("42", e.c_str());
    EXPECT_STREQ("42.042", f.c_str());
    EXPECT_STREQ("42.042", g.c_str());
}

UTEST(string, concat_string) {
    String a = "a";
    String b = "b";

    EXPECT_STREQ("ab", (a + b).c_str());
    EXPECT_STREQ("ba", (b + a).c_str());
}

UTEST(string, concat_std_string) {
    String a = "a";
    std::string b = "b";

    EXPECT_STREQ("ab", (a + b).c_str());
    EXPECT_STREQ("ba", (b + a).c_str());
}

UTEST(string, concat_int) {
    String a = "a";
    int b = 42; // which is also the char '*' in ASCII
    int c = 0; // which is also the char '\0' in ASCII

    EXPECT_STREQ("a42", (a + b).c_str());
    EXPECT_STREQ("42a", (b + a).c_str());
    EXPECT_STREQ("a0", (a + c).c_str());
    EXPECT_STREQ("0a", (c + a).c_str());
}

UTEST(string, from_int) {
    int i0 = 42;
    int i1 = 0;
    String s0(i0);
    String s1(i1);

    EXPECT_STREQ("42", s0.c_str());
    EXPECT_STREQ("0", s1.c_str());
}

UTEST(string, from_float) {
    float f0 = 0.0;
    float f1 = 42.0420; // Note, that we only have 3 digits after the comma
    float f2 = 42.41; // Float can't describe 42.42 exactly
    String s0(f0);
    String s1(f1);
    String s2(f2);

    EXPECT_STREQ("0", s0.c_str());
    EXPECT_STREQ("42.042", s1.c_str());
    EXPECT_STREQ("42.41", s2.c_str());
}

UTEST(string, to_float) {
    float f0 = 42.0420;
    float f1 = 0;
    String s0(f0);
    String s1(f1);
    float r0 = s0.toFloat();
    float r1 = s1.toFloat();

    EXPECT_EQ(f0, r0);
    EXPECT_EQ(f1, r1);
}

UTEST(string, concat_float) {
    String a = "a";
    float b = 42.41; // Float can't describe 42.42 exactly

    EXPECT_STREQ("a42.41", (a + b).c_str());
    EXPECT_STREQ("42.41a", (b + a).c_str());
}

UTEST(string, concat_double) {
    String a = "a";
    double b = 42.42;

    EXPECT_STREQ("a42.42", (a + b).c_str());
    EXPECT_STREQ("42.42a", (b + a).c_str());
}