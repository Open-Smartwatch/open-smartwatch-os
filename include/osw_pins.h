#pragma once

// NOTE This file will slowly be migrated into the platform header files.

#define SD_CS 4
// SD_MISO 19
// for SCK, MOSI see TFT

#define GPS_RX 27
#define GPS_TX 14
#define GPS_FON 26
#define GPS_3D_FIX 36
#define GPS_GEO_FENCE 39

#if defined(GPS_EDITION)
#define BTN_1 0
#define BTN_2 13
#define BTN_3 33
#elif defined(GPS_EDITION_ROTATED)
#define BTN_1 13
#define BTN_2 33
#define BTN_3 0
#elif OSW_DEVICE_BTN_TOUCH == 1
#define BTN_1 OSW_DEVICE_BTN_TOUCH_0
#define BTN_2 OSW_DEVICE_BTN_TOUCH_1
#define BTN_3 OSW_DEVICE_BTN_TOUCH_2
//#define BTN_4 OSW_DEVICE_BTN_TOUCH_3
#else
#define BTN_1 0
#define BTN_2 13
#define BTN_3 10
#endif

// assign active LOW or HIGH states according to hardware
#if defined(GPS_EDITION_ROTATED)
#define BTN_STATE_ARRAY {HIGH, HIGH, LOW}
#elif OSW_DEVICE_BTN_TOUCH == 1
#define BTN_TOUCH_THRESHOLD_ARRAY {40,40,40}
#else
#define BTN_STATE_ARRAY {LOW, HIGH, HIGH}
#endif

// button order is: select, up, down
#define BTN_NUMBER 3
#define BTN_POSX_ARRAY {24, 208, 208}
#define BTN_POSY_ARRAY {190, 44, 190}
#define BTN_POS_ISTOP_ARRAY {false, true, false}
#define BTN_POS_ISLEFT_ARRAY {true, false, false}
#define BTN_NAME_ARRAY {"SELECT", "UP", "DOWN"}
#define BTN_PIN_ARRAY {BTN_1, BTN_2, BTN_3}
