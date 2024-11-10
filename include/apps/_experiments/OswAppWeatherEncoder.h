#pragma once
#ifdef OSW_FEATURE_WEATHER

#include <optional>

#include "apps/_experiments/OswAppWeather.h"

class OswAppWeatherEncoder {
  public:
    OswAppWeatherEncoder();
    bool setUpdate(OswAppWeather::weather_update_t update);
    bool setTimestamp(time_t t);
    std::optional<String> getEncoded();

  private:
    String _time2str(time_t time);
    String _temp2str(int temp);
    String _hum2str(int humidity);
    String _pres2str(int pressure);
    String _wthr2str(int weather);
    bool time_loaded = false;
    time_t timestamp;
    String updates;
};
#endif
