#include "osw_ui.h"

#include "./apps/main/switcher.h"
#include "./overlays/overlays.h"
#include "osw_config.h"

OswUI OswUI::instance = OswUI();
OswUI::OswUI(){};

void OswUI::setup(){};

OswUI* OswUI::getInstance() { return &OswUI::instance; };

uint16_t OswUI::getBackgroundColor(void) { return rgb888to565(OswConfigAllKeys::themeBackgroundColor.get()); }
uint16_t OswUI::getBackgroundDimmedColor(void) {
  return rgb888to565(OswConfigAllKeys::themeBackgroundDimmedColor.get());
}
uint16_t OswUI::getForegroundColor(void) { return rgb888to565(OswConfigAllKeys::themeForegroundColor.get()); }
uint16_t OswUI::getForegroundDimmedColor(void) {
  return rgb888to565(OswConfigAllKeys::themeForegroundDimmedColor.get());
}
uint16_t OswUI::getPrimaryColor(void) { return rgb888to565(OswConfigAllKeys::themePrimaryColor.get()); }
uint16_t OswUI::getInfoColor(void) { return rgb888to565(OswConfigAllKeys::themeInfoColor.get()); }
uint16_t OswUI::getSuccessColor(void) { return rgb888to565(OswConfigAllKeys::themeSuccessColor.get()); }
uint16_t OswUI::getWarningColor(void) { return rgb888to565(OswConfigAllKeys::themeWarningColor.get()); }
uint16_t OswUI::getDangerColor(void) { return rgb888to565(OswConfigAllKeys::themeDangerColor.get()); }

void OswUI::resetTextColors(void) {  //
  OswHal::getInstance()->gfx()->setTextColor(rgb888to565(OswConfigAllKeys::themeForegroundColor.get()),
                                             rgb888to565(OswConfigAllKeys::themeBackgroundColor.get()));
}

void OswUI::setTextCursor(Button btn) {
  // TODO: this is an ugly hack and needs to go into the main repo
  OswHal* hal = OswHal::getInstance();
  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextMiddleAligned();
  switch (btn) {
    case BUTTON_2:
      hal->gfx()->setTextRightAligned();
      hal->gfx()->setTextCursor(204, 196);
      break;
    case BUTTON_3:
      hal->gfx()->setTextRightAligned();
      hal->gfx()->setTextCursor(204, 44);
      break;
    case BUTTON_1:
    default:
      hal->gfx()->setTextRightAligned();
      hal->gfx()->setTextCursor(46, 196);
  }
}

void OswUI::loop(OswAppSwitcher& mainAppSwitcher, uint16_t& mainAppIndex) {
  static long lastFlush = 0;

  // BG
  OswHal::getInstance()->gfx()->fill(this->getBackgroundColor());
  this->resetTextColors();

  // Apps
  mainAppSwitcher.loop();

  // Full-Screen progress
  // TODO

  // Limit to configured fps and handle display flushing
  if (millis() - lastFlush > 1000 / this->mTargetFPS && OswHal::getInstance()->isRequestFlush()) {
    // Only draw overlays if enabled
    if (OswConfigAllKeys::settingDisplayOverlays.get())
      // Only draw on first face if enabled, or on all others
      if ((mainAppIndex == 0 && OswConfigAllKeys::settingDisplayOverlaysOnWatchScreen.get()) || mainAppIndex != 0)
        drawOverlays();
    OswHal::getInstance()->flushCanvas();
    lastFlush = millis();
  }
}