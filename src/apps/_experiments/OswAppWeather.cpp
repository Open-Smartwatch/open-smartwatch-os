#ifdef OSW_FEATURE_WEATHER
#include "apps/_experiments/OswAppWeather.h"
#include "apps/_experiments/OswAppWeatherEncoder.h"
#include "services/OswServiceTaskWiFi.h"
#include <services/OswServiceTasks.h>
#include <cstring>
#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>
#include "fonts/DS_DIGI12pt7b.h"
#ifndef OSW_EMULATOR
#include <HTTPClient.h>
#endif
#define OPENWEATHERMAP_URL "https://api.openweathermap.org/data/2.5/forecast?q="
#define URL_REQ OPENWEATHERMAP_URL OPENWEATHERMAP_CITY "," OPENWEATHERMAP_STATE_CODE "&appid=" OPENWEATHERMAP_APIKEY "&cnt=24"
#ifdef    OSW_EMULATOR
#include <iostream>
#include <fstream>
#endif
/*
    TODO:   multiple location support
            measurement unit conversion (?)
*/





OswAppWeather::WeatherDecoder::WeatherDecoder(const String& input_String) {
    if(input_String.length() <  16 || (input_String.length()%8)!= 0 ) {
        this->in_ok = false;
    }
    this->in_String = input_String;
    this->nUpdates = (this->in_String.length()-8)/8;
}

bool OswAppWeather::WeatherDecoder::strIsValid() {
    return in_ok;
}

time_t OswAppWeather::WeatherDecoder::getTime() {
    String time_str = this->in_String.substring(0,8);
    time_t t = this->_str2time(time_str);
    return t;
}

std::vector<OswAppWeather::weather_update_t> OswAppWeather::WeatherDecoder::getUpdates() {
    OswAppWeather::weather_update_t update;
    String update_str;
    std::vector<OswAppWeather::weather_update_t> updates;
    for (int i=0 ; i<nUpdates; i++) {
        update_str = this->in_String.substring(8 + (8 * i), 8 + (8 * i) + 8);
        update.temp = this->_str2temp(update_str.substring(0, 3));
        update.humidity = this->_str2hum(update_str.substring(3, 4));
        update.pressure = this->_str2pres(update_str.substring(4, 7));
        update.weather = this->_str2wthr(update_str.substring(7, 8));
        updates.push_back(update);
    }
    return updates;
}

time_t OswAppWeather::WeatherDecoder::_str2time(const String& t) {
    int time = t.toInt();
    time = time * 8;
    time = 2147483647 - time;
    return time;
}

int OswAppWeather::WeatherDecoder::_str2temp(const String& temp) {
    int temp_int = temp.toInt();
    return temp_int;
}

int OswAppWeather::WeatherDecoder::_str2hum(const String& humidity) {
    int hum = humidity.toInt();
    hum = (hum*10)+ 5;
    return hum;
}

int OswAppWeather::WeatherDecoder::_str2pres(const String& pressure) {
    int press = pressure.toInt();
    press = press + 850;
    return press;
}

int OswAppWeather::WeatherDecoder::_str2wthr(const String& weather) {
    char wthr = weather[0];
    return wthr - 65;
}

OswAppWeather::WeatherParser::WeatherParser() {}

std::optional<String> OswAppWeather::WeatherParser::encodeWeather(DynamicJsonDocument& doc) {
    const char* code = nullptr;
    code = doc["cod"];
    if(strcmp("200",code)) {
        if(code==nullptr) {
            OSW_LOG_E("Error, corrupted API response.");
            return std::nullopt; // ERROR_CORRUPTED_RESPONSE
        } else {
            OSW_LOG_E("Error, response code: ", code);
            return std::nullopt; // ERROR_API_RESPONSE
        }
    }
    cnt = doc["cnt"];
    time_t time = doc["list"][0]["dt"];
    OswAppWeatherEncoder encoder;
    encoder.setTimestamp(time);
    bool res;
    for(int i=0; i<cnt; i++) {
        OswAppWeather::weather_update_t update;
        int temp = doc["list"][i]["main"]["temp"];
        update.temp = temp - 273;
        update.humidity = doc["list"][i]["main"]["humidity"];
        int pressure = doc["list"][i]["main"]["pressure"];
        update.pressure = pressure;
        update.weather = this->_getWCond(doc["list"][i]["weather"][0]["id"]);
        res = encoder.setUpdate(update);
        if (!res) {
            OSW_LOG_E("Failed to pass update to encoder!");
            return "ERROR_INPUT";
        }
    }
    return encoder.getEncoded();
}

