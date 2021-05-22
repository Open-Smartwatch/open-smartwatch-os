#ifndef OSW_SERVICE_TASKGPS_H
#define OSW_SERVICE_TASKGPS_H

#include "osw_service.h"

class OswServiceTaskGPS : public OswServiceTask {
 public:
  OswServiceTaskGPS(){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  ~OswServiceTaskGPS(){};
};

#endif