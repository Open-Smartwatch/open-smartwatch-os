
#include <gfx_util.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

#include "./apps/watchfaces/OswAppWatchfaceZwilight.h"
#include "fonts/FreeMonoBold9pt7b.h"

#define CENTER_X (DISP_W / 2)
#define CENTER_Y (DISP_H / 2)

void OswAppWatchfaceZwilight::cleanupLuaState() {
    #ifdef OSW_FEATURE_LUA
    if (luaState) {
        lua_close(luaState);
        luaState = NULL;
    }
    #endif
}

inline void OswAppWatchfaceZwilight::printLuaError() {
    #ifdef OSW_FEATURE_LUA
    if (luaState)
        OSW_LOG_E(lua_tostring(luaState, -1));
    #endif
}

bool OswAppWatchfaceZwilight::getTwilights() {

    #ifdef OSW_FEATURE_LUA
    cleanupLuaState();
    luaState = luaL_newstate();

    if (!luaState) {
        OSW_LOG_E("luaL_newstate failed\n");
        return false;
    }

    luaL_openlibs(luaState);
//    halToLua(luaState);

    //Include search paths
    luaL_dostring(luaState, LUA_PACKAGE_CMD);

    if (luaL_dofile(luaState, LUA_APP_PATH "twilight.lua")) {
        printLuaError();
        cleanupLuaState();
        return false;
    }

//    lua_getnumber(lua_getglobal("varname"))

    int luaInitialStackSize = lua_gettop(luaState);

    uint32_t year = 0;
    uint32_t month = 0;
    uint32_t day = 0;
    time_t timeZone = 0;

    hal->updateTimezoneOffsets();
    timeZone = hal->getTimezoneOffsetPrimary();
    hal->getDate(timeZone, &day, &month, &year);

    uint32_t timeZone_offset_h = timeZone / 3600;

    //  setParams(latitude, longitude, timezone, altitude, year, month, day, dst)
    lua_getglobal(luaState, "setParams");

    lua_pushnumber(luaState, 47.51f);
    lua_pushnumber(luaState, 12.09f);
    lua_pushnumber(luaState, timeZone_offset_h);
    lua_pushnumber(luaState, 520.0f);
    lua_pushnumber(luaState, year);
    lua_pushnumber(luaState, month);
    lua_pushnumber(luaState, day);
    lua_pushboolean(luaState, false); // true

    if (lua_pcall(luaState, 8, 0, 0)) {
        printf("xxxxx8\n");
        printLuaError();
        cleanupLuaState();
        return false;
    }

    for (int i = LAST_MIDNIGHT; i <= MIDNIGHT; ++i) {
        lua_getglobal(luaState, "getTimes");
        lua_pushnumber(luaState, i);
        if (lua_pcall(luaState, 1, 1, 0)) {
            printf("xxxx33333333 knock out\n");
            printLuaError();
            cleanupLuaState();
            return false;
        }
        sunTimes[i] = lua_tonumber(luaState, -1);
        lua_pop(luaState, 1);
    }

/*
    lua_getglobal(luaState, "SunTime");
    lua_getfield(luaState, -1, "times");
    for (int i = 1; i <=11; ++i) {
        if ( lua_geti(luaState, -1, i) == LUA_TNUMBER) { // Times
            sunTimes[i] = lua_tonumber(luaState, -1);
            printf("xxxxx SunTime.times[%d]=%f\n", i, sunTimes[i]);
        } else {
            sunTimes[i] = -10000;
        }

    }
    lua_pop(luaState, 11+2);
*/

    if (luaInitialStackSize != lua_gettop(luaState)) {
        OSW_LOG_E("Oh my god, the lua stack has grown from", luaInitialStackSize, "to", lua_gettop(luaState));
    }

    #else 
    #error "LUA not defined"
    #endif

    return true;
}

void OswAppWatchfaceZwilight::displayWeekDay2(const char* weekday) {
    OswHal* hal = OswHal::getInstance();

    char weekday2[3];
    weekday2[0] = weekday[0];
    weekday2[1] = weekday[1];
    weekday2[2] = '\0';
    hal->gfx()->print(weekday2);
}

