#include <time.h>
#include <Preferences.h>

#include "./services/OswServiceTaskCore.h"
#include "osw_hal.h"

/*
Core task that will handle I2C communication to for example the RTC
Also keeping track of the steps and update these steps every "new" day
*/

void OswServiceTaskCore::setup(OswHal* hal) {
    // Pinned on core 1
     OswServiceTask::setup(hal);
    _timePreviousUpdate = time(nullptr);    
    hal->fetchRtcTime();
}

void OswServiceTaskCore::loop(OswHal* hal) {
    // We should add here all I2C sensors ++ core updates to make sure we don't have bus errors.
    //at least 1 second delay
    if(time(nullptr) > _timePreviousUpdate) {
        Serial.println("hit updating RTC");
        //checking if we need to update the RTC
        if(hal->_setRTC){
            hal->_setRTC = false;
            hal->setRtcTime();
        }
        //update the internal Hal time
        hal->fetchRtcTime();
        Serial.println(hal->getUTCTime());
        _timePreviousUpdate = time(nullptr);
    }
}

void OswServiceTaskCore::stop(OswHal* hal) {
    OswServiceTask::stop(hal); 
}