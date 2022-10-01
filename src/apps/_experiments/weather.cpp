#ifndef OSW_EMULATOR
#ifdef OSW_FEATURE_WEATHER
#include "./apps/_experiments/weather.h"
#include "./services/OswServiceTaskWiFi.h"
#include <services/OswServiceTasks.h>
#include <HTTPClient.h>
#include <cstring>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include "./fonts/DS_DIGI12pt7b.h"
#include "./ArduinoJson.h"

#define OPENWEATHERMAP_APIKEY "5643586bde5db6443716d934ced6c66a" // change key
#define OPENWEATHERMAP_CITY "turin"
#define OPENWEATHERMAP_STATE_CODE "IT"

#define OPENWEATHERMAP_URL "https://api.openweathermap.org/data/2.5/forecast?"
#define URL_REQ OPENWEATHERMAP_URL "q=" OPENWEATHERMAP_CITY "," OPENWEATHERMAP_STATE_CODE "&appid=" OPENWEATHERMAP_APIKEY "&cnt=24"
/*
    TODO:   multiple location support
            Weather icons class available for all the apps (?)
            measurement unit conversion (?)
            
*/

class WeatherEncoder {
    public:
      WeatherEncoder();
      bool setUpdate(weather_update_t update);
      bool setTimestamp(time_t t);
      string getEncoded();

    private:
      string _time2str(time_t time);
      string _temp2str(int temp);
      string _hum2str(int humidity);
      string _pres2str(int pressure);
      string _wthr2str(int weather);
      bool time_loaded = false;
      time_t timestamp;
      string updates;
};

WeatherEncoder::WeatherEncoder(){}

bool WeatherEncoder::setUpdate(weather_update_t update){
    bool update_ok = true;
    if(update.temp > 99 || update.temp < -99 ){update_ok = false;}
    if(update.humidity > 100 || update.humidity < 0) {update_ok = false;}
    if(update.pressure < 0 || update.pressure > 2000 ) {update_ok = false;}
    if(update.weather < 0 || update.weather > 15) {update_ok = false;}
    if(!update_ok){return false;}
    string update_s;
    update_s.append(_temp2str(update.temp));
    update_s.append(_hum2str(update.humidity));
    update_s.append(_pres2str(update.pressure));
    update_s.append(_wthr2str(update.weather));
    this->updates.append(update_s);
    return true;

}

bool WeatherEncoder::setTimestamp(time_t t){
  if(t > 0 && t < 2147483647 ){
    this->timestamp = t;
    this->time_loaded = true;
    return true;
  }
  return false;
}

string WeatherEncoder::getEncoded(){
  if(this->time_loaded){
    string encoded;
    encoded.append(_time2str(this->timestamp));
    encoded.append(this->updates);
    return encoded;
  }else{
    return "error_no_timestamp";
  }
}

string WeatherEncoder::_time2str(time_t time){
    time =  2147483647 - time ; // time = seconds to end of the epoch 01/19/2038 3:14 AM 
    time = time / 8 ;
    char time_dgts[9];
    sprintf(time_dgts,"%08ld",time);
    return time_dgts;
  }

string WeatherEncoder::_temp2str(int temp){
  string temp_s;
  if (temp > 0){
    temp_s.append("+");
  }else{
    temp_s.append("-");
  }
  char temp_dgts[3];
  sprintf(temp_dgts,"%02d",abs(temp));
  temp_s.append(temp_dgts);
  return temp_s;
}

string WeatherEncoder::_hum2str(int humidity){
  if(humidity == 100){humidity = 99;}
  char h[2];
  sprintf(h,"%d",humidity/10);
  return h;
}

string WeatherEncoder::_pres2str(int pressure){
  pressure = pressure - 850; 
  if(pressure < 0){pressure = 0;}
  if(pressure > 999){pressure = 999;}
  string pres_s;
  char pres_dgts[3];
  sprintf(pres_dgts,"%03d",abs(pressure));
  pres_s.append(pres_dgts);
  return pres_s;
}

string WeatherEncoder::_wthr2str(int weather){
  char w[2];
  sprintf(w,"%c",weather+65);
  return w;
}



