  
#include "./apps/main/switcher.h"



#define SLEEP_TIMOUT 1000

  void OswAppSwitcher::setup(OswHal* hal){
    _apps[_appIndex]->setup(hal);
  }

  void OswAppSwitcher::loop(OswHal* hal){
    static unsigned long appOnScreenSince = millis();

    _apps[_appIndex]->loop(hal);

    if(_checked){
        unsigned long since = hal->btnIsDownSince(_btn);

        // draw sleep
        if (hal->btnIsDownSince(_btn) > DEFAULTLAUNCHER_LONG_PRESS + SLEEP_TIMOUT) {
            hal->gfx()->fillCircle(120, 230, 9, rgb565(128, 128, 128));
            hal->gfx()->fillCircle(120, 230, 8, rgb565(255, 255, 255));
            hal->gfx()->fillCircle(123, 230, 6, rgb565(0, 0, 0));
        }

        if(since > DEFAULTLAUNCHER_LONG_PRESS + SLEEP_TIMOUT){
            sleep(hal);
        }
        if(hal->btnHasGoneUp(_btn)){
            cycleApp(hal);
        }
    }else{    
        if(_type == LONG_PRESS){
            if(hal->btnIsLongPress(_btn)){
                _checked = true;
            }
        }else{
            if(hal->btnHasGoneDown(_btn)){
                _checked = false;
            }
        }
    }

    if(_enableAutoSleep && _appIndex == 0){
        if (_appIndex == 0 && (millis() - appOnScreenSince) > 15000) {
            hal->gfx()->fill(rgb565(0, 0, 0));
            hal->flushCanvas();
            hal->deepSleep();
        }
    }
    // draw app switcher
    if (hal->btnIsDown(_btn)) {
        if (hal->btnIsDownSince(_btn) < DEFAULTLAUNCHER_LONG_PRESS + SLEEP_TIMOUT) {
            uint8_t brightness = hal->btnIsDownSince(_btn) - 255;
            hal->gfx()->drawFrame(120 - 4, 230 - 4, 8, 8, rgb565(brightness, brightness, brightness));
            
        if(hal->btnIsDownSince(_btn) > DEFAULTLAUNCHER_LONG_PRESS){
                hal->gfx()->fillFrame(120 - 4, 230 - 4, 8, 8, rgb565(255, 255, 255));
        }
        }
    }

  }

  void OswAppSwitcher::cycleApp(OswHal* hal){
    _apps[_appIndex]->stop(hal);
    _appIndex++;
    if(_appIndex == _appCount){
        _appIndex = 0;
    }  
    _apps[_appIndex]->setup(hal);  
    hal->suppressButtonUntilUp(_btn);
  }

  void OswAppSwitcher::sleep(OswHal* hal){
    hal->gfx()->fill(rgb565(0, 0, 0));
    hal->flushCanvas();
    hal->deepSleep();
  }

  void OswAppSwitcher::stop(OswHal* hal){
    _apps[_appIndex]->stop(hal);
  }

  void OswAppSwitcher::registerApp(OswApp* app){
    _appCount++;
    _apps.push_back(app);
  }