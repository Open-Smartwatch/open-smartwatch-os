#pragma once
#include <osw_hal.h>
#include <vector>
#include "osw_app.h"

using namespace std;
typedef struct {
    int temp;
    int humidity;
    int pressure;
    int weather;
    bool _last_update = false; // used only by the decoder
} weather_update_t;


class OswAppWeather : public OswApp {
  public:
    OswAppWeather(void) {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    void get_w();
    void print_w();
    void drawLayout();
    bool loadData();
    void drawDate();
    void drawWeather();
    void drawWeatherIcon();
    ~OswAppWeather() {};
    void drawRain( int x, int y );
    void drawSnow( int x, int y, int level);
    void drawCloud(int x, int y, uint32_t color = rgb888(255, 255, 255),float k=1);
    void drawSun(int x, int y, int radius = 15);
    void drawMoon(int x, int y, int radius = 15);
    void drawFog(int x, int y, int fog);
    void drawThunderBolt(int x, int y);
    void printLastUpdate();
    void drawTriangleThick(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t radius,uint16_t color);
    void drawRefreshIcon(uint16_t color);
    void printDate();
    void weatherRequest();
    void getDayList(int nUpdates = 24);
    int getNextDay();
    int getPrevDay();
    void _drawDroplet(int x, int y, uint32_t color = rgb888(255, 255, 255));
    Preferences pref;
    String api_key;
    String url;
    String state1;
    String location1;
    bool displayBufferDisabled = false;
    bool getFinish = false ;
    //TODO: store root certificate in NVS ?

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
    const char* rootCACertificate = "-----BEGIN CERTIFICATE-----" \
                                    "MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw"\
                                    "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh"\
                                    "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw"\
                                    "WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg"\
                                    "RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK"\
                                    "AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP"\
                                    "R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx"\
                                    "sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm"\
                                    "NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg"\
                                    "Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG"\
                                    "/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC"\
                                    "AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB"\
                                    "Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA"\
                                    "FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw"\
                                    "AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw"\
                                    "Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB"\
                                    "gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W"\
                                    "PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl"\
                                    "ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz"\
                                    "CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm"\
                                    "lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4"\
                                    "avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2"\
                                    "yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O"\
                                    "yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids"\
                                    "hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+"\
                                    "HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv"\
                                    "MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX"\
                                    "nLRbwHOoq7hHwg=="\
                                    "-----END CERTIFICATE-----";
  private:
    bool _request();
};