class WeatherDecoder{
  public:
    WeatherDecoder(string input_string);
    bool strIsValid();
    time_t getTime();
    vector<weather_update_t> getUpdates();
  private:
    time_t _str2time(string t);
    int _str2temp(string temp);
    int _str2hum(string humidity);
    int _str2pres(string pressure);
    int _str2wthr(string weather);
    bool in_ok = true;
    int n_updates = 0;
    string in_string;
};

WeatherDecoder::WeatherDecoder(string input_string){
  //TODO: more accurate input ctrl
  if(input_string.length() <  16 || (input_string.length()%8)!= 0 ){
    this->in_ok = false;
  }
  this->in_string = input_string;
  this->n_updates = (this->in_string.length()-8)/8;
}

bool WeatherDecoder::strIsValid(){
  return in_ok;
}

time_t WeatherDecoder::getTime(){
  string time_str = this->in_string.substr(0,8);
  time_t t = this->_str2time(time_str);
  return t;
}

vector<weather_update_t> WeatherDecoder::getUpdates(){
  weather_update_t update;
  string update_str;
  vector<weather_update_t> updates;
  for (int i=0 ; i<n_updates; i++){
    update_str = this->in_string.substr(8 + (8*i), 8);
    update.temp = this->_str2temp(update_str.substr(0,3));
    update.humidity = this->_str2hum(update_str.substr(3,1));
    update.pressure = this->_str2pres(update_str.substr(4,3));
    update.weather = this->_str2wthr(update_str.substr(7,1));
    updates.push_back(update);
  }
  return updates;
}

time_t WeatherDecoder::_str2time(string t){
  int time = stoi(t);
  time = time * 8;
  time = 2147483647 - time;
  return time;
}

int WeatherDecoder::_str2temp(string temp){
  int temp_int = stoi(temp);
  return temp_int;
}

int WeatherDecoder::_str2hum(string humidity){
  int hum = (stoi(humidity)*10) + 5;
  return hum;
}

int WeatherDecoder::_str2pres(string pressure){
  int pres = stoi(pressure) + 850;
  return pres;
}

int WeatherDecoder::_str2wthr(string weather){
  char wthr = weather[0];
  return wthr - 65;
}



class WeatherParser{
  public:
    WeatherParser();
    string encodeWeather(DynamicJsonDocument& doc);
  private:
    int _getWCond(int weather_code);
    int cnt;
    vector<weather_update_t> updates;
    vector<int> clear_code{800};//0
    vector<int>clouds_min{801};//1
    vector<int>clouds_med{802};//2
    vector<int>clouds_high{803, 804};//3
    vector<int>mist{701};//4
    vector<int>fog{741};//5
    vector<int>snow_min{611, 612, 615, 616};//6
    vector<int>snow_med{600, 613, 601, 620};//7
    vector<int>snow_high{602, 621, 622};//8
    vector<int>rain_min{500, 300, 301, 302, 310, 311, 312, 313, 314, 321};//9
    vector<int>rain_med{502, 501};//10
    vector<int>rain_high{503, 504, 511, 520, 521, 522, 531};//11
    vector<int>thunderstorm{200, 201, 210, 211, 231, 230};//12
    vector<int>thunderstorm_heavy{202, 212, 221, 232};//13
    vector<int>squall_tornado{771, 781};//14
    //15 ->unknown
    vector<vector<int>>weather_conditions{clear_code, clouds_min, clouds_med, clouds_high, mist, fog, snow_min, snow_med, 
                                          snow_high, rain_min, rain_med, rain_high, thunderstorm, 
                                          thunderstorm_heavy, squall_tornado };
};

WeatherParser::WeatherParser(){}

