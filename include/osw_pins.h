#ifndef OSW_PINS_H
#define OSW_PINS_H

// pin mapping
#if defined(GPS_EDITION)
#define TFT_CS 5
#define TFT_DC 12
#define TFT_RST 2
#define TFT_SCK 18
#define TFT_MOSI 23
#define TFT_MISO -1  // no data coming back
#define TFT_LED 9
#else
#define TFT_CS 5
#define TFT_DC 12
#define TFT_RST 33
#define TFT_SCK 18
#define TFT_MOSI 23
#define TFT_MISO -1  // no data coming back
#define TFT_LED 9
#endif

#define SD_CS 4
// SD_MISO 19
// for SCK, MOSI see TFT

#define GPS_RX 14
#define GPS_TX 27
#define GPS_FON 26
#define GPS_3D_FIX 36
#define GPS_GEO_FENCE 39
#define RX1 27
#define TX1 14

#define RTC_INT 32
#define BMA_INT_1 34
#define BMA_INT_2 35

#if defined(GPS_EDITION)
#define BTN_1 0
#define BTN_2 33
#define BTN_3 13
#define VIBRATE 35
#else
#define BTN_1 0
#define BTN_2 10
#define BTN_3 13
#endif

#define BTN_1_APP_SWITCH_TIMEOUT 500
#define BTN_1_SLEEP_TIMEOUT 1500

#define SCL 22
#define SDA 21

#define STAT_PWR 15
#define B_MON 25

#endif


// VP is 36
// VN is 39