int OswAppWeather::WeatherParser::_getWCond(int weather_code) {
    for(int i=0; i<15; i++) {
        for(size_t j=0; j < weather_conditions[i].size(); j++) {
            if(weather_code == weather_conditions[i][j]) {
                return i;
            }
        }
    }
    return 15; // unknown weather def
}

void OswAppWeather::drawPopUp() {
    this->hal->gfx()->drawThickLine(50,120,190,120,15,rgb888(255,255,255),true);
    this->hal->gfx()->drawThickLine(51,120,189,120,14,rgb888(0,0,0),true);
    this->hal->gfx()->setTextCursor(120,120);
    this->hal->gfx()->setTextColor(rgb888(255,255,255));
    this->hal->gfx()->setTextCenterAligned();
    this->hal->gfx()->setTextMiddleAligned();
    this->hal->gfx()->print("connecting...");
}


void OswAppWeather::drawWeather() {
    if(this->updtSelector >= this->forecast.size()) {
        return;
    }

    updtTime = initTimestamp + (this->updtSelector * 10800 );
    this->printWIcon.drawIcon(this->forecast[this->updtSelector].weather,120,45,1);
    //draw time of current weather updatea
    this->hal->gfx()->setFont(&DS_DIGI12pt7b);
    this->hal->gfx()->setTextSize(1);
    this->hal->gfx()->setTextMiddleAligned();
    this->hal->gfx()->setTextCenterAligned();
    this->hal->gfx()->setTextCursor(200, 119);
    strftime(this->timeUpdt,sizeof(this->timeUpdt),"%H:%M",localtime(&updtTime));
    this->hal->gfx()->print(this->timeUpdt);
    if(this->mainSelector==1) {
        this->hal->gfx()->drawThickLine(170,132,228,132,2,rgb565(164, 35, 52));
    }
    //weather data
    sprintf(this->buffer,"t:%2d",this->forecast[this->updtSelector].temp);
    this->hal->gfx()->setTextCursor(120, 90);
    this->hal->gfx()->print(buffer);

    sprintf(this->buffer,"H:%d%%",this->forecast[this->updtSelector].humidity);
    this->hal->gfx()->setTextCursor(120, 119);
    this->hal->gfx()->print(buffer);

    sprintf(this->buffer, "p:%d",this->forecast[this->updtSelector].pressure);
    this->hal->gfx()->setTextCursor(120, 148);
    this->hal->gfx()->print(buffer);
}

void OswAppWeather::drawRefreshIcon(uint16_t color) {
    this->hal->gfx()->fillCircle(185,152,12, color);
    this->hal->gfx()->fillCircle(185,152,8,rgb565(0,0,0));
    this->hal->gfx()->drawThickLine(185,157,200,157,2,rgb565(0,0,0));
    this->drawTriangleThick(194,154, 199,154, 199,149,2, color);
}


void OswAppWeather::drawLayout() {
    this->hal->gfx()->setFont(nullptr);
    this->hal->gfx()->setTextCursor(120, 215);
    this->hal->gfx()->setTextMiddleAligned();
    this->hal->gfx()->setTextCenterAligned();
    this->hal->gfx()->print("updated:");
    if(this->mainSelector!=2) {
        //up
        this->drawTriangleThick(180,50, 190,40, 200,50,4,rgb888(255,255,255));
        //down
        this->drawTriangleThick(180,190, 190,200, 200,190,4,rgb888(255,255,255));
    } else {
        this->hal->gfx()->setFont(nullptr);
        this->hal->gfx()->setTextCursor(180,50);
        this->hal->gfx()->print("update");
    }
    //<-
    this->drawTriangleThick(37,190, 45,198, 45,182,4,rgb565(100,100,100));
    //->
    this->drawTriangleThick(68,190, 60,198, 60,182,4,rgb888(255,255,255));
    //separator
    this->hal->gfx()->drawThickLine(78, 60,78,168,2,rgb565(164, 35, 52));
    if(this->mainSelector!=2) {
        this->drawRefreshIcon(rgb565(255,255,255));
    } else {
        this->drawRefreshIcon(rgb565(164, 35, 52));
    }
}

