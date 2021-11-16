#include "./services/OswServiceTaskGPS.h"

#include "osw_hal.h"

void OswServiceTaskGPS::setup() {
  OswServiceTask::setup();
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

  OswHal::getInstance()->setupGps();

  Serial.print("PSRAM free: ");
  Serial.println(ESP.getMinFreePsram());
  Serial.print("Free Memory: ");
  Serial.println((int)xPortGetFreeHeapSize());
#endif
}

void OswServiceTaskGPS::loop() {
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  OswHal::getInstance()->gpsParse();
#endif
}

void OswServiceTaskGPS::stop() {
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  OswHal::getInstance()->gpsBackupMode();
#endif
}
