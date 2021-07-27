#include <time.h>


#include "./services/OswServiceTaskCore.h"
#include "osw_hal.h"

// #include <RtcDS3231.h>

/*
Core task that will handle I2C communication to for example the RTC
Also keeping track of the steps and update these steps every "new" day
*/

const uint32_t seconds_per_step_reset =  60*60*24;  //1-Day
const uint32_t step_count_vector_size = 7; 

void OswServiceTaskCore::setup(OswHal* hal) {
    // Pinned on core 1
     OswServiceTask::setup(hal);
    _pref.begin("core",false);
    _lastEpochStepUpdate = _pref.getUInt("lastEpochSU",0);

     std::vector<uint32_t>* StepCountvector = hal->getStepCountvector();
     StepCountvector->insert(StepCountvector->begin(),_pref.getUInt("StepV6",0));
     StepCountvector->insert(StepCountvector->begin(),_pref.getUInt("StepV5",0));
     StepCountvector->insert(StepCountvector->begin(),_pref.getUInt("StepV4",0));
     StepCountvector->insert(StepCountvector->begin(),_pref.getUInt("StepV3",0));
     StepCountvector->insert(StepCountvector->begin(),_pref.getUInt("StepV2",0));
     StepCountvector->insert(StepCountvector->begin(),_pref.getUInt("StepV1",0));
     StepCountvector->insert(StepCountvector->begin(),_pref.getUInt("StepV0",0));

    //check if value is valid
    if(_lastEpochStepUpdate > hal->getLocalTime() + seconds_per_step_reset){ 
        _lastEpochStepUpdate = 0;
    }
    hal->setTotalStepCount(_pref.getUInt("TotalStepCount",0));
    _timePreviousUpdate = time(nullptr);

#ifdef DEBUG
    uint32_t* stepdatavec = StepCountvector->data();
    Serial.printf("%u  Current stepvector:  ",_lastEpochStepUpdate);
    for(size_t i = 0; i<StepCountvector->size(); i++){
        Serial.printf("%u   ",stepdatavec[i]);
    }
    Serial.println();
#endif
}


void OswServiceTaskCore::loop(OswHal* hal) {
    //check if step counts need to be refreshed
    if(hal->getLocalTime() > (_lastEpochStepUpdate + seconds_per_step_reset)){
#ifdef DEBUG
            Serial.printf("hit updating steps %d  -> ",hal->getStepCount());
#endif

            const uint32_t tempUTC = hal->getLocalTime();

            //get the information from HAL;
            std::vector<uint32_t>* StepCountvector = hal->getStepCountvector();
            uint32_t stepsInThisPeriod =  hal->getStepCount();
            
            //reset the Step counter as soon as read out to not miss any steps
            hal->resetAccelerometer();          //this also sets the step_count back to 0 and does totalstepcount += stepcount
            hal->initAccelerometer();           //reinit accelerometer

            //fix the timing
            const uint32_t lastEpochStepSave = _lastEpochStepUpdate;        
            _lastEpochStepUpdate =  tempUTC - (tempUTC % seconds_per_step_reset);        

            //fix for when we missed a update Period 
            uint32_t TimesToUpdate = (_lastEpochStepUpdate - lastEpochStepSave)/seconds_per_step_reset - 1;
            if(TimesToUpdate > step_count_vector_size) TimesToUpdate = step_count_vector_size;
            if(TimesToUpdate) stepsInThisPeriod = stepsInThisPeriod/TimesToUpdate;
            while(TimesToUpdate){
                StepCountvector->insert(StepCountvector->begin(),stepsInThisPeriod);
                TimesToUpdate--;
            }
            
            //insert the new value
            StepCountvector->insert(StepCountvector->begin(),stepsInThisPeriod);

            //resize the vector
            while(StepCountvector->size() > step_count_vector_size){
                StepCountvector->pop_back();
            } 

            _pref.putUInt("TotalStepCount",hal->getTotalStepCount());
            _pref.putUInt("lastEpochSU",_lastEpochStepUpdate);
            
            uint32_t* stepdatavec = StepCountvector->data();
            if(StepCountvector->size() > 0) _pref.putUInt("StepV0",stepdatavec[0]);
            if(StepCountvector->size() > 1) _pref.putUInt("StepV1",stepdatavec[1]);
            if(StepCountvector->size() > 2) _pref.putUInt("StepV2",stepdatavec[2]);
            if(StepCountvector->size() > 3) _pref.putUInt("StepV3",stepdatavec[3]);
            if(StepCountvector->size() > 4) _pref.putUInt("StepV4",stepdatavec[4]);
            if(StepCountvector->size() > 5) _pref.putUInt("StepV5",stepdatavec[5]);
            if(StepCountvector->size() > 6) _pref.putUInt("StepV6",stepdatavec[6]);
            
#ifdef DEBUG
            Serial.print("Current stepvector:  ");
            for(size_t i = 0; i<StepCountvector->size(); i++){
                Serial.printf("%u   ",stepdatavec[i]);
            }
            Serial.println();
#endif
    }
}

void OswServiceTaskCore::stop(OswHal* hal) {
    OswServiceTask::stop(hal); 
}