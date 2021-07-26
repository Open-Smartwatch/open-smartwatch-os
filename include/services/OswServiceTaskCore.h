#ifndef OSW_SERVICE_TASKCORE_H
#define OSW_SERVICE_TASKCORE_H

#include "osw_service.h"

class OswServiceTaskCore : public OswServiceTask {
 public:
  OswServiceTaskCore(){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswServiceTaskCore(){};

 private:
  time_t _timePreviousUpdate;
};

#endif