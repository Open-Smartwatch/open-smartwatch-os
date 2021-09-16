#ifndef OSW_SERVICE_TASKEXAMPLE_H
#define OSW_SERVICE_TASKEXAMPLE_H

#include "osw_service.h"

class OswServiceTaskExample : public OswServiceTask {
 public:
  OswServiceTaskExample(){};
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswServiceTaskExample(){};

 private:
  time_t printLimit = 0;
};

#endif