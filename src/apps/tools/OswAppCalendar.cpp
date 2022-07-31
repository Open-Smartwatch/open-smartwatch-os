#include "./apps/tools/OswAppCalendar.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

void OswAppCalendar::printEvent(const char *date, const char *event, uint16_t color) {
    OswHal *hal = OswHal::getInstance();
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(30, y);
    hal->gfx()->print(date);
    hal->gfx()->fillCircle(105, y, 2, color);

    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(115, y);
    hal->gfx()->print(event);

    y += 8;
}

void OswAppCalendar::setup() {
}
void OswAppCalendar::loop() {
    OswHal *hal = OswHal::getInstance();

    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(120, 60);
    uint32_t m, d, y = 0;
    hal->getLocalDate(&d, &m, &y);
    const char * wd=hal->getLocalWeekday();

    {
        char weekday3[4];
        weekday3[0] = wd[0];
        weekday3[1] = wd[1];
        weekday3[2] = wd[2];
        weekday3[3] = '\0';
        hal->gfx()->print(m+String("/")+d+String("-")+weekday3);
    }

    char eventNote[] = "Discord Open-smartwatch dev-team";
    printEvent("14:00~17:00", hal->gfx()->printSlice(eventNote, 15, true), rgb565(235, 235, 23));

    hal->requestFlush();
}

void OswAppCalendar::stop() {
}