void OswAppWatchfaceZwilight::drawWatch(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second) {
    OswHal* hal = OswHal::getInstance();

    uint16_t day_color =        rgb565(0, 50, 255);
    uint16_t civil_color =      rgb565(0, 50 * .66, 255 * .66);
    uint16_t nautic_color =     rgb565(0, 50 * .40, 255 * .40);
    uint16_t astronomic_color = rgb565(0, 50 * .35, 255 * .35);
    uint16_t night_color =      rgb565(0, 50 * 0.3, 255 * 0.3);
    uint16_t sun_color =        rgb565(255, 255, 10);


    hal->gfx()->setTextSize(1);
    hal->gfx()->setFont(&FreeMonoBold9pt7b);

    const char* weekday = hal->getLocalWeekday();

    // night
    hal->gfx()->drawArcAA(CENTER_X, CENTER_Y, 270 - sunTimes[ASTRONOMIC_DAWN]*(360/24), 270 - sunTimes[ASTRONOMIC_DUSK]*(360/24), 103, 5, night_color, STRAIGHT_END);

    // astronomical
    hal->gfx()->drawArcAA(CENTER_X, CENTER_Y, 270 - sunTimes[NAUTIC_DAWN]*(360/24), 270 - sunTimes[ASTRONOMIC_DAWN]*(360/24), 103, 5, astronomic_color, STRAIGHT_END);
    hal->gfx()->drawArcAA(CENTER_X, CENTER_Y, 270 - sunTimes[ASTRONOMIC_DUSK]*(360/24), 270 - sunTimes[NAUTIC_DUSK]*(360/24), 103, 5, astronomic_color, STRAIGHT_END);

    // nautical
    hal->gfx()->drawArcAA(CENTER_X, CENTER_Y, 270 - sunTimes[CIVIL_DAWN]*(360/24), 270 - sunTimes[NAUTIC_DAWN]*(360/24), 103, 5, nautic_color, STRAIGHT_END);
    hal->gfx()->drawArcAA(CENTER_X, CENTER_Y, 270 - sunTimes[NAUTIC_DUSK]*(360/24), 270 - sunTimes[CIVIL_DUSK]*(360/24), 103, 5, nautic_color, STRAIGHT_END);

    // civil
    hal->gfx()->drawArcAA(CENTER_X, CENTER_Y, 270 - sunTimes[SUN_RISE]*(360/24), 270 - sunTimes[CIVIL_DAWN]*(360/24), 103, 5, civil_color, STRAIGHT_END);
    hal->gfx()->drawArcAA(CENTER_X, CENTER_Y, 270 - sunTimes[CIVIL_DUSK]*(360/24), 270 - sunTimes[SUN_SET]*(360/24), 103, 5, civil_color, STRAIGHT_END);

    // daytime
    hal->gfx()->drawArcAA(CENTER_X, CENTER_Y, 270 - sunTimes[SUN_SET]*(360/24), 270 - sunTimes[SUN_RISE]*(360/24), 103, 5, day_color, STRAIGHT_END);


    // outer indices
    //hal->gfx()->drawNTicksAA(CENTER_X, CENTER_Y, 120, 110, 24*2, ui->getForegroundDimmedColor(), 2);
    hal->gfx()->drawNTicksAA(CENTER_X, CENTER_Y, 120, 110, 24, ui->getForegroundDimmedColor());

    // Time indicator
    int16_t day_indicator_angle;
    day_indicator_angle = 270 - 360/24 * (hour + minute / 60.0f);
    while (day_indicator_angle > 360)
        day_indicator_angle -= 360;
    while (day_indicator_angle < 0)
        day_indicator_angle += 360;
    hal->gfx()->drawCircleAA(CENTER_X + 115*cosf_tlu(day_indicator_angle), CENTER_Y - 115*sinf_tlu(day_indicator_angle),
                              5, 2, sun_color);

    // sunTimes
    // Attention MIDNIGHT is for today; LAST_MIDNIGHT was for yesterday
    // We only want to show one, as they differ a bit.
    for (int i = MIDNIGHT; i>=LAST_MIDNIGHT+1; --i) {
        day_indicator_angle = 270 - sunTimes[i] * (360/24);
        while (day_indicator_angle > 360)
            day_indicator_angle -= 360;
        while (day_indicator_angle < 0)
            day_indicator_angle += 360;

        int16_t x0, y0, x1, y1;
        int16_t r1 = 120;
        int16_t r2 = 105;
        x0 = CENTER_X + r1*cosf_tlu(day_indicator_angle);
        y0 = CENTER_Y - r1*sinf_tlu(day_indicator_angle);
        x1 = CENTER_X + r2*cosf_tlu(day_indicator_angle);
        y1 = CENTER_Y - r2*sinf_tlu(day_indicator_angle);
        hal->gfx()->drawLineAA(x0, y0, x1, y1, sun_color);
    }

    // Day complication
    hal->gfx()->setTextColor(dimColor(ui->getForegroundColor(), 25));

    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(120-30+2, 120-40);
    OswAppWatchfaceZwilight::displayWeekDay2(weekday);

    // Day of Month complication
    hal->gfx()->print(" ");
    hal->gfx()->printDecimal(day, 2);

    // Steps complication

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    {   // draw step arc
        uint32_t steps = hal->environment()->getStepsToday();
        uint32_t stepsTarget = OswConfigAllKeys::stepsPerDay.get();

        #ifdef OSW_EMULATOR
        steps = 6666;
        #endif

        int16_t radius = 48;
        int16_t lw = 4;
//        int16_t sa = 180+55;
//        int16_t ea = 180-55;
        int16_t sa = 360;
        int16_t ea = 180;
        int16_t angle_val = sa - (sa-ea) * (float)min(steps, stepsTarget) / (float)stepsTarget;
        uint16_t color = day_color;
        uint16_t dimmed_color = night_color;
        hal->gfx()->drawCircleAA(CENTER_X, CENTER_Y, radius + lw, lw*2, dimmed_color, ea, angle_val);

        hal->gfx()->drawCircleAA(CENTER_X, CENTER_Y, radius + lw, lw*2, color, angle_val, sa);

        int x, y;

        x = CENTER_X + cosf_tlu(ea) * radius;
        y = CENTER_Y - sinf_tlu(ea) * radius;
        hal->gfx()->drawCircleAA(x, y, lw, 0, dimmed_color);

        x = CENTER_X + cosf_tlu(angle_val) * radius;
        y = CENTER_Y - sinf_tlu(angle_val) * radius;
        hal->gfx()->drawCircleAA(x, y, lw-1, 0, color);

        x = CENTER_X + cosf_tlu(sa) * radius;
        y = CENTER_Y - sinf_tlu(sa) * radius;
        hal->gfx()->drawCircleAA(x, y, lw, 0, color);

        hal->gfx()->setTextColor(day_color);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(CENTER_X, CENTER_Y + radius + 22);
        
        hal->gfx()->print(steps);
    }
#endif

    // Now the hands

    const int16_t clock_r = 95;

    // Indices
    hal->gfx()->drawCircleAA(CENTER_X, CENTER_Y, clock_r, 5, rgb565(35, 35, 35));
    hal->gfx()->drawCircleAA(CENTER_X, CENTER_Y, clock_r - 5, 5, rgb565(30, 30, 30));
    hal->gfx()->drawMinuteTicks(CENTER_X, CENTER_Y, clock_r, clock_r - 5, ui->getForegroundDimmedColor(), true);
    hal->gfx()->drawHourTicks(CENTER_X, CENTER_Y, clock_r, clock_r - 10, ui->getForegroundColor(), true);

    // Hours
    hal->gfx()->drawThickTick(CENTER_X, CENTER_Y,  0, 16, (int)(360.0f / 12.0f * (hour + minute / 60.0f)), 3, ui->getForegroundColor(), true, STRAIGHT_END);
    hal->gfx()->drawThickTick(CENTER_X, CENTER_Y, 16, 60, (int)(360.0f / 12.0f * (hour + minute / 60.0f)), 7, ui->getForegroundColor(), true);

    // Minutes
    hal->gfx()->drawThickTick(CENTER_X, CENTER_Y,  0, 16, (int)(360.0f / 60.0f * (minute + second / 60.0f)), 3, ui->getForegroundColor(), true, STRAIGHT_END);
    hal->gfx()->drawThickTick(CENTER_X, CENTER_Y, 16, clock_r - 5, (int)(360.0f / 60.0f * (minute + second / 60.0f)), 7, ui->getForegroundColor(), true);

    // Seconds
    hal->gfx()->fillCircleAA(CENTER_X, CENTER_Y, 6, ui->getDangerColor());
    hal->gfx()->drawThickTick(CENTER_X, CENTER_Y, -16, clock_r - 5, 360 / 60 * second, 3, ui->getDangerColor(), true);
}

