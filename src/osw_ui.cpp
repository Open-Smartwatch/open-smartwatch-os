#include "osw_ui.h"

#include "osw_config.h"

OswUI OswUI::instance = OswUI();
OswUI::OswUI(){};

void OswUI::setup(OswHal* hal) {
  _hal = hal;
};

OswUI* OswUI::getInstance() { return &OswUI::instance; };

uint16_t OswUI::getBackgroundColor(void) { return rgb888to565(OswConfigAllKeys::themeBackgroundColor.get()); }
uint16_t OswUI::getBackgroundDimmedColor(void) { return rgb888to565(OswConfigAllKeys::themeBackgroundDimmedColor.get()); }
uint16_t OswUI::getForegroundColor(void) { return rgb888to565(OswConfigAllKeys::themeForegroundColor.get()); }
uint16_t OswUI::getForegroundDimmedColor(void) { return rgb888to565(OswConfigAllKeys::themeForegroundDimmedColor.get()); }
uint16_t OswUI::getPrimaryColor(void) { return rgb888to565(OswConfigAllKeys::themePrimaryColor.get()); }
uint16_t OswUI::getInfoColor(void) { return rgb888to565(OswConfigAllKeys::themeInfoColor.get()); }
uint16_t OswUI::getSuccessColor(void) { return rgb888to565(OswConfigAllKeys::themeSuccessColor.get()); }
uint16_t OswUI::getWarningColor(void) { return rgb888to565(OswConfigAllKeys::themeWarningColor.get()); }
uint16_t OswUI::getDangerColor(void) { return rgb888to565(OswConfigAllKeys::themeDangerColor.get()); }

void OswUI::resetTextColors(void) {  //
  _hal->gfx()->setTextColor(rgb888to565(OswConfigAllKeys::themeForegroundColor.get()), rgb888to565(OswConfigAllKeys::themeBackgroundColor.get()));
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