string WeatherParser::encodeWeather(DynamicJsonDocument& doc){
  const char* code = nullptr;
  code = doc["cod"];
  if(strcmp("200",code)){
    if(code==nullptr){
      Serial.println("Error, corrupted API response.");
      return  "ERROR_CORRUPTED_RESPONSE";
    }else{
      Serial.print("Error, response code: ");
      Serial.println(code);
      return "ERROR_API_RESPONSE";
    }
  }
  cnt = doc["cnt"];
  time_t time = doc["list"][0]["dt"];
  WeatherEncoder encoder;
  encoder.setTimestamp(time);
  bool res;
  for(int i=0; i<cnt; i++){
    weather_update_t update;
    int temp = doc["list"][i]["main"]["temp"];
    update.temp = temp - 273;
    update.humidity = doc["list"][i]["main"]["humidity"];
    int pressure = doc["list"][i]["main"]["pressure"];
    update.pressure = pressure;
    update.weather = this->_getWCond(doc["list"][i]["weather"][0]["id"]);
    res = encoder.setUpdate(update);
    if (!res){
      return "ERROR_INPUT";
    }
  }
  return encoder.getEncoded();
}

int WeatherParser::_getWCond(int weather_code){
  for(int i=0; i<15; i++){
    for(int j=0; j < weather_conditions[i].size(); j++){
      if(weather_code == weather_conditions[i][j]){
        return i;
      }
    }
  }
  return 15; // unknown weather def 
}




void OswAppWeather::drawSun(int x, int y, int radius ){
  this->hal->gfx()->fillCircle(x, y, radius ,rgb888(255, 250, 1));
}


void OswAppWeather::drawMoon(int x, int y, int radius ){
    this->hal->gfx()->fillCircle(x, y, radius ,rgb888(255, 255, 255));
}
/*draw a cloud given: x coordinate and y coordinate.
  color: rgb888 color
  k: size coef. NOTE: if k is > 1 cloud may be partially out of the screen
*/
void OswAppWeather::drawCloud(int x, int y, uint32_t color, float k ){
    x = x+22;
    y=y+15;
    float radius = 15*k;
    float k1=12*k;
    float j1=5*k;
    float k2=6*k;
    float j2=1*k;
    float k3=14*k;
   
    this->hal->gfx()->fillCircle(x, y, radius ,color);

    this->hal->gfx()->fillCircle(x-k1, y+j1, radius-j1  ,color);

    this->hal->gfx()->fillCircle(x-k2, y-j2, radius-j1 ,color);

    this->hal->gfx()->fillCircle(x+k3, y+j1, radius-j1 ,color);
}

void OswAppWeather::_drawDroplet(int x, int y, uint32_t color){
  this->hal->gfx()->drawLine(x, y, x+5, y+5, color);
  this->hal->gfx()->drawLine(x+1, y, x+6, y+5, color);
  this->hal->gfx()->drawLine(x+2, y, x+7, y+5, color);
}

void OswAppWeather::drawRain(int x, int y ){
    this->_drawDroplet(x,y);
    this->_drawDroplet(x+10,y+4);
    this->_drawDroplet(x+15, y);
}

void OswAppWeather::drawSnow(int x, int y, int level){
  switch (level)
  {
  case 1:
    this->hal->gfx()->fillCircle(x+10,y+3,3,rgb888(255,255 , 255));
    break;
  case 2:
    this->hal->gfx()->fillCircle(x,y+2,3,rgb888(255,255 , 255));
    this->hal->gfx()->fillCircle(x+18,y+2,3,rgb888(255,255 , 255)); 
    break;
  case 3:
    this->hal->gfx()->fillCircle(x,y,3,rgb888(255,255 , 255));
    this->hal->gfx()->fillCircle(x+10,y+5,3,rgb888(255,255 , 255));
    this->hal->gfx()->fillCircle(x+18,y,3,rgb888(255,255 , 255));
  default:
    break;
  }
}

void OswAppWeather::drawThunderBolt(int x, int y)
{
    x = x - 10;
    y = y - 3;
    this->hal->gfx()->drawLine(x, y, x + 3, y + 5, rgb888(255, 222, 40));
    this->hal->gfx()->drawLine(x + 1, y, x + 4, y + 5, rgb888(255, 222, 40));
    this->hal->gfx()->drawLine(x + 2, y, x + 5, y + 5, rgb888(255, 222, 40));

    this->hal->gfx()->drawLine(x + 4, y + 4, x + 7, y + 8, rgb888(255, 222, 40));
    this->hal->gfx()->drawLine(x + 5, y + 4, x + 8, y + 9, rgb888(255, 222, 40));
    this->hal->gfx()->drawLine(x + 6, y + 4, x + 9, y + 10, rgb888(255, 222, 40));
}

