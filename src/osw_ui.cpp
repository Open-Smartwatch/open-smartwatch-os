#include "osw_ui.h"

#include "osw_config.h"

OswUI OswUI::instance = OswUI();
OswUI::OswUI(){};

void OswUI::setup(OswHal* hal) {
  _hal = hal;
  // OswConfigAllKeys::themePrimaryColor.get()
  themeBackgroundColor = rgb888to565(OswConfigAllKeys::themeBackgroundColor.get());
  themeForegroundColor = rgb888to565(OswConfigAllKeys::themeForegroundColor.get());
  themePrimaryColor = rgb888to565(OswConfigAllKeys::themePrimaryColor.get());
  themeInfoColor = rgb888to565(OswConfigAllKeys::themeInfoColor.get());
  themeSuccessColor = rgb888to565(OswConfigAllKeys::themeSuccessColor.get());
  themeWarningColor = rgb888to565(OswConfigAllKeys::themeWarningColor.get());
  themeDangerColor = rgb888to565(OswConfigAllKeys::themeDangerColor.get());
};

OswUI* OswUI::getInstance() { return &OswUI::instance; };

uint16_t OswUI::getBackgroundColor(void) { return themeBackgroundColor; }
uint16_t OswUI::getBackgroundDimmedColor(void) { return themeBackgroundDimmedColor; }
uint16_t OswUI::getForegroundColor(void) { return themeForegroundColor; }
uint16_t OswUI::getForegroundDimmedColor(void) { return themeForegroundDimmedColor; }
uint16_t OswUI::getPrimaryColor(void) { return themePrimaryColor; }
uint16_t OswUI::getInfoColor(void) { return themeInfoColor; }
uint16_t OswUI::getSuccessColor(void) { return themeSuccessColor; }
uint16_t OswUI::getWarningColor(void) { return themeWarningColor; }
uint16_t OswUI::getDangerColor(void) { return themeDangerColor; }

void OswUI::resetColors(void) {  //
  _hal->gfx()->setTextColor(getForegroundColor(), getBackgroundColor());
  themeBackgroundColor = rgb888to565(OswConfigAllKeys::themeBackgroundColor.get());
  themeForegroundColor = rgb888to565(OswConfigAllKeys::themeForegroundColor.get());
  themePrimaryColor = rgb888to565(OswConfigAllKeys::themePrimaryColor.get());
  themeInfoColor = rgb888to565(OswConfigAllKeys::themeInfoColor.get());
  themeSuccessColor = rgb888to565(OswConfigAllKeys::themeSuccessColor.get());
  themeWarningColor = rgb888to565(OswConfigAllKeys::themeWarningColor.get());
  themeDangerColor = rgb888to565(OswConfigAllKeys::themeDangerColor.get());
}

void OswUI::setTextCursor(Button btn) {
  // TODO: this is an ugly hack and needs to go into the main repo
  _hal->gfx()->setTextSize(2);
  _hal->gfx()->setTextMiddleAligned();
  switch (btn) {
    case BUTTON_2:
      _hal->gfx()->setTextRightAligned();
      _hal->gfx()->setTextCursor(204, 196);
      break;
    case BUTTON_3:
      _hal->gfx()->setTextRightAligned();
      _hal->gfx()->setTextCursor(204, 44);
      break;
    case BUTTON_1:
    default:
      _hal->gfx()->setTextRightAligned();
      _hal->gfx()->setTextCursor(46, 196);
  }
}
