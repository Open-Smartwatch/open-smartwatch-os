#include "./services/OswServiceTaskGPS.h"

#include "osw_hal.h"

void OswServiceTaskGPS::setup(OswHal* hal) {
  OswServiceTask::setup(hal);
#ifdef GPS_EDITION
  hal->setupGps();
  hal->setupSD();

  Serial.print("PSRAM free: ");
  Serial.println(ESP.getMinFreePsram());
  Serial.print("Free Memory: ");
  Serial.println((int)xPortGetFreeHeapSize());
#endif
}

void OswServiceTaskGPS::loop(OswHal* hal) {
#ifdef GPS_EDITION
  // TODO: move to background service
  hal->gpsParse();
#endif
}