void OswAppWeather::drawFog(int x, int y, int fog)
{
    this->hal->gfx()->drawHLine(x, y, 22, rgb888(255, 255, 255));
    this->hal->gfx()->drawHLine(x, y + 1, 22, rgb888(255, 255, 255));
    this->hal->gfx()->drawHLine(x, y + 2, 22, rgb888(255, 255, 255));

    this->hal->gfx()->drawHLine(x, y + 5, 22, rgb888(255, 255, 255));
    this->hal->gfx()->drawHLine(x, y + 6, 22, rgb888(255, 255, 255));
    this->hal->gfx()->drawHLine(x, y + 7, 22, rgb888(255, 255, 255));
    if (fog > 1)
    {
        this->hal->gfx()->drawHLine(x, y + 10, 22, rgb888(255, 255, 255));
        this->hal->gfx()->drawHLine(x, y + 11, 22, rgb888(255, 255, 255));
        this->hal->gfx()->drawHLine(x, y + 12, 22, rgb888(255, 255, 255));
    }
}

void OswAppWeather::drawWeatherIcon(){
    int x = 120;
    int y = 45;
    switch (this->forecast[this->updtSelector].weather)
   {
  case 0: // sun
    this->drawSun(x,y);
    break;
  case 1: //clouds min
    this->drawSun(x-15, y-5);
    this->drawCloud(x-22,y-15);
    break;
  case 2: //clouds medium
    this->drawCloud(x-22,y-15);
    break;
  case 3: //heavy clouds
    this->drawCloud(x-22,y-15,rgb888(253,253,253),1);
    this->drawCloud(x-12,y-15);
    break;
  case 4://mist
    this->drawFog(x-10,y,0);
    break;
  case 5://fog
    this->drawFog(x-10,y,1);
    break;
  case 6: //snow min
    this->drawSnow(x-10,y+20,1);
    this->drawCloud(x-22,y-15);
    break;
  case 7: //snow med
    this->drawSnow(x-10,y+20,2);
    this->drawCloud(x-22,y-15);
    break;
  case 8: //snow heavy
    this->drawSnow(x-10,y+20,3);
    this->drawCloud(x-22,y-15);
    break;
  case 9: //rain min
    this->drawRain(x-5,y+20);
    this->drawSun(x-15, y-5);
    this->drawCloud(x-22,y-15);
    break;
  case 10: //rain med
    this->drawCloud(x-22,y-15);
    this->drawRain(x-5,y+20);
    break;
  case 11: //rain heavy
    this->drawCloud(x-22,y-15,rgb888(253,253,253),1);
    this->drawCloud(x-12,y-15);
    this->drawRain(x-5,y+20);
    break;
  case 12: //thunderstorm 1
    this->drawCloud(x-22,y-15);
    //this->drawRain(x,y+20);
    this->drawThunderBolt(x+5,y+20);
    break;
  case 13: //thunderstorm 1
    this->drawThunderBolt(x+5,y+20);
    this->drawThunderBolt(x+20,y+18);
    this->drawCloud(x-22,y-15,rgb888(253,253,253),1);
    this->drawCloud(x-12,y-15);
    break;
  case 14:
    this->hal->gfx()->setTextCursor(120,45);
    this->hal->gfx()->print("!Danger!");
    break;
  case 15: //unknown
    this->hal->gfx()->setTextCursor(120,45);
    this->hal->gfx()->print("?");

  default:
    break;
  }
}


