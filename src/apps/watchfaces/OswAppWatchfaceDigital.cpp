#include <gfx_util.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

#include "./apps/watchfaces/OswAppWatchface.h"
#include "./apps/watchfaces/OswAppWatchfaceDigital.h"

uint8_t OswAppWatchfaceDigital::dateFormatCache = 42;

uint8_t OswAppWatchfaceDigital::getDateFormat() {
    if(OswAppWatchfaceDigital::dateFormatCache == 42)
        OswAppWatchfaceDigital::refreshDateFormatCache();
    // Note, that we are using the cache here, as this function is commonly evaluated with every frame drawn!
    return OswAppWatchfaceDigital::dateFormatCache;
}

void OswAppWatchfaceDigital::refreshDateFormatCache() {
    String format = OswConfigAllKeys::dateFormat.get();
    OswAppWatchfaceDigital::dateFormatCache = (format == "mm/dd/yyyy" ? 1 : (format == "dd.mm.yyyy" ? 2 : 3));
}

// display Weekday to 3 charater
void OswAppWatchfaceDigital::displayWeekDay3(const char* weekday) {
    OswHal* hal = OswHal::getInstance();

    char weekday3[4];
    weekday3[0] = weekday[0];
    weekday3[1] = weekday[1];
    weekday3[2] = weekday[2];
    weekday3[3] = '\0';
    hal->gfx()->print(weekday3);
}

void OswAppWatchfaceDigital::dateOutput(uint32_t yearInt, uint32_t monthInt, uint32_t dayInt) {
    OswHal* hal = OswHal::getInstance();
    switch (OswAppWatchfaceDigital::getDateFormat()) {
    case 1:  // 0 : mm/dd/yyyy
        hal->gfx()->printDecimal(monthInt, 2);
        hal->gfx()->print("/");
        hal->gfx()->printDecimal(dayInt, 2);
        hal->gfx()->print("/");
        hal->gfx()->print(yearInt);
        break;
    case 2:  // 1 : dd.mm.yyyy
        hal->gfx()->printDecimal(dayInt, 2);
        hal->gfx()->print(".");
        hal->gfx()->printDecimal(monthInt, 2);
        hal->gfx()->print(".");
        hal->gfx()->print(yearInt);
        break;
    case 3:  // 2 : yy.mm/dd
        hal->gfx()->print(yearInt);
        hal->gfx()->print(".");
        hal->gfx()->printDecimal(monthInt, 2);
        hal->gfx()->print("/");
        hal->gfx()->printDecimal(dayInt, 2);
        break;
    }
}

static void drawDate(time_t timeZone, uint8_t fontSize, uint8_t CoordY) {
    uint32_t dayInt = 0;
    uint32_t monthInt = 0;
    uint32_t yearInt = 0;
    OswHal* hal = OswHal::getInstance();
    const char* weekday = hal->getWeekday(timeZone);

    hal->getDate(timeZone,&dayInt, &monthInt, &yearInt);

    // we want to output a value like "Wed, 05/02/2021"

    hal->gfx()->setTextSize(fontSize);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(6.9f), CoordY);

    OswAppWatchfaceDigital::displayWeekDay3(weekday);

    // The GFX library has an alignment bug, causing single letters to "float", therefore the workaround above is used to still utilize the correct string printing.
    //hal->gfx()->print(weekday[0]);
    //hal->gfx()->print(weekday[1]);
    //hal->gfx()->print(weekday[2]);
    hal->gfx()->print(", ");

    // i really would want the date to be dynamic based on what's in the config, but the most efficient thing to do right
    // now is simply three if statements covering the 3 common conditions.
    OswAppWatchfaceDigital::dateOutput(yearInt, monthInt, dayInt);
}

void OswAppWatchfaceDigital::timeOutput(uint32_t hour, uint32_t minute, uint32_t second,bool showSecond) {
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->printDecimal(hour, 2);
    hal->gfx()->print(":");
    hal->gfx()->printDecimal(minute, 2);
    if (showSecond) {
        hal->gfx()->print(":");
        hal->gfx()->printDecimal(second, 2);
    }
}

static void drawTime(time_t timeZone,uint8_t CoordY) {
    uint32_t second = 0;
    uint32_t minute = 0;
    uint32_t hour = 0;
    bool afterNoon = false;
    char am[] = "AM";
    char pm[] = "PM";
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->setTextSize(OswConfigAllKeys::timeFormat.get() ? 4 : 3);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(OswConfigAllKeys::timeFormat.get() ? 4 : 5.5),CoordY );

    hal->getTime(timeZone,&hour, &minute, &second, &afterNoon);
    OswAppWatchfaceDigital::timeOutput(hour, minute, second);
    if (!OswConfigAllKeys::timeFormat.get()) {
        hal->gfx()->print(" ");
        if (afterNoon) {
            hal->gfx()->print(pm);
        } else {
            hal->gfx()->print(am);
        }
    }
}

const char* OswAppWatchfaceDigital::getAppId() {
    return OswAppWatchfaceDigital::APP_ID;
}

const char* OswAppWatchfaceDigital::getAppName() {
    return LANG_DIGITAL;
}

void OswAppWatchfaceDigital::drawSteps() {
#ifdef OSW_FEATURE_STATS_STEPS
    uint8_t w = 8;
    OswAppWatchface::drawStepHistory(OswUI::getInstance(), (DISP_W / 2) - w * 3.5f, 180, w, w * 4, OswConfigAllKeys::stepsPerDay.get());
#else
    OswHal* hal = OswHal::getInstance();
    uint32_t steps = hal->environment()->getStepsToday();
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(120, 210 - hal->gfx()->getTextOfsetRows(1) / 2);

    hal->gfx()->print(steps);
#endif
}

void OswAppWatchfaceDigital::digitalWatch(short timeZone,uint8_t fontSize, uint8_t dateCoordY, uint8_t timeCoordY) {

    drawDate(timeZone,fontSize, dateCoordY);

    drawTime(timeZone,timeCoordY);
}

void OswAppWatchfaceDigital::onStart() {
    OswAppV2::onStart();
    OswAppWatchface::addButtonDefaults(this->knownButtonStates);
}

void OswAppWatchfaceDigital::onLoop() {
    OswAppV2::onLoop();

    this->needsRedraw = this->needsRedraw or time(nullptr) != this->lastTime; // redraw every second
}

void OswAppWatchfaceDigital::onDraw() {
    OswAppV2::onDraw();
    digitalWatch(OswHal::getInstance()->getTimezoneOffsetPrimary(), 2, 80, 120);

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    drawSteps();
#endif

    this->lastTime = time(nullptr);
}

void OswAppWatchfaceDigital::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state);
    if(OswAppWatchface::onButtonDefaults(*this, id, up, state))
        return; // if the button was handled by the defaults, we are done here
}