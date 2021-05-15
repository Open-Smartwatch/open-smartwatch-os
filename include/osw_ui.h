#ifndef OSW_UI_H
#define OSW_UI_H

#include <osw_hal.h>

class OswUI {
 public:
  OswUI();
  void setup(OswHal* hal);
  static OswUI* getInstance();

  uint16_t getBackgroundColor(void);
  uint16_t getBackgroundDimmedColor(void);
  uint16_t getForegroundColor(void);
  uint16_t getForegroundDimmedColor(void);
  uint16_t getPrimaryColor(void);
  uint16_t getInfoColor(void);
  uint16_t getSuccessColor(void);
  uint16_t getWarningColor(void);
  uint16_t getDangerColor(void);

  void reloadThemeColors(void);
  void resetTextColors(void);
  void setTextCursor(Button btn);

 private:
  static OswUI instance;
  OswHal* _hal;
  uint16_t themeBackgroundColor = 0;
  uint16_t themeBackgroundDimmedColor = 0;
  uint16_t themeForegroundColor = 0;
  uint16_t themeForegroundDimmedColor = 0;
  uint16_t themePrimaryColor = 0;
  uint16_t themeInfoColor = 0;
  uint16_t themeSuccessColor = 0;
  uint16_t themeWarningColor = 0;
  uint16_t themeDangerColor = 0;
};

#endif