void OswAppWeather::drawWeather(){
    updtTime = initTimestamp + (this->updtSelector * 10800 );
    this->drawWeatherIcon();
    //draw time of current weather updatea
    this->hal->gfx()->setFont(&DS_DIGI12pt7b);
    this->hal->gfx()->setTextSize(1);
    this->hal->gfx()->setTextMiddleAligned();
    this->hal->gfx()->setTextCenterAligned();
    this->hal->gfx()->setTextCursor(200 , 119);
    strftime(this->timeUpdt,sizeof(this->timeUpdt),"%H:%M",localtime(&updtTime));
    this->hal->gfx()->print(this->timeUpdt);
    if(this->mainSelector==1){
        this->hal->gfx()->drawThickLine(170,132,228,132,2,rgb565(164, 35, 52));
    }
    //time struct to get the day associated to the incremented timestamp updtTime
    tm* tm_1;
    tm* tm_2;
    tm_1 = localtime(&updtTime);
    time_t time = this->initTimestamp;
    time = time + 86400;
    tm_2 = localtime(&time);
    time = time + 86400;
    tm_2 = localtime(&time);
    //weather data 
    sprintf(this->buffer,"t:%2d",this->forecast[this->updtSelector].temp);
    this->hal->gfx()->setTextCursor(120 , 90);
    this->hal->gfx()->print(buffer);

    sprintf(this->buffer,"H:%d%%",this->forecast[this->updtSelector].humidity);
    this->hal->gfx()->setTextCursor(120 , 119);
    this->hal->gfx()->print(buffer);

    sprintf(this->buffer, "p:%d",this->forecast[this->updtSelector].pressure);
    this->hal->gfx()->setTextCursor(120 , 148);
    this->hal->gfx()->print(buffer);
}

void OswAppWeather::drawRefreshIcon(uint16_t color){
    this->hal->gfx()->fillCircle(185,152,12, color);
    this->hal->gfx()->fillCircle(185,152,8,rgb565(0,0,0));
    this->hal->gfx()->drawThickLine(185,157,200,157,2,rgb565(0,0,0));
    this->drawTriangleThick(194,154, 199,154, 199,149,2, color);
}


void OswAppWeather::drawLayout(){
    this->hal->gfx()->setFont(nullptr);
    this->hal->gfx()->setTextCursor(120 , 215);
    this->hal->gfx()->setTextMiddleAligned();
    this->hal->gfx()->setTextCenterAligned();
    this->hal->gfx()->print("last sync:");
    if(this->mainSelector!=2){
         //up
        this->drawTriangleThick(180,50, 190,40, 200,50,4,rgb888(255,255,255));
        //down
        this->drawTriangleThick(180,190, 190,200, 200,190,4,rgb888(255,255,255));
    }else{
        this->hal->gfx()->setFont(nullptr);
        this->hal->gfx()->setTextCursor(180,50);
        this->hal->gfx()->print("sync");
    }
    //<-
    this->drawTriangleThick(37,190, 45,198, 45,182,4,rgb565(100,100,100));
    //->
    this->drawTriangleThick(68,190, 60,198, 60,182,4,rgb888(255,255,255));
    //separator
    this->hal->gfx()->drawThickLine(78, 60,78,168,2,rgb565(164, 35, 52));
    if(this->mainSelector!=2){
        this->drawRefreshIcon(rgb565(255,255,255));
    }else{
        this->drawRefreshIcon(rgb565(164, 35, 52));

    }
}

void OswAppWeather::drawTriangleThick(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t radius,uint16_t color){
    this->hal->gfx()->drawThickLine(x0, y0, x1, y1, radius, color);
    this->hal->gfx()->drawThickLine(x1, y1, x2, y2, radius, color);
    this->hal->gfx()->drawThickLine(x2, y2, x0, y0, radius, color);
}


void OswAppWeather::printLastUpdate(){
    this->hal->gfx()->setFont(nullptr);  
    this->hal->gfx()->setTextCursor(120 , 225);
    this->hal->gfx()->print(this->initTimeDMY);
}

void OswAppWeather::weatherRequest(){
  if(!OswServiceAllTasks::wifi.isConnected()){
    OswServiceAllTasks::wifi.enableWiFi();
    OswServiceAllTasks::wifi.connectWiFi();  
  }
  this->requestMode = true;
}

