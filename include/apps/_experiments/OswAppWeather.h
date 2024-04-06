#pragma once
#ifdef OSW_FEATURE_WEATHER
#include <osw_hal.h>
#include <vector>
#include <OswAppV1.h>
#include "ArduinoJson.h"
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
    class WeatherDecoder {
      public:
        WeatherDecoder(const String& input_String);
        bool strIsValid();
        time_t getTime();
        std::vector<OswAppWeather::weather_update_t> getUpdates();
      private:
        time_t _str2time(const String& t);
        int _str2temp(const String& temp);
        int _str2hum(const String& humidity);
        int _str2pres(const String& pressure);
        int _str2wthr(const String& weather);
        bool in_ok = true;
        int nUpdates = 0;
        String in_String;
    };

    class WeatherParser {
      public:
        WeatherParser();
        std::optional<String> encodeWeather(DynamicJsonDocument& doc);
      private:
        int _getWCond(int weather_code);
        int cnt;
        std::vector<OswAppWeather::weather_update_t> updates;
        std::vector<int>clearCode{800};//0
        std::vector<int>cloudsMin{801};//1
        std::vector<int>cloudsMed{802};//2
        std::vector<int>cloudsHigh{803, 804};//3
        std::vector<int>mist{701};//4
        std::vector<int>fog{741};//5
        std::vector<int>snowMin{611, 612, 615, 616};//6
        std::vector<int>snowMed{600, 613, 601, 620};//7
        std::vector<int>snowHigh{602, 621, 622};//8
        std::vector<int>rainMin{500, 300, 301, 302, 310, 311, 312, 313, 314, 321};//9
        std::vector<int>rainMed{502, 501};//10
        std::vector<int>rainHigh{503, 504, 511, 520, 521, 522, 531};//11
        std::vector<int>thunderstorm{200, 201, 210, 211, 231, 230};//12
        std::vector<int>thunderstormHeavy{202, 212, 221, 232};//13
        std::vector<int>squallTornado{771, 781};//14
        //15 ->unknown
        std::vector<std::vector<int>>weather_conditions{clearCode, cloudsMin, cloudsMed, cloudsHigh, mist, fog, snowMin, snowMed,
                snowHigh, rainMin, rainMed, rainHigh, thunderstorm,
                thunderstormHeavy, squallTornado };
    };

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
    OswHal* hal = nullptr;
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
    void drawPopUp();
};
#endif
