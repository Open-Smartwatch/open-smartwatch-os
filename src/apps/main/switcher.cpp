
#include "./apps/main/switcher.h"

#define SLEEP_TIMOUT 1000

void OswAppSwitcher::setup(OswHal* hal) {
  appOnScreenSince = millis();
  if (*_rtcAppIndex >= _appCount) {
    *_rtcAppIndex = 0;
  }
  _apps[*_rtcAppIndex]->setup(hal);
}

void OswAppSwitcher::loop(OswHal* hal) {
  // if we enable sending the watch to sleep by clicking (really really) long enough
  if (_enableDeepSleep && hal->btnIsDownSince(_btn) > DEFAULTLAUNCHER_LONG_PRESS + SLEEP_TIMOUT) {
    // remember we need to sleep once the button goes up
    _doSleep = true;
  }

  // detect switch action depending on mode
  switch (_type) {
    case LONG_PRESS:
      if (hal->btnIsDownSince(_btn) > DEFAULTLAUNCHER_LONG_PRESS) {
        _doSwitch = true;
      }
      break;
    case SHORT_PRESS:
    default:
      if (hal->btnHasGoneDown(_btn)) {
        _doSwitch = true;
      }
  }

  // do action only once the button goes up
  if (hal->btnHasGoneUp(_btn)) {
    if (_doSleep) {
      _doSleep = false;
      sleep(hal);
    }
    if (_doSwitch) {
      _doSwitch = false;
      cycleApp(hal);
      // we need to clear the button state, otherwise nested switchers
      // using the same button will switch too
      hal->clearButtonState(_btn);
    }
  }

  if (_enableAutoSleep && *_rtcAppIndex == 0 && !hal->btnIsDown(_btn)) {
    long timeout = OswConfigAllKeys::settingDisplayTimeout.get();
    if (*_rtcAppIndex == 0 && (millis() - appOnScreenSince) > timeout * 1000) {
      if(hal->btnIsDown(BUTTON_1) || hal->btnIsDown(BUTTON_2) || hal->btnIsDown(BUTTON_3)){
        appOnScreenSince = millis();
      }else{
        Serial.print("Going to sleep after ");
        Serial.println(timeout);
        hal->gfx()->fill(rgb565(0, 0, 0));
        hal->flushCanvas();
        hal->deepSleep();
      }
    }
  }

  hal->gfx()->resetText();
  OswUI::getInstance()->resetTextColors();  // yes this resets the colors in hal->gfx()
  _apps[*_rtcAppIndex]->loop(hal);

  // draw app switcher
  if (hal->btnIsDown(_btn)) {
    uint8_t btnX = 0;
    uint8_t btnY = 0;
    int16_t progressOffset = 0;
    // draw button switcher close to button
    switch (_btn) {
      case BUTTON_2:
        btnX = 214;
        btnY = 190;
        progressOffset = 135;
        break;
      case BUTTON_3:
        btnX = 214;
        btnY = 50;
        progressOffset = 135;
        break;
      case BUTTON_1:
      default:
        btnX = 26;
        btnY = 190;
        progressOffset = -45;
        break;
    }

    switch (_type) {
      case LONG_PRESS:
        // long press has the hollow square that fills (draws around short press)
        if (hal->btnIsDownSince(_btn) > DEFAULTLAUNCHER_LONG_PRESS) {
          // draw a large frame
          hal->gfx()->fillCircle(btnX, btnY, 20, OswUI::getInstance()->getSuccessColor());
        } else {
          uint8_t progress = 0;
          if (hal->btnIsDownSince(_btn) > DEFAULTLAUNCHER_LONG_PRESS / 2) {
            progress = (hal->btnIsDownSince(_btn) - (DEFAULTLAUNCHER_LONG_PRESS / 2)) /
                       ((DEFAULTLAUNCHER_LONG_PRESS / 2) / 255.0);
          }
          hal->gfx()->drawArc(btnX, btnY, progressOffset, progressOffset + (progress / 255.0) * 180, progress / 4, 20,
                              3, OswUI::getInstance()->getForegroundColor());
        }
        break;
      case SHORT_PRESS:
      default:
        hal->gfx()->fillCircle(btnX, btnY, 10, OswUI::getInstance()->getSuccessColor());
    }

    if (_enableDeepSleep && hal->btnIsDownSince(_btn) > DEFAULTLAUNCHER_LONG_PRESS + SLEEP_TIMOUT) {
      // draw half moon
      hal->gfx()->fillCircle(btnX, btnY, 9, OswUI::getInstance()->getForegroundDimmedColor());
      hal->gfx()->fillCircle(btnX, btnY, 8, OswUI::getInstance()->getBackgroundColor());
      hal->gfx()->fillCircle(btnX + 3, btnY, 6, OswUI::getInstance()->getForegroundDimmedColor());
    }
  }
}

void OswAppSwitcher::cycleApp(OswHal* hal) {
  appOnScreenSince = millis();
  _apps[*_rtcAppIndex]->stop(hal);
  *_rtcAppIndex = *_rtcAppIndex + 1;
  if (*_rtcAppIndex == _appCount) {
    *_rtcAppIndex = 0;
  }
  _apps[*_rtcAppIndex]->setup(hal);
  hal->suppressButtonUntilUp(_btn);
}

void OswAppSwitcher::sleep(OswHal* hal) {
  hal->gfx()->fill(rgb565(0, 0, 0));
  hal->flushCanvas();
  hal->deepSleep();
}

void OswAppSwitcher::stop(OswHal* hal) { _apps[*_rtcAppIndex]->stop(hal); }

void OswAppSwitcher::registerApp(OswApp* app) {
  _appCount++;
  _apps.push_back(app);
}