void OswAppWeather::drawTriangleThick(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t radius,uint16_t color) {
    this->hal->gfx()->drawThickLine(x0, y0, x1, y1, radius, color);
    this->hal->gfx()->drawThickLine(x1, y1, x2, y2, radius, color);
    this->hal->gfx()->drawThickLine(x2, y2, x0, y0, radius, color);
}

void OswAppWeather::printLastUpdate() {
    this->hal->gfx()->setFont(nullptr);
    this->hal->gfx()->setTextCursor(120, 225);
    this->hal->gfx()->print(this->initTimeDMY);
}
#ifndef OSW_EMULATOR
void OswAppWeather::weatherRequest() {
    if(!OswServiceAllTasks::wifi.isConnected()) {
        OswServiceAllTasks::wifi.enableWiFi();
        OswServiceAllTasks::wifi.connectWiFi();
    }
    this->requestMode = true;
}

bool OswAppWeather::_request() {
    // TODO cleanup the error handling (especially the return false; statements)
    HTTPClient http;
    int code = 0;
    OSW_LOG_D("Request: ");
    OSW_LOG_D(this->url);
    http.begin(this->url, OPENWEATHERMAP_CA_CERT);
    if (OswServiceAllTasks::wifi.isConnected()) {
        OswHal::getInstance()->disableDisplayBuffer();
        this->forecast.clear();
        this->dataLoaded = false;
        OSW_LOG_D("free heap ", ESP.getFreeHeap());
        code = http.GET();
    } else {
        return false;
    }
    http.end();
    OswServiceAllTasks::wifi.disconnectWiFi();
    OSW_LOG_D("Request returned code: ", code);
    if (code != HTTP_CODE_OK) {
        OSW_LOG_E("Unexpected API response: ", code);
        this->dataLoaded = false;
        return false;
    }
    DynamicJsonDocument doc(16432);
    deserializeJson(doc,http.getStream());
    WeatherParser pars;
    std::optional<String> encoded = pars.encodeWeather(doc);
    if(!encoded.has_value()) {
        OSW_LOG_E("Something went wrong with the encoding of the weather data?!");
        this->dataLoaded = false;
        return false;
    }
    int encoded_len = encoded.value().length();
    char encoded_arr[encoded_len + 1];
    strcpy(encoded_arr, encoded.value().c_str());
    String encoded_S = String(encoded_arr);
    if (!this->pref.putString("wtr",encoded_S)) {
        OSW_LOG_E("Unable to write weather-update to NVS?!");
        this->dataLoaded = false;
        return false;
    }
    this->requestMode=false;
    bool res = this->loadData();
    if (!res) {
        this->dataLoaded = false;
        return false;
    }
    OSW_LOG_D("weather updated correctly");
    this->dataLoaded=true;
    return true;
}
#else
void OswAppWeather::weatherRequest() {
    this->requestMode = true;
}
bool OswAppWeather::_request() {
    // in emulator instantly "fullfill" the request
    this->requestMode=false;
    this->dataLoaded=true;
    return true;
}
#endif


void OswAppWeather::getDayList(int nUpdates) {
    time_t timestamp = this->initTimestamp;
    tm* time_current;
    int mday_prev;
    char date_buff[6];
    time_current = localtime(&this->initTimestamp);
    mday_prev = time_current->tm_mday;
    strftime(date_buff, sizeof(date_buff), "%d/%m", localtime(&this->initTimestamp));
    this->dayFirstUpdt.push_back(0);
    for(int i=1; i<24; i++) {
        timestamp = timestamp + (3600*3);
        mday_prev = time_current->tm_mday;
        time_current = localtime(&timestamp);
        if (time_current->tm_mday != mday_prev) {
            strftime(date_buff, sizeof(date_buff), "%d/%m", localtime(&timestamp));
            dayFirstUpdt.push_back(i);
        }
    }
}

