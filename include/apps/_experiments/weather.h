#pragma once
#ifdef OSW_FEATURE_WEATHER
#include <osw_hal.h>
#include <vector>
#include "osw_app.h"

using namespace std;

class PrintWeatherIcon {
  public:
    PrintWeatherIcon();
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


class OswAppWeather : public OswApp {
  public:
    typedef struct {
        int temp;
        int humidity;
        int pressure;
        int weather;
        bool _lastUpdate = false; // used only by the decoder
    } weather_update_t;
    OswAppWeather(void) {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWeather() {};
  private:
    void getW();
    void printW();
    void drawLayout();
    bool loadData();
    void drawDate();
    void drawWeather();
    void printLastUpdate();
    void drawTriangleThick(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t radius,uint16_t color);
    void drawRefreshIcon(uint16_t color);
    void printDate();
    void weatherRequest();
    void getDayList(int nUpdates = 24);
    int getNextDay();
    int getPrevDay();
    PrintWeatherIcon printWIcon;
    Preferences pref;
    String api_key;
    String url;
    String state1;
    String location1;
    bool displayBufferDisabled = false;
    bool getFinish = false ;
    uint8_t mainSelector = 2;
    uint8_t daySelector = 0;
    uint8_t placeSelector = 0;
    uint8_t updtSelector = 0;
    OswHal* hal = OswHal::getInstance();
    std::vector<weather_update_t> forecast;//one update each 3h
    time_t initTimestamp;
    char initTimeMD[6];
    char dateMD[3][6];
    char initTimeDMY[11];
    char timeUpdt[6];
    char buffer[40];
    bool dataLoaded = false;
    bool requestMode = false;
    time_t updtTime;
    tm* tmInit;
    std::vector<int> dayFirstUpdt{};// n-th entry is the index of first update of the n-th day
    bool _request();
};
#endif