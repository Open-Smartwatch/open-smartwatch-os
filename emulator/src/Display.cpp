#include "../include/Display.h"
#include <OswLogger.h>

std::unique_ptr<FakeDisplay> fakeDisplayInstance;

FakeDisplay::FakeDisplay(int width, int height, SDL_Window* window, SDL_Renderer* renderer) : Arduino_G(width, height), mainWindow(window), mainRenderer(renderer), width(width), height(height) { };

FakeDisplay::~FakeDisplay() {

}

void FakeDisplay::begin(int32_t speed) {
    // Nothing to do...
}

void FakeDisplay::drawBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED
}

void FakeDisplay::drawIndexedBitmap(int16_t x, int16_t y, uint8_t* bitmap, uint16_t* color_index, int16_t w, int16_t h) {
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED
}

void FakeDisplay::draw3bitRGBBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h) {
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED
}

void FakeDisplay::draw24bitRGBBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h) {
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED
}

void FakeDisplay::drawPixel(int32_t x, int32_t y, uint16_t color) {
    // color is in rgb565 (r=5 bit, g=6 bit, b=5 bit)
    const Uint8 r =  ((color >> 8) & 0b11111000);
    const Uint8 g = ((color >> 3) & 0b11111100);
    const Uint8 b = ((color << 3));
    SDL_SetRenderDrawColor(this->mainRenderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawPoint(this->mainRenderer, x, y);
}

void FakeDisplay::draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t* bitmap, int16_t w, int16_t h) {
    // On a real CPU we can just iterate over the givenbitmap and render every pixel by itself
    for(int16_t currW = 0; currW < w; ++currW)
        for(int16_t currH = 0; currH < h; ++currH)
            this->drawPixel(x + currW, y + currH, bitmap[currH * w + currW]);
}

void FakeDisplay::displayOn() {
    this->isEnabled = true;
}

void FakeDisplay::displayOff() {
    this->isEnabled = false;
}