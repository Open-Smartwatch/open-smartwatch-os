#pragma once

#include "osw_service.h"

class OswServiceTaskMemMonitor : public OswServiceTask {
  public:
    OswServiceTaskMemMonitor() {};
    virtual void setup() override;
    virtual void loop() override;
    void updateLoopTaskStats();  // Call this from the main loop regularly!
    void printStats();
    bool hasLowMemoryCondition();
    ~OswServiceTaskMemMonitor() {};

  private:
    unsigned core0high;
    unsigned core1high;
    unsigned heapHigh;
    bool lowMemoryCondition = false;
};
