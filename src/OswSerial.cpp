#include <OswSerial.h>

OswSerial* OswSerial::instance = nullptr;

void OswSerial::begin(unsigned long baud) {
#if OSW_PLATFORM_HARDWARE_ESP32_USE_JTAG_SERIAL == 1
    // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG would do its magic - but is was not set in ESP-IDF during compile time, so...
    // the function bootloader_console_init() would normally set this up, but the compiled one configures another UART
    {
        // the following line is just a forward to the rom-function
        // ld will be instructed to just use a hard-coded addresses
        // ...but the ESP-IDF version is too old and does not have this function yet
        // esp_rom_output_switch_buffer(ESP_ROM_USB_SERIAL_DEVICE_NUM);

        // this replaces the above function, at least in ESP-IDF v4.3
        {
            UartDevice* uart = GetUartDevice();
            uart->buff_uart_no = ESP_ROM_USB_SERIAL_DEVICE_NUM;
        }

        // same as before, but not needed for older chips
        // esp_rom_output_set_as_console(ESP_ROM_USB_SERIAL_DEVICE_NUM);

        // esp_rom_output_switch_buffer or uart_buff_switch // -> does NOT exist in my old ESP-IDF version
        // -> use older variant, which directly accesses the rom-code
    }
#else
    Serial.begin(115200);
#endif
}

void OswSerial::putc(uint8_t c) {
#if OSW_PLATFORM_HARDWARE_ESP32_USE_JTAG_SERIAL == 1
    esp_rom_uart_putc(c);
#else
    Serial.print((char) c);
#endif
}

void OswSerial::flush() {
#if OSW_PLATFORM_HARDWARE_ESP32_USE_JTAG_SERIAL == 1
    esp_rom_uart_flush_tx(ESP_ROM_USB_SERIAL_DEVICE_NUM);
#else
    Serial.flush();
#endif
}

bool OswSerial::getc(uint8_t& c) {
#if OSW_PLATFORM_HARDWARE_ESP32_USE_JTAG_SERIAL == 1
    return esp_rom_uart_rx_one_char(&c) == 0;
#else
    if(::Serial.available()) {
        c = ::Serial.read();
        return true;
    } else {
        return false;
    }
#endif
}

void OswSerial::println() {
#if OSW_PLATFORM_HARDWARE_ESP32_USE_JTAG_SERIAL == 1
    this->putc('\n');
    this->flush();
#else
    Serial.println();
#endif
}

OswSerial* OswSerial::getInstance() {
    if(OswSerial::instance == nullptr) {
        OswSerial::instance = new OswSerial();
    }
    return OswSerial::instance;
};

void OswSerial::resetInstance() {
    delete OswSerial::instance;
    OswSerial::instance = nullptr;
}
