#include "utest.h"

#include <OswLogger.h>
#include "../../include/String.h"

UTEST(logging, variadic_defines) {
    OSW_LOG_D("Hello World!");
    OSW_LOG_D("c_str", 42);
    OSW_LOG_D(String("String"), "c_str", 42, 'a', false, 42.042f, 42.042d);
    OSW_LOG_I("Hello World!");
    OSW_LOG_I("c_str", 42);
    OSW_LOG_I(String("String"), "c_str", 42, 'a', false, 42.042f, 42.042d);
    OSW_LOG_W("Hello World!");
    OSW_LOG_W("c_str", 42);
    OSW_LOG_W(String("String"), "c_str", 42, 'a', false, 42.042f, 42.042d);
    OSW_LOG_E("Hello World!");
    OSW_LOG_E("c_str", 42);
    OSW_LOG_E(String("String"), "c_str", 42, 'a', false, 42.042f, 42.042d);
}

UTEST(logging, functions) {
    OswLogger::getInstance()->info(__FILE__, __LINE__, "Hello World!");
    OswLogger::getInstance()->info(__FILE__, __LINE__, "c_str", 42);
    OswLogger::getInstance()->info(__FILE__, __LINE__, String("String"), "c_str", 42, 'a', false, 42.042f, 42.042d);
}

UTEST(logging, newlines) {
    char buffer[256];
    strncpy(buffer, "[char*] This is a line.\nThis is another line.", 256);
    OSW_LOG_I((char*) buffer); // char*
    OSW_LOG_I((const char*) "[const char*] This is a line.\nThis is another line."); // const char*
    OSW_LOG_I(std::string("[std::string] This is a line.\nThis is another line."));
    OSW_LOG_I(String("[String] This is a line.\nThis is another line."));
    std::string sstr("[std::string&] This is a line.\nThis is another line.");
    OSW_LOG_I(sstr);
    String str("[String&] This is a line.\nThis is another line.");
    OSW_LOG_I(str);
}