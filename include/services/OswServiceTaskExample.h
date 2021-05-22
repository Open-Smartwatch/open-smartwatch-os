#ifndef OSW_SERVICE_TASKEXAMPLE_H
#define OSW_SERVICE_TASKEXAMPLE_H

#include "osw_service.h"

class OswServiceTaskExample : public OswServiceTask {
 public:
  OswServiceTaskExample(){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswServiceTaskExample(){};

 private:
  time_t printLimit = 0;
};

#endif