void OswAppWeather::printDate() {
    time_t time;
    char date_buf[6];
    this->hal->gfx()->fillCircle(4,120,3,rgb888(255,255, 255));
    if(this->dayFirstUpdt.size()!=0) {
        if(this->updtSelector < this->dayFirstUpdt[1]) {
            this->hal->gfx()->setFont(nullptr);
            this->hal->gfx()->setTextCursor(40, 91);
            this->hal->gfx()->print("---");
        } else {
            this->hal->gfx()->setFont(&DS_DIGI12pt7b);
            this->hal->gfx()->setTextCursor(40, 91);
            time = this->updtTime - 86400;
            strftime(date_buf, sizeof(date_buf), "%d/%m", localtime(&time));
            this->hal->gfx()->print(date_buf);
        }

        if(this->updtSelector >= this->dayFirstUpdt[this->dayFirstUpdt.size()-1]) {
            this->hal->gfx()->setFont(nullptr);
            this->hal->gfx()->setTextCursor(40, 149);
            this->hal->gfx()->print("---");
        } else {
            this->hal->gfx()->setFont(&DS_DIGI12pt7b);
            time = this->updtTime + 86400;
            this->hal->gfx()->setTextCursor(40, 149);
            strftime(date_buf, sizeof(date_buf), "%d/%m", localtime(&time));
            this->hal->gfx()->print(date_buf);
        }
        if(this->dayFirstUpdt.size()!=0) {
            this->hal->gfx()->setFont(&DS_DIGI12pt7b);
            this->hal->gfx()->setTextCursor(40, 120);
            time = this->updtTime;
            strftime(date_buf, sizeof(date_buf), "%d/%m", localtime(&time));
            this->hal->gfx()->print(date_buf);
        }
    } else {
        this->hal->gfx()->setFont(nullptr);
        this->hal->gfx()->setTextCursor(40, 120);
        this->hal->gfx()->print("no data");
    }

    this->hal->gfx()->setFont(&DS_DIGI12pt7b);

    if(this->mainSelector==0) {
        this->hal->gfx()->drawThickLine(10,132,68,132,2,rgb565(164, 35, 52));
    }
}

bool OswAppWeather::loadData() {
    String wstr;
#ifdef OSW_EMULATOR
    std::ifstream inFile;
    inFile.open("file_weather.json"); //open the input file
    if(!inFile.is_open()) {
        OSW_LOG_E("Unable to open 'file_weather.json' in the current working directory");
        OSW_LOG_W("→ Because the emulator has no HTTP-capabilities (yet), you need to provide this file manually for now.");
    } else {
        std::stringstream strStream;
        strStream << inFile.rdbuf();
        std::string strW = strStream.str();
        OSW_LOG_D("json file raw:");
        OSW_LOG_D(strW);
        DynamicJsonDocument doc(16432*2);// when in emulator more space is needed
        deserializeJson(doc,strW);
        WeatherParser pars;
        std::optional<String> encoded = pars.encodeWeather(doc);
        if(encoded.has_value()) {
            OSW_LOG_D("as encoded:");
            OSW_LOG_D(encoded.value());
            wstr += encoded.value(); // this copies the value of "encoded" to "wstr" (just assignment does not take ownership of the value itself)
        } else {
            OSW_LOG_E("Something went wrong with the encoding of the weather data from the local file?!");
        }
    }
#else
    wstr = this->pref.getString("wtr");
#endif
    if (wstr!="") {
        OSW_LOG_D("size of wstr: ", wstr.length());
        if( (wstr.length() % 8) != 0 ) {
            this->dataLoaded = false;
            return false;
        }
        if( wstr.length()<16) {
            this->dataLoaded = false;
            return false;
        }
        WeatherDecoder decoder(wstr.c_str());
        this->initTimestamp = decoder.getTime();
        this->getDayList();
        if(!strftime(this->initTimeDMY, sizeof(this->initTimeDMY), "%d/%m/%Y", localtime(&this->initTimestamp))) {
            OSW_LOG_W("Failed to parse timestamp: ", this->initTimeDMY);
        }
        if(!strftime(this->initTimeMD, sizeof(this->initTimeMD), "%d/%m", localtime(&this->initTimestamp))) {
            OSW_LOG_W("Failed to parse timestamp: ", this->initTimeMD);
        }
        tmInit = localtime(&this->initTimestamp);
        forecast = decoder.getUpdates();
        this->dataLoaded = true;
        this->updtSelector = 0;
        return true;
    } else {
        this->dataLoaded = false;
        return false;
    }
}

