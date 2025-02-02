#ifndef OSW_EMULATOR
#include <Arduino_GFX.h>
#include <databus/Arduino_ESP32SPI.h>
#include <display/Arduino_GC9A01.h>
#endif
#include <gfx_2d_print.h>
#include <gfx_util.h>
#include <math_osm.h>
#include <pngle.h>

#include "Arduino_Canvas_Graphics2D.h"
#include "config_defaults.h"
#include "osw_hal.h"
#include "osw_pins.h"
#include OSW_TARGET_PLATFORM_HEADER

#if OSW_PLATFORM_HARDWARE_DISPLAY_RST == 0
#define OSW_PLATFORM_HARDWARE_DISPLAY_RST -1
#endif
#ifndef OSW_PLATFORM_HARDWARE_DISPLAY_ROTATION
#define OSW_PLATFORM_HARDWARE_DISPLAY_ROTATION -1
#endif

#ifndef OSW_EMULATOR
Arduino_DataBus* bus = new Arduino_ESP32SPI(
    OSW_PLATFORM_HARDWARE_DISPLAY_DC,
    OSW_PLATFORM_HARDWARE_DISPLAY_CS,
    OSW_PLATFORM_HARDWARE_DISPLAY_SCK,
    OSW_PLATFORM_HARDWARE_DISPLAY_MOSI,
    -1, // no data coming back
    OSW_PLATFORM_HARDWARE_DISPLAY_SPI_NUM);
Arduino_GC9A01* tft = new Arduino_GC9A01(bus, OSW_PLATFORM_HARDWARE_DISPLAY_RST, OSW_PLATFORM_HARDWARE_DISPLAY_ROTATION, true /* IPS */);
#else
FakeDisplay* tft = nullptr;
#endif

class PixelPainter : public DrawPixel {
  public:
    PixelPainter() {};
    void drawPixel(int32_t x, int32_t y, uint16_t color) {
        // draw pixel
        tft->drawPixel(x, y, color);
    };
};
PixelPainter* pixelPainter = new PixelPainter();

void OswHal::requestDisableDisplayBuffer() {
    _requestDisableBuffer = true;
}
void OswHal::requestEnableDisplayBuffer() {
    _requestEnableBuffer = true;
}
void OswHal::disableDisplayBuffer() {
    if(!this->displayBufferEnabled())
        return;
    this->canvas->disableBuffer(pixelPainter);
}
void OswHal::enableDisplayBuffer() {
    if(this->displayBufferEnabled())
        return;
    this->canvas->enableBuffer();
}
bool OswHal::displayBufferEnabled() {
    return this->canvas->hasBuffer();
}

void OswHal::setupDisplay(bool fromLightSleep) {
#ifdef OSW_EMULATOR
    // Always fetch the current instance, just in case the emulator replaced it in the meantime (and as tft is not bound to this objects lifetime)
    tft = fakeDisplayInstance.get();
#endif

    // Configure backlight pin as early as possible to avoid flickering on startup
#if OSW_PLATFORM_HARDWARE_DISPLAY_LED != 0
    ledcAttachPin(OSW_PLATFORM_HARDWARE_DISPLAY_LED, 1);
    ledcSetup(1, 12000, 8);  // 12 kHz PWM, 8-bit resolution
    ledcWrite(1, 0); // force off initially
#else
#ifndef OSW_EMULATOR // meh, the emulator ignores this for now...
#warning "Display LED pin unconfigured; can't control backlight brightness"
#endif
#endif

    // Moved from static allocation to here, as new() operators are limited (size-wise) in that context
    if(!this->canvas)
        this->canvas = new Arduino_Canvas_Graphics2D(DISP_W, DISP_H, tft);

    if(!fromLightSleep) {
        this->canvas->begin();  // will not deconfigured upon light sleep, use default speed and default SPI mode

        // Clean the buffer, if it is enabled (otherwise it would take too long during system startup)
        if(this->displayBufferEnabled()) {
            this->gfx()->fillBuffer(rgb565(0, 0, 0));
            this->flushCanvas();
        }
    }

    // Finally, enable the backlight and show the canvas
    this->displayOn();
}