bool OswAppWeather::_request(){
    WiFiClientSecure *client = new WiFiClientSecure;
    client->setCertificate(this->rootCACertificate);
    HTTPClient http;
    Serial.println(this->url);
    http.begin(this->url);
    int code = 0;
    if (OswServiceAllTasks::wifi.isConnected()) {
        OswHal::getInstance()->disableDisplayBuffer();
        this->forecast.clear();
        this->dataLoaded=false;
        Serial.println("free heap");
        Serial.println(ESP.getFreeHeap());
        code = http.GET();
    }else{
      return false;
    }
    http.end();
    delete client;
    OswServiceAllTasks::wifi.disconnectWiFi();
    Serial.println("code:");
    Serial.println(code);
    if (code == 200){
        DynamicJsonDocument doc(16432);
        deserializeJson(doc,http.getStream());
        WeatherParser pars;
        string encoded = pars.encodeWeather(doc);
        int encoded_len = encoded.length();
        char encoded_arr[encoded_len + 1];
        strcpy(encoded_arr, encoded.c_str());
        String encoded_S = String(encoded_arr);
        if (!this->pref.putString("wtr",encoded_S)){
          Serial.println("Error: unable to write to NVS");
          this->dataLoaded = false;
          return false;
        }
    }else{
        Serial.println("Error: API response: ");
        Serial.print(code);
        this->dataLoaded = false;
        return false;
    }
    this->requestMode=false;
    bool res = this->loadData();
    if (res){
      Serial.println("weather updated correctly");
      this->dataLoaded=true;
      return true;
    }else{
      this->dataLoaded = false;
      return false;
    }

}

  
void OswAppWeather::getDayList(int n_updates){
    time_t timestamp = this->initTimestamp;
    tm* time_current;
    int mday_prev;
    char date_buff[6];
    time_current = localtime(&this->initTimestamp);
    mday_prev = time_current->tm_mday;
    strftime(date_buff, sizeof(date_buff), "%d/%m", localtime(&this->initTimestamp));
    this->dayFirstUpdt.push_back(0);
    for(int i=1; i<24; i++){
        timestamp = timestamp + (3600*3);
        mday_prev = time_current->tm_mday;
        time_current = localtime(&timestamp);
        if (time_current->tm_mday != mday_prev){
            strftime(date_buff, sizeof(date_buff), "%d/%m", localtime(&timestamp));
            dayFirstUpdt.push_back(i);
        }
    }
}


void OswAppWeather::printDate(){
    time_t time;
    char date_buf[6];
    this->hal->gfx()->fillCircle(4,120,3,rgb888(255,255 , 255));
    if(this->dayFirstUpdt.size()!=0){
        if(this->updtSelector < this->dayFirstUpdt[1]){
            this->hal->gfx()->setFont(nullptr);
            this->hal->gfx()->setTextCursor(40 , 91);
            this->hal->gfx()->print("---");
        }else{
            this->hal->gfx()->setFont(&DS_DIGI12pt7b); 
            this->hal->gfx()->setTextCursor(40 , 91);
            time = this->updtTime - 86400;
            strftime(date_buf, sizeof(date_buf), "%d/%m", localtime(&time));
            this->hal->gfx()->print(date_buf);
        }
        
        if(this->updtSelector >= this->dayFirstUpdt[this->dayFirstUpdt.size()-1]){
            this->hal->gfx()->setFont(nullptr);
            this->hal->gfx()->setTextCursor(40 , 149);
            this->hal->gfx()->print("---");
        }else{
            this->hal->gfx()->setFont(&DS_DIGI12pt7b); 
            time = this->updtTime + 86400;
            this->hal->gfx()->setTextCursor(40 , 149);
            strftime(date_buf, sizeof(date_buf), "%d/%m", localtime(&time));
            this->hal->gfx()->print(date_buf);
        }
        if(this->dayFirstUpdt.size()!=0){
            this->hal->gfx()->setFont(&DS_DIGI12pt7b); 
            this->hal->gfx()->setTextCursor(40 , 120);
            time = this->updtTime;
            strftime(date_buf, sizeof(date_buf), "%d/%m", localtime(&time));
            this->hal->gfx()->print(date_buf);
        }
    }else{
        this->hal->gfx()->setFont(nullptr); 
        this->hal->gfx()->setTextCursor(40 , 120);
        this->hal->gfx()->print("no data");
    }
    
    this->hal->gfx()->setFont(&DS_DIGI12pt7b);  

    if(this->mainSelector==0){
        this->hal->gfx()->drawThickLine(10,132,68,132,2,rgb565(164, 35, 52));
    }
}

