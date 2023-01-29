#include "utest.h"

#include <OswLogger.h>
#include "../../include/String.h"
#include "helpers/CaptureSerial.hpp"

/**
 * This test also needs to hook into the serial object to capture the output
 * to really check it. This is done in the CaptureSerial object, which will
 * be bound to the stack-frame of its test - and destroyed upon leaving.
 */

#define EXPECT_LASTLINE(expect) EXPECT_STREQ(expect, Serial.buffer.back().str().c_str())

// Defines to use check log messages (all except debug!)
#ifndef NDEBUG
#define EXPECT_LASTLINE_MSG(lvl, msg) EXPECT_LASTLINE((std::string(lvl) + ": " + __FILE__ + "@" + std::to_string(__LINE__ - 1) + ": " + msg).c_str())
#else
#define EXPECT_LASTLINE_MSG(lvl, msg) EXPECT_LASTLINE((std::string(lvl) + ": " + msg).c_str())
#endif

// Defines to use check debug levels, which are hidden if not in debug mode
#ifndef NDEBUG
#define EXPECT_LASTLINE_MSGDBG(msg) EXPECT_LASTLINE((std::string("D: ") + __FILE__ + "@" + std::to_string(__LINE__ - 1) + ": " + msg).c_str())
#else
#define EXPECT_LASTLINE_MSGDBG(msg) EXPECT_LASTLINE((std::string("")).c_str())
#endif

UTEST(logging, variadic_defines) {
    CaptureSerial capture;
    OSW_LOG_D("Hello World!");
    EXPECT_LASTLINE_MSGDBG("Hello World!");
    OSW_LOG_D("c_str", 42);
    EXPECT_LASTLINE_MSGDBG("c_str42");
    OSW_LOG_D(String("String"), "c_str", 42, 'a', false, 42.042f, 42.042d);
    EXPECT_LASTLINE_MSGDBG("Stringc_str42a042.04242.042");
    OSW_LOG_I("Hello World!");
    EXPECT_LASTLINE_MSG("I", "Hello World!");
    OSW_LOG_I("c_str", 42);
    EXPECT_LASTLINE_MSG("I", "c_str42");
    OSW_LOG_I(String("String"), "c_str", 42, 'a', false, 42.042f, 42.042d);
    EXPECT_LASTLINE_MSG("I", "Stringc_str42a042.04242.042");
    OSW_LOG_W("Hello World!");
    EXPECT_LASTLINE_MSG("W", "Hello World!");
    OSW_LOG_W("c_str", 42);
    EXPECT_LASTLINE_MSG("W", "c_str42");
    OSW_LOG_W(String("String"), "c_str", 42, 'a', false, 42.042f, 42.042d);
    EXPECT_LASTLINE_MSG("W", "Stringc_str42a042.04242.042");
    OSW_LOG_E("Hello World!");
    EXPECT_LASTLINE_MSG("E", "Hello World!");
    OSW_LOG_E("c_str", 42);
    EXPECT_LASTLINE_MSG("E", "c_str42");
    OSW_LOG_E(String("String"), "c_str", 42, 'a', false, 42.042f, 42.042d);
    EXPECT_LASTLINE_MSG("E", "Stringc_str42a042.04242.042");
}

UTEST(logging, functions) {
    CaptureSerial capture;
    OswLogger::getInstance()->info(__FILE__, __LINE__, "Hello World!");
    EXPECT_LASTLINE_MSG("I", "Hello World!");
    OswLogger::getInstance()->info(__FILE__, __LINE__, "c_str", 42);
    EXPECT_LASTLINE_MSG("I", "c_str42");
    OswLogger::getInstance()->info(__FILE__, __LINE__, String("String"), "c_str", 42, 'a', false, 42.042f, 42.042d);
    EXPECT_LASTLINE_MSG("I", "Stringc_str42a042.04242.042");
}

UTEST(logging, newlines) {
    CaptureSerial capture;
    char buffer[256];
    strncpy(buffer, "[char*] This is a line.\nThis is another line.", 256);
    OSW_LOG_I((char*) buffer); // char*
    EXPECT_LASTLINE_MSG("I", "This is another line.");
    OSW_LOG_I((const char*) "[const char*] This is a line.\nThis is another line."); // const char*
    EXPECT_LASTLINE_MSG("I", "This is another line.");
    OSW_LOG_I(std::string("[std::string] This is a line.\nThis is another line."));
    EXPECT_LASTLINE_MSG("I", "This is another line.");
    OSW_LOG_I(String("[String] This is a line.\nThis is another line."));
    EXPECT_LASTLINE_MSG("I", "This is another line.");
    std::string sstr("[std::string&] This is a line.\nThis is another line.");
    OSW_LOG_I(sstr);
    EXPECT_LASTLINE_MSG("I", "This is another line.");
    String str("[String&] This is a line.\nThis is another line.");
    OSW_LOG_I(str);
    EXPECT_LASTLINE_MSG("I", "This is another line.");
}