void OswAppWatchfaceZwilight::onStart() {
    OswAppV2::onStart();

    this->viewFlags = OswAppV2::ViewFlags::NO_OVERLAYS; // no overlay for this watchface

#ifndef OSW_EMULATOR
    unsigned long old_micros = micros();
#else
    unsigned long old_millis = millis();
#endif

    hal->updateTimezoneOffsets();

    this->getTwilights();

#ifndef OSW_EMULATOR
    unsigned long ms_for_onDraw = (micros()-old_micros)/1000;
#else
    unsigned long ms_for_onDraw = millis()-old_millis;
#endif
    OSW_LOG_I("Time to start ", ms_for_onDraw, " ms");

}

void OswAppWatchfaceZwilight::onLoop() {
    OswAppV2::onLoop();

    this->needsRedraw = this->needsRedraw or time(nullptr) != this->lastTime; // redraw every second
}

void OswAppWatchfaceZwilight::onDraw() {
    OswAppV2::onDraw();

#ifndef OSW_EMULATOR
    unsigned long old_micros = micros();
#else
    unsigned long old_millis = millis();
#endif

    uint32_t year = 0;
    uint32_t month = 0;
    uint32_t day = 0;
    uint32_t hour = 0;
    uint32_t minute = 0;
    uint32_t second = 0;
    time_t timeZone = 0;

    timeZone = hal->getTimezoneOffsetPrimary();
    hal->getDate(timeZone, &day, &month, &year);
    hal->getTime(timeZone, &hour, &minute, &second);

    drawWatch(year, month, day, hour, minute, second);
#ifndef OSW_EMULATOR
    unsigned long ms_for_onDraw = (micros()-old_micros)/1000;
#else
    unsigned long ms_for_onDraw = millis()-old_millis;
#endif
    OSW_LOG_I("Time to draw ", ms_for_onDraw, " ms");

    this->lastTime = time(nullptr);
}

void OswAppWatchfaceZwilight::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state);
}

void OswAppWatchfaceZwilight::onStop() {
    OswAppV2::onStop();

    hal->gfx()->clearFont();
}

const char* OswAppWatchfaceZwilight::getAppId() {
    return OswAppWatchfaceZwilight::APP_ID;
}

const char* OswAppWatchfaceZwilight::getAppName() {
    return LANG_ZWILIGHT;
}
