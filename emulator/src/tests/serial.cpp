#include "utest.h"

#include <OswLogger.h>
#include "../../include/String.h"
#include "helpers/CaptureSerial.hpp"

/**
 * This test also needs to hook into the serial object to capture the output
 * to really check it. This is done in the CaptureSerial object, which will
 * be bound to the stack-frame of its test - and destroyed upon leaving.
 */

UTEST(serial, buffer_basic) {
    Serial.setBuffered(false);
    EXPECT_FALSE(Serial.isBuffered());
    EXPECT_TRUE(Serial.buffer.empty());

    // Enable it once
    Serial.setBuffered(true);
    EXPECT_TRUE(Serial.isBuffered() );
    EXPECT_TRUE(Serial.buffer.empty()); // Still be empty!

    // Add some string and make sure when the buffer is extended
    Serial.print("ABC");
    EXPECT_FALSE(Serial.buffer.empty());
    EXPECT_STREQ(Serial.buffer.back().str().c_str(), "ABC");
    Serial.println();
    EXPECT_FALSE(Serial.buffer.empty());
    EXPECT_STREQ(Serial.buffer.back().str().c_str(), "ABC");

    Serial.println("EFG");
    EXPECT_FALSE(Serial.buffer.empty());
    EXPECT_STREQ(Serial.buffer.back().str().c_str(), "EFG");

    // Disable it again
    Serial.setBuffered(false);
    EXPECT_FALSE(Serial.isBuffered());
    EXPECT_TRUE(Serial.buffer.empty());
}

UTEST(serial, buffer_capture_serial_class) {
    CaptureSerial capture;
    EXPECT_TRUE(Serial.buffer.empty()); // Upon serial buffer creation, it should be empty
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