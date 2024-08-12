#pragma once

#include <cstdint>
#include <WString.h>
#include OSW_TARGET_PLATFORM_HEADER

#if OSW_PLATFORM_HARDWARE_ESP32_USE_JTAG_SERIAL == 1
#include <esp32s3/rom/uart.h> // change this if needed, as this is specifically ESP32s2
#include <esp_rom_caps.h>
#include <esp_rom_uart.h>
#else
#include <HardwareSerial.h>
#endif

/**
 * @brief This class is not the fastest (only one byte at a time), but it is the most compatible
 *
 */
class OswSerial {
  public:
    /**
     * @brief Start / initialize serial communication (not all platforms may respect the baud rate)
     *
     * @param baud
     */
    void begin(unsigned long baud);

    /**
     * @brief Put one byte to the serial port (potentially buffered)
     *
     * If you only want to print a single character, prefer this method over print()
     *
     * @param c
     */
    void putc(uint8_t c);

    /**
     * @brief FLush out the outgoing buffer, if buffered
     *
     */
    void flush();

    /**
     * @brief Get one byte from the serial port
     *
     * @param c where to store the byte
     * @return true if a byte was read
     * @return false if no byte was read
     */
    bool getc(uint8_t& c);

    /**
     * @brief Print a string to the serial port and flush it
     *
     * @tparam T
     * @param t
     */
    template<typename T>
    void print(T t) {
#if OSW_PLATFORM_HARDWARE_ESP32_USE_JTAG_SERIAL == 1
        for(auto& c : String(t)) {
            this->putc(c);
        }
        this->flush();
#else
        Serial.print(t);
#endif
    }

    /**
     * @brief Print a string to the serial port and flush it
     *
     * @tparam T
     * @param t
     */
    template<typename T>
    void println(T t) {
        this->print(t);
        this->println();
    }

    /**
     * @brief Print a newline to the serial port and flush it
     *
     */
    void println();

    static OswSerial* getInstance();
    static void resetInstance();
  private:
    static OswSerial* instance;
};