int OswAppWeather::getNextDay() {
    for(size_t i=0; i<this->dayFirstUpdt.size(); i++) {
        if(this->dayFirstUpdt[i] > this->updtSelector) {
            return this->dayFirstUpdt[i];
        }
    }
    return 0;
}

int OswAppWeather::getPrevDay() {
    for(int i=this->dayFirstUpdt.size()-1; i>=0; i--) {
        if(this->dayFirstUpdt[i] < this->updtSelector) {
            return this->dayFirstUpdt[i];
        }
    }
    return 0;
}


void OswAppWeather::setup() {
    OSW_LOG_D("OSW Weather Setup ");
    this->location1 = OswConfigAllKeys::weatherLocation1.get();
    this->state1 = OswConfigAllKeys::weatherState1.get();
    this->api_key = OswConfigAllKeys::weatherApiKey.get();
    this->url = String(OPENWEATHERMAP_URL);
    this->url = this->url + this->location1;
    this->url = this->url + ",";
    this->url = this->url + this->state1;
    this->url = this->url + "&appid=";
    this->url = this->url + this->api_key;
    this->url = this->url + "&cnt=24";
    pref.begin("wheater-app", false);
    this->loadData();
    this->hal = OswHal::getInstance(); // ALWAYS update cached hal reference, as it may change at any point (strictly speaking, it should be updated in every loop, but this is a good place to start with)
    // ↑ this is fixed in OswAppV2 where you always have the up-to-date hal reference available
    this->printWIcon.getHal(this->hal);
    OSW_LOG_D("Setup end");
}

void OswAppWeather::loop() {

    this->drawLayout();
    if(this->dataLoaded) {
        this->drawWeather();
        this->printDate();
        this->printLastUpdate();
    }

#ifndef OSW_EMULATOR
    if(this->requestMode) {
        if (OswServiceAllTasks::wifi.isConnected()) {
            this->_request();
        } else {
            this->drawPopUp();
        }
    }
#else
    if(this->requestMode) {
        this->_request();
        this->drawPopUp();
    }
#endif
    if (hal->btnHasGoneDown(BUTTON_2)) {
        if(this->mainSelector==1) { // next update
            if(this->updtSelector<23) {
                this->updtSelector++;
            }
        }
        if(this->mainSelector==0) { //next day
            if(this->dayFirstUpdt.size() > 0 and this->updtSelector >= this->dayFirstUpdt[this->dayFirstUpdt.size()-1]) {
                this->updtSelector=this->updtSelector;
            } else {
                this->updtSelector = this->getNextDay();
            }
        }
    }
    if (hal->btnHasGoneDown(BUTTON_3)) {

        if(this->mainSelector==1) {
            if(this->updtSelector>0) {
                this->updtSelector--;
            }
        }
        if(this->mainSelector==0) {
            if(this->updtSelector == 0) {
                this->updtSelector=this->updtSelector;
            } else {
                this->updtSelector = this->getPrevDay();
            }
        }
        if(this->mainSelector==2) {
            this->weatherRequest();
        }
    }
    if( hal->btnHasGoneDown(BUTTON_1)) {
        if(this->mainSelector!=2) {
            this->mainSelector++;
        } else {
            this->mainSelector=0;
        }
    }

}

void OswAppWeather::stop() {

}
#endif
