#include "utest.h"

#include <OswLogger.h>
#include "../../include/String.h"
#include "CaptureSerial.hpp"

/**
 * This test also needs to hook into the serial object to capture the output
 * to really check it. This is done in the CaptureSerial object, which will
 * be bound to the stack-frame of its test - and destroyed upon leaving.
 */

UTEST(serial, basic_buffer) {
    CaptureSerial capture;
    EXPECT_STREQ(Serial.buffer.back().str().c_str(), ""); // Upon serial buffer creation, it should be empty
    Serial.print("ABC");
    EXPECT_STREQ(Serial.buffer.back().str().c_str(), "ABC");
    Serial.print("DEF");
    EXPECT_STREQ(Serial.buffer.back().str().c_str(), "ABCDEF");
    Serial.println();
    EXPECT_STREQ(Serial.buffer.back().str().c_str(), "ABCDEF"); // The buffer shall not contain the newline, just after a println()...
    Serial.print("");
    EXPECT_STREQ(Serial.buffer.back().str().c_str(), ""); // ...but now it should!
    Serial.println("GHI");
    EXPECT_STREQ(Serial.buffer.back().str().c_str(), "GHI"); // The buffer shall not contain the newline, just after a println(smth)...
    Serial.print("");
    EXPECT_STREQ(Serial.buffer.back().str().c_str(), ""); // ...but now it should!
}