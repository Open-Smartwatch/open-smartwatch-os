#include "./services/OswServiceTaskExample.h"
#include "osw_hal.h"
#include <time.h>

void OswServiceTaskExample::setup(OswHal* hal) {
    OswServiceTask::setup(hal);
    Serial.println(String(__FILE__)  + "->" + __FUNCTION__ + "()");
}

void OswServiceTaskExample::loop(OswHal* hal) {
    time_t now = time(nullptr);
    if(now != this->printLimit) {
        Serial.println(String(__FILE__)  + "->" + __FUNCTION__ + "()");
        this->printLimit = now;
    }
}

void OswServiceTaskExample::stop(OswHal* hal) {
    OswServiceTask::stop(hal);
    Serial.println(String(__FILE__)  + "->" + __FUNCTION__ + "()");
}