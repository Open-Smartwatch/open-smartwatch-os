#pragma once
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
    void setColors(uint32_t cloudBright, uint32_t cloudDark, uint32_t thunderbolt, uint32_t sun);
    void getHal(OswHal* h);
  private:
    OswHal* hal;
    uint32_t cloudBrightColor = rgb888(255,255,255);
    uint32_t cloudDarkColor = rgb888(253,253,253);
    uint32_t thunderboltColor = rgb888(255, 222, 40);
    uint32_t sunColor = rgb888(255, 250, 1);
};



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
    //this certificate will expire the day 01/01/2029
    //source https://support.sectigo.com/articles/Knowledge/Sectigo-Intermediate-Certificates
    //https://comodoca.my.salesforce.com/sfc/p/#1N000002Ljih/a/3l000000VbAR/r5xd1JISSRVHeYpuB5yWee1XcPZeIVFHFx_iWXmT5M8
    const char* rootCACertificate = "-----BEGIN CERTIFICATE-----\n"
                                    "MIIFgTCCBGmgAwIBAgIQOXJEOvkit1HX02wQ3TE1lTANBgkqhkiG9w0BAQwFADB7\n"
                                    "MQswCQYDVQQGEwJHQjEbMBkGA1UECAwSR3JlYXRlciBNYW5jaGVzdGVyMRAwDgYD\n"
                                    "VQQHDAdTYWxmb3JkMRowGAYDVQQKDBFDb21vZG8gQ0EgTGltaXRlZDEhMB8GA1UE\n"
                                    "AwwYQUFBIENlcnRpZmljYXRlIFNlcnZpY2VzMB4XDTE5MDMxMjAwMDAwMFoXDTI4\n"
                                    "MTIzMTIzNTk1OVowgYgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpOZXcgSmVyc2V5\n"
                                    "MRQwEgYDVQQHEwtKZXJzZXkgQ2l0eTEeMBwGA1UEChMVVGhlIFVTRVJUUlVTVCBO\n"
                                    "ZXR3b3JrMS4wLAYDVQQDEyVVU0VSVHJ1c3QgUlNBIENlcnRpZmljYXRpb24gQXV0\n"
                                    "aG9yaXR5MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAgBJlFzYOw9sI\n"
                                    "s9CsVw127c0n00ytUINh4qogTQktZAnczomfzD2p7PbPwdzx07HWezcoEStH2jnG\n"
                                    "vDoZtF+mvX2do2NCtnbyqTsrkfjib9DsFiCQCT7i6HTJGLSR1GJk23+jBvGIGGqQ\n"
                                    "Ijy8/hPwhxR79uQfjtTkUcYRZ0YIUcuGFFQ/vDP+fmyc/xadGL1RjjWmp2bIcmfb\n"
                                    "IWax1Jt4A8BQOujM8Ny8nkz+rwWWNR9XWrf/zvk9tyy29lTdyOcSOk2uTIq3XJq0\n"
                                    "tyA9yn8iNK5+O2hmAUTnAU5GU5szYPeUvlM3kHND8zLDU+/bqv50TmnHa4xgk97E\n"
                                    "xwzf4TKuzJM7UXiVZ4vuPVb+DNBpDxsP8yUmazNt925H+nND5X4OpWaxKXwyhGNV\n"
                                    "icQNwZNUMBkTrNN9N6frXTpsNVzbQdcS2qlJC9/YgIoJk2KOtWbPJYjNhLixP6Q5\n"
                                    "D9kCnusSTJV882sFqV4Wg8y4Z+LoE53MW4LTTLPtW//e5XOsIzstAL81VXQJSdhJ\n"
                                    "WBp/kjbmUZIO8yZ9HE0XvMnsQybQv0FfQKlERPSZ51eHnlAfV1SoPv10Yy+xUGUJ\n"
                                    "5lhCLkMaTLTwJUdZ+gQek9QmRkpQgbLevni3/GcV4clXhB4PY9bpYrrWX1Uu6lzG\n"
                                    "KAgEJTm4Diup8kyXHAc/DVL17e8vgg8CAwEAAaOB8jCB7zAfBgNVHSMEGDAWgBSg\n"
                                    "EQojPpbxB+zirynvgqV/0DCktDAdBgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rID\n"
                                    "ZsswDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQFMAMBAf8wEQYDVR0gBAowCDAG\n"
                                    "BgRVHSAAMEMGA1UdHwQ8MDowOKA2oDSGMmh0dHA6Ly9jcmwuY29tb2RvY2EuY29t\n"
                                    "L0FBQUNlcnRpZmljYXRlU2VydmljZXMuY3JsMDQGCCsGAQUFBwEBBCgwJjAkBggr\n"
                                    "BgEFBQcwAYYYaHR0cDovL29jc3AuY29tb2RvY2EuY29tMA0GCSqGSIb3DQEBDAUA\n"
                                    "A4IBAQAYh1HcdCE9nIrgJ7cz0C7M7PDmy14R3iJvm3WOnnL+5Nb+qh+cli3vA0p+\n"
                                    "rvSNb3I8QzvAP+u431yqqcau8vzY7qN7Q/aGNnwU4M309z/+3ri0ivCRlv79Q2R+\n"
                                    "/czSAaF9ffgZGclCKxO/WIu6pKJmBHaIkU4MiRTOok3JMrO66BQavHHxW/BBC5gA\n"
                                    "CiIDEOUMsfnNkjcZ7Tvx5Dq2+UUTJnWvu6rvP3t3O9LEApE9GQDTF1w52z97GA1F\n"
                                    "zZOFli9d31kWTz9RvdVFGD/tSo7oBmF0Ixa1DVBzJ0RHfxBdiSprhTEUxOipakyA\n"
                                    "vGp4z7h/jnZymQyd/teRCBaho1+V\n"
                                    "-----END CERTIFICATE-----\n";
    bool _request();
};
