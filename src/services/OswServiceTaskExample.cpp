#include "./services/OswServiceTaskExample.h"
#include "osw_hal.h"
#include <time.h>

void OswServiceTaskExample::setup() {
    OswServiceTask::setup();
    Serial.println(String(__FILE__)  + "->" + __FUNCTION__ + "()");
}

void OswServiceTaskExample::loop() {
    time_t now = time(nullptr);
    if(now != this->printLimit) {
        Serial.println(String(__FILE__)  + "->" + __FUNCTION__ + "()");
        this->printLimit = now;
    }
}

void OswServiceTaskExample::stop() {
    OswServiceTask::stop();
    Serial.println(String(__FILE__)  + "->" + __FUNCTION__ + "()");
}