void OswHal::stopDisplay(bool toLightSleep) {
    this->displayOff();

    // If we have a fast buffer, also clean it, to prevent flashing upon returning from light sleep (deep sleep will loose the buffer anyway, but as this is fast...)
    if(!toLightSleep and this->displayBufferEnabled()) {
        this->gfx()->fillBuffer(rgb565(0, 0, 0));
        this->flushCanvas();
    }

#if OSW_PLATFORM_HARDWARE_DISPLAY_LED != 0
    ledcDetachPin(OSW_PLATFORM_HARDWARE_DISPLAY_LED);
    // just pull down the backlight pin
    pinMode(OSW_PLATFORM_HARDWARE_DISPLAY_LED, OUTPUT);
    digitalWrite(OSW_PLATFORM_HARDWARE_DISPLAY_LED, LOW);
#endif
}

Arduino_Canvas_Graphics2D* OswHal::getCanvas(void) {
    return this->canvas;
}
Graphics2DPrint* OswHal::gfx(void) {
    return this->canvas;
}

void OswHal::flushCanvas(void) {
    this->canvas->flush();
}

void OswHal::displayOff(void) {
    this->setBrightness(0, false);
    tft->displayOff();
    _screenOffSince = millis();
}

unsigned long OswHal::screenOnTime() {
    return millis() - _screenOnSince;
}
unsigned long OswHal::screenOffTime() {
    return millis() - _screenOffSince;
}

void OswHal::displayOn() {
    _screenOnSince = millis();
#if OSW_PLATFORM_HARDWARE_DISPLAY_ROTATION != -1
    tft->setRotation(OSW_PLATFORM_HARDWARE_DISPLAY_ROTATION); // fix rotation not being applied via orig constr
#endif
    tft->displayOn(); // instruct display to show image, before enabling backlight
    this->setBrightness(OswConfigAllKeys::settingDisplayBrightness.get(), false);
}

void OswHal::setBrightness(uint8_t b, bool storeToNVS) {
    _brightness = b;
#if OSW_PLATFORM_HARDWARE_DISPLAY_LED != 0
    ledcWrite(1, _brightness);
#endif
    if(storeToNVS) {
        OswConfig::getInstance()->enableWrite();
        OswConfigAllKeys::settingDisplayBrightness.set(_brightness);
        OswConfig::getInstance()->disableWrite();
    }
    OSW_LOG_D("Setting brightness to ", (int) b, storeToNVS ? " (stored)" : " (not stored)");
}

void OswHal::increaseBrightness(uint8_t v) {
    uint8_t new_brightness = 0;

    if(_brightness == 0) {
        _brightness = OswHal::screenBrightness(true);
    }

    if (_brightness > 255 - v) {
        new_brightness = 255;
    } else {
        new_brightness = _brightness + v;
    }
    setBrightness(new_brightness);
}

void OswHal::decreaseBrightness(uint8_t v) {
    uint8_t new_brightness = 0;

    if(_brightness == 0) {
        _brightness = OswHal::screenBrightness(true);
    }

    short minBrightness = 0;
#ifdef DISPLAY_MIN_BRIGHTNESS
    minBrightness = DISPLAY_MIN_BRIGHTNESS;
#endif

    if (minBrightness < 0) {
        minBrightness = 0;
    }
    if ((_brightness - v) < minBrightness) {
        new_brightness = minBrightness;
    } else {
        new_brightness = _brightness - v;
    }
    setBrightness(new_brightness);
}

uint8_t OswHal::screenBrightness(bool checkHardware) {
    uint8_t screen_brightness = 0;

    if(checkHardware) {
#if OSW_PLATFORM_HARDWARE_DISPLAY_LED != 0
        screen_brightness = ledcRead(1);
#endif
        _brightness = screen_brightness;
    }
    return _brightness;
}
