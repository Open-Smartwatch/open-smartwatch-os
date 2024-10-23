#pragma once
#ifdef OSW_FEATURE_WEATHER

#include <Arduino.h>
#include <osw_hal.h>

class OswAppWeatherIconPrinter {
  public:
    OswAppWeatherIconPrinter();
    void drawRain(int x, int y, float k);
    void drawSnow(int x, int y, int level, float k);
    void drawCloud(int x, int y, uint32_t color = rgb888(255, 255, 255), float k = 1);
    void drawSun(int x, int y, float k = 1);
    void drawMoon(int x, int y, int radius = 15);
    void drawThunderBolt(int x, int y, float k);
    void drawFog(int x, int y, float k, int mistFog);
    void drawIcon(int code, int x, int y, float k = 1);
    void drawDroplet(int x, int y, float k, uint32_t color = rgb888(255, 255, 255));
    void setColors(uint32_t cloudBright, uint32_t cloudDark, uint32_t thunderbolt, uint32_t sun, uint32_t droplet);
    void getHal(OswHal* h);
  private:
    OswHal* hal;
    uint32_t cloudBrightColor = rgb888(255,255,255);
    uint32_t cloudDarkColor = rgb888(253,253,253);
    uint32_t thunderboltColor = rgb888(255, 222, 40);
    uint32_t sunColor = rgb888(255, 250, 1);
    uint32_t dropletColor = rgb888(255,255,255);
};
#endif
