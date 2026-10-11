#include <gfx_util.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include "./apps/widget/OswAppWidgets.h"
#include OSW_TARGET_PLATFORM_HEADER

#ifdef OSW_FEATURE_STATS_STEPS
void OswAppWidget::drawStepHistory(OswUI* ui, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t max) {
    OswHal* hal = OswHal::getInstance();
    OswUI::getInstance()->resetTextColors();

    OswDate oswDate = { };
    hal->getLocalDate(oswDate);

    for (uint8_t i = 0; i < 7; i++) {
        uint32_t s = hal->environment()->getStepsOnDay(i);
        uint16_t boxHeight = ((float)(s > max ? max : s) / max) * h;
        boxHeight = boxHeight < 2 ? 0 : boxHeight;

        // step bars
        uint16_t c = (unsigned int) OswConfigAllKeys::stepsPerDay.get() <= s ? ui->getSuccessColor() : ui->getPrimaryColor();
        hal->gfx()->fillFrame(x + i * w, y + (h - boxHeight), w, boxHeight, c);
        // bar frames
        uint16_t f = oswDate.weekDay == i ? ui->getForegroundColor() : ui->getForegroundDimmedColor();
        hal->gfx()->drawRFrame(x + i * w, y, w, h, 2, f);

        // labels
        hal->gfx()->setTextCenterAligned();  // horiz.
        hal->gfx()->setTextBottomAligned();
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextCursor(DISP_W / 2, y - 1);

        hal->gfx()->print(hal->environment()->getStepsToday() + (OswConfigAllKeys::settingDisplayStepsGoal.get() ? String("/") + max:""));

        hal->gfx()->setTextCursor(DISP_W / 2, y + 1 + 8 + w * 4);
        hal->gfx()->setTextColor(ui->getForegroundColor());  // Let's make the background transparent.
        // See : https://github.com/Open-Smartwatch/open-smartwatch-os/issues/194
        // font : WHITE / bg : None
        hal->gfx()->print(hal->environment()->getStepsTotal());
        hal->gfx()->setTextColor(ui->getForegroundColor(), ui->getBackgroundColor());  // restore. font : WHITE / bg : BLACK
    }
}
#endif