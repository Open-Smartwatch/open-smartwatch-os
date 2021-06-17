#ifndef OSW_SERVICE_RAW_SCREEN_SERVER_H
#define OSW_SERVICE_RAW_SCREEN_SERVER_H

#include "osw_service.h"

class OswServiceTaskRawScreenServer : public OswServiceTask {
 public:
  OswServiceTaskRawScreenServer(){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswServiceTaskRawScreenServer(){};

 private:
  time_t printLimit = 0;
};

#endif
