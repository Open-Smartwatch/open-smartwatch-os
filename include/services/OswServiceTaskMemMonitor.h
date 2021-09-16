#ifndef OSW_SERVICE_TASKMEMMONITOR_H
#define OSW_SERVICE_TASKMEMMONITOR_H

#include "osw_service.h"

class OswServiceTaskMemMonitor : public OswServiceTask {
 public:
  OswServiceTaskMemMonitor(){};
  virtual void setup() override;
  virtual void loop() override;
  void updateLoopTaskStats();  // Call this from the main loop regulary!
  void printStats();
  ~OswServiceTaskMemMonitor(){};

 private:
  unsigned core0high;
  unsigned core1high;
  unsigned heapHigh;
};

#endif