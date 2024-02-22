#ifdef OSW_FEATURE_WEATHER
#include "apps/_experiments/OswAppWeatherEncoder.h"

OswAppWeatherEncoder::OswAppWeatherEncoder() {}

bool OswAppWeatherEncoder::setUpdate(OswAppWeather::weather_update_t update) {
    bool update_ok = true;
    if(update.temp > 99 || update.temp < -99 ) {
    OSW_LOG_W("ERROR TEMP");
    OSW_LOG_W(update.temp);
    update_ok = false;
    }
    if(update.humidity > 100 || update.humidity < 0) {
        OSW_LOG_W("ERROR HUMIDITY");
        OSW_LOG_W(update.humidity);
        update_ok = false;
    }
    if(update.pressure < 0 || update.pressure > 2000 ) {
        OSW_LOG_W("ERROR PRESSURE");
        OSW_LOG_W(update.pressure);
        update_ok = false;
    }
    if(update.weather < 0 || update.weather > 15) {
        OSW_LOG_W(update.weather);
        OSW_LOG_W("ERROR WEATHER");
        update_ok = false;
    }
    if(!update_ok) {
        return false;
    }
    String update_s;
    update_s += _temp2str(update.temp);
    update_s += _hum2str(update.humidity);
    update_s += _pres2str(update.pressure);
    update_s += _wthr2str(update.weather);
    this->updates += update_s;
    return true;

}

bool OswAppWeatherEncoder::setTimestamp(time_t t) {
    if(t > 0 && t < 2147483647 ) {
        this->timestamp = t;
        this->time_loaded = true;
        return true;
    }
    return false;
}

String OswAppWeatherEncoder::getEncoded() {
    if(this->time_loaded) {
        String encoded;
        encoded += _time2str(this->timestamp);
        encoded += this->updates;
        return encoded;
    } else {
        return "error_no_timestamp";
    }
}

String OswAppWeatherEncoder::_time2str(time_t time) {
    time =  2147483647 - time ;
    time = time / 8 ;// time = (seconds to end of the epoch 01/19/2038 3:14 AM ) / 8
    char time_dgts[10];
    sprintf(time_dgts,"%08ld",time);
    return time_dgts;
}

String OswAppWeatherEncoder::_temp2str(int temp) {
    String temp_s;
    if (temp > 0) {
        temp_s += '+';
    } else {
        temp_s += '-';
    }
    char temp_dgts[11];
    sprintf(temp_dgts,"%02d",abs(temp));
    temp_s += temp_dgts;
    return temp_s;
}

String OswAppWeatherEncoder::_hum2str(int humidity) {
    if(humidity == 100) {
        humidity = 99;
    }
    char h[11];
    sprintf(h,"%d",humidity/10);
    return h;
}

String OswAppWeatherEncoder::_pres2str(int pressure) {
    pressure = pressure - 850;
    if(pressure < 0) {
        pressure = 0;
    }
    if(pressure > 999) {
        pressure = 999;
    }
    String pres_s;
    char pres_dgts[11];
    sprintf(pres_dgts, "%03d",abs(pressure));
    pres_s += pres_dgts;
    return pres_s;
}

String OswAppWeatherEncoder::_wthr2str(int weather) {
    char w[2];
    sprintf(w,"%c",weather+65);
    return w;
}
#endif
