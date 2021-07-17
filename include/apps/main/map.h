#ifndef OSW_APP_MAP_H
#define OSW_APP_MAP_H

#include <NMEAGPS.h>
#include <osw_hal.h>

#include "osw_app.h"

class OswAppMap : public OswApp {
 public:
  OswAppMap(){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppMap(){};

 protected:
  void drawSatelliteOverlay(OswHal* hal);
  void drawDataOverlay(OswHal* hal);

 private:
  uint8_t overlay = 0;
  NMEAGPS::satellite_view_t _satellites[NMEAGPS_MAX_SATELLITES];
  uint8_t _sat_count;  // in the above array
};

#endif
