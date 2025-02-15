#pragma once

#define OSW_PLATFORM_ENVIRONMENT 1
#define OSW_PLATFORM_ENVIRONMENT_TEMPERATURE 1
#define OSW_PLATFORM_ENVIRONMENT_PRESSURE 1
#define OSW_PLATFORM_ENVIRONMENT_HUMIDITY 1
//#define OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER 0
#define OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER 1

#define OSW_PLATFORM_HARDWARE_DS3231MZ 1
#define OSW_PLATFORM_HARDWARE_BMA400 1
//#define OSW_PLATFORM_HARDWARE_BMI270 0
//#define OSW_PLATFORM_HARDWARE_BMP581 0
#define OSW_PLATFORM_HARDWARE_BME280 1
#define OSW_PLATFORM_HARDWARE_BME280_ADDR_77 1
#define OSW_PLATFORM_HARDWARE_QMC5883L 0
#define OSW_PLATFORM_HARDWARE_QMC5883P 1 // TODO: Implement
//#define OSW_PLATFORM_HARDWARE_VIRTUAL 0
#define OSW_PLATFORM_HARDWARE_ESP32 1
//#define OSW_PLATFORM_HARDWARE_ESP32_USE_JTAG_SERIAL 0
#define OSW_PLATFORM_HARDWARE_DISPLAY_CS 5
#define OSW_PLATFORM_HARDWARE_DISPLAY_DC 8
#define OSW_PLATFORM_HARDWARE_DISPLAY_RST 2
#define OSW_PLATFORM_HARDWARE_DISPLAY_SCK 19
#define OSW_PLATFORM_HARDWARE_DISPLAY_MOSI 21
#define OSW_PLATFORM_HARDWARE_DISPLAY_LED 7
#define OSW_PLATFORM_HARDWARE_DISPLAY_SPI_NUM VSPI
#define OSW_PLATFORM_HARDWARE_DISPLAY_ROTATION 2
//#define OSW_PLATFORM_HARDWARE_VIBRATE 1
//#define OSW_PLATFORM_BLOCK_SLEEP 0 // used during the integration of new platforms to prevent the device from going to sleep
//#define OSW_PLATFORM_IS_FLOW3R_BADGE 0
#define OSW_PLATFORM_DEFAULT_CPUFREQ 240

//#define OSW_DEVICE_ESP32_WIFI_LOWPWR 0
#define OSW_DEVICE_DS3231MZ_RTCINT 38
#define OSW_DEVICE_BMA400_INT1 36
#define OSW_DEVICE_BMA400_INT2 -1 // TODO: check if this detected - I ran out of pins
#define OSW_DEVICE_I2C_SCL 25
#define OSW_DEVICE_I2C_SDA 26
#define OSW_DEVICE_I2C_DEBUG 1
#define OSW_DEVICE_TPS2115A_STATPWR 37
#define OSW_DEVICE_ESP32_BATLVL 25 // TODO: implement MAX17043 support #define LALRT 39
#define OSW_DEVICE_ESP32_USE_INTTEMP 1
// TODO: implement WIFI power toggle #define OSW_DEVICE_WIFI_POWER_TOGGLE 12
// TODO: #define MOTOR_ENABLE 20