#pragma once
#ifdef OSW_FEATURE_WEATHER
#include <osw_hal.h>
#include <vector>
#include <OswAppV1.h>
#include "OswAppWeatherIconPrinter.h"

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
    OswAppWeatherIconPrinter printWIcon;
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