#ifndef OSW_SERVICE_TASKGPS_H
#define OSW_SERVICE_TASKGPS_H

#include "osw_service.h"

class OswServiceTaskGPS : public OswServiceTask {
 public:
  OswServiceTaskGPS(){};
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswServiceTaskGPS(){};
};

#endif