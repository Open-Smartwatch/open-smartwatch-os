#include "./services/OswServiceTaskGPS.h"

#include "osw_hal.h"

void OswServiceTaskGPS::setup(OswHal* hal) {
  OswServiceTask::setup(hal);
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

  hal->setupGps();

  Serial.print("PSRAM free: ");
  Serial.println(ESP.getMinFreePsram());
  Serial.print("Free Memory: ");
  Serial.println((int)xPortGetFreeHeapSize());
#endif
}

void OswServiceTaskGPS::loop(OswHal* hal) {
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

  // TODO: move to background service
  hal->gpsParse();
#endif
}
