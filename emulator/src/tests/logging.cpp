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