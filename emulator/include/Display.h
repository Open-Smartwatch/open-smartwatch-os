#pragma once

#include <memory>

#include <SDL2/SDL.h>

#include "Arduino_GFX.h"
#include "Defines.h"

class FakeDisplay : public Arduino_G {
public:
    FakeDisplay(int width, int height, SDL_Window* window, SDL_Renderer* renderer);
    ~FakeDisplay();

    void displayOn();
    void displayOff();

    void drawPixel(int32_t x, int32_t y, uint16_t color);

    // Required by Arduino_G
    void begin(int32_t speed = 0) override;
    void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) override;
    void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h) override;
    void draw3bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) override;
    void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) override;
    void draw24bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) override;
private:
    SDL_Window* mainWindow;
    SDL_Renderer* mainRenderer;
    const int width;
    const int height;
    bool isEnabled = false;
};

extern std::unique_ptr<FakeDisplay> fakeDisplayInstance;