bool OswAppWeather::loadData(){ 
    String wstr = this->pref.getString("wtr");
    if (!wstr.equals("")){
      Serial.println("size of wstr: ");
      Serial.println(wstr.length());
      if( (wstr.length() % 8) != 0 ){this->dataLoaded = false; return false;}
      if( wstr.length()<16){this->dataLoaded = false; return false;}
      WeatherDecoder decoder(wstr.c_str());
      this->initTimestamp = decoder.getTime();
      tm* tmx;
      this->getDayList();
      tmx = localtime(&this->initTimestamp);
      if(strftime(this->initTimeDMY, sizeof(this->initTimeDMY), "%d/%m/%Y", localtime(&this->initTimestamp))){
          //Serial.println(this->inittimeDMY);
      }
      if(strftime(this->initTimeMD, sizeof(this->initTimeMD), "%d/%m", localtime(&this->initTimestamp))){
          //Serial.println(this->initTimeMD);
      }
      tmInit = localtime(&this->initTimestamp);
      forecast = decoder.getUpdates();
      this->dataLoaded = true;
      return true;
    }else{
      this->dataLoaded = false;
      return false;
    }
}

int OswAppWeather::getNextDay(){
    for(int i=0; i<this->dayFirstUpdt.size();i++){
        if(this->dayFirstUpdt[i] > this->updtSelector){
            return this->dayFirstUpdt[i];
        }
    }
    return 0;
}

int OswAppWeather::getPrevDay(){
    for(int i=this->dayFirstUpdt.size()-1; i>=0; i--){
        if(this->dayFirstUpdt[i] < this->updtSelector){
            return this->dayFirstUpdt[i];
        }
    }
    return 0;
}


void OswAppWeather::setup() {
  this->location1 = OswConfigAllKeys::location1.get();
  this->state1 = OswConfigAllKeys::state1.get();
  this->api_key = OswConfigAllKeys::api_key.get();
  this->url = String(OPENWEATHERMAP_URL);
  this->url.concat(this->location1);
  this->url.concat(",");
  this->url.concat(this->state1);
  this->url.concat("&appid=");
  this->url.concat(this->api_key);
  this->url.concat("&cnt=24");
  pref.begin("wheater-app");
  this->loadData();
}

void OswAppWeather::loop() {

    this->drawLayout();
    if(this->dataLoaded){
        this->drawWeather();   
        this->printDate();
        this->printLastUpdate();
    }
    if(this->requestMode){
       if (OswServiceAllTasks::wifi.isConnected()) {
          this->_request();
       }else{
        //TODO:pop-up
        this->hal->gfx()->setTextCursor(120,100);
        this->hal->gfx()->setTextSize(2);
        this->hal->gfx()->print("loading...");
       }
    }
    this->hal->requestFlush(); 
    if (hal->btnHasGoneDown(BUTTON_2)) {
            if(this->mainSelector==1){// next update
                if(this->updtSelector<23) {this->updtSelector++;}
            }
            if(this->mainSelector==0){//next day
                if(this->updtSelector >= this->dayFirstUpdt[this->dayFirstUpdt.size()-1]){
                    this->updtSelector=this->updtSelector;
                }else{
                    this->updtSelector = this->getNextDay();
                }
            }
    }
    if (hal->btnHasGoneDown(BUTTON_3)) {
       
        if(this->mainSelector==1){
            if(this->updtSelector>0){this->updtSelector--; }              
        }
        if(this->mainSelector==0){
            if(this->updtSelector == 0){
                this->updtSelector=this->updtSelector;
            }else{
                this->updtSelector = this->getPrevDay();
            }
        }
        if(this->mainSelector==2){
            this->weatherRequest();
        }
    }
    if( hal->btnHasGoneDown(BUTTON_1)){
        if(this->mainSelector!=2){
            this->mainSelector++;
        }else{
            this->mainSelector=0;
        }
    }

}

void OswAppWeather::stop() {

}
#endif
#endif