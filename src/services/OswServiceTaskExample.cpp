#include "./services/OswServiceTaskExample.h"
#include "osw_hal.h"
#include <time.h>

void OswServiceTaskExample::setup() {
    OswServiceTask::setup();
    OSW_LOG_I(__FUNCTION__, "()");
}

void OswServiceTaskExample::loop() {
    time_t now = time(nullptr);
    if(now != this->printLimit) {
        OSW_LOG_I(__FUNCTION__, "()");
        this->printLimit = now;
    }
}

void OswServiceTaskExample::stop() {
    OswServiceTask::stop();
    OSW_LOG_I(__FUNCTION__, "()");
}