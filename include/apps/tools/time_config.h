#ifndef OSW_APP_TIME_FROM_WEB_H
#define OSW_APP_TIME_FROM_WEB_H

#include <osw_hal.h>
#include <osw_ui.h>
#include "apps/main/switcher.h"

#include "osw_app.h"

extern OswAppSwitcher *settingsAppSwitcher;

class OswAppTimeConfig : public OswApp {
 public:
  OswAppTimeConfig(void){
    ui = OswUI::getInstance();
  };
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswAppTimeConfig(){};

 private:
  void enterManualMode();
  void handleIncrementButton();
  void handleDecrementButton();
  void handleNextButton();
  bool manualSettingScreen = false;
  int8_t manualSettingStep = 0;
  int16_t manualSettingTimestamp[11];
  OswUI* ui;
};

#endif
