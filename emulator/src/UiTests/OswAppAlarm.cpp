#include <iostream>

// ImGUI
#include "imgui.h"

// Test engine
#include "imgui_te_engine.h"
#include "imgui_te_ui.h"
#include "imgui_te_context.h"

#include "../../../include/apps/clock/OswAppAlarm.h"
#include "../Helpers/TestAlarm.h"
#include "globals.h"

extern OswAppAlarm *oswAppAlarm;

namespace {
    void switchToAlarm() {
        main_currentAppIndex = 2;
        main_clockAppIndex = 2;
    }
};

void RegisterAlarmTests(ImGuiTestEngine *e)
{
    ImGuiTest *t = NULL;

    // Just open and test the initial state
    t = IM_REGISTER_TEST(e, "Alarm", "initial state should be IDLE");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        switchToAlarm();

        const auto currentAlarmState = TestAlarm::getState(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarmState, OswAppAlarm::AlarmState::IDLE);
    };

    // Test that BUTTON_2 does not do anything when alarm list is empty
    t = IM_REGISTER_TEST(e, "Alarm", "should do nothing when BUTTON_2 is pressed and there are no active alarms");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 2");

        const auto currentAlarmState = TestAlarm::getState(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarmState, OswAppAlarm::AlarmState::IDLE);
    };

    // Press BUTTON_3 and test the state
    t = IM_REGISTER_TEST(e, "Alarm", "should be in TIME_PICKER state after pressing BUTTON_3");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentAlarmState = TestAlarm::getState(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarmState, OswAppAlarm::AlarmState::TIME_PICKER);
    };

    // Test incrementing time in TIME_PICKER
    t = IM_REGISTER_TEST(e, "Alarm", "should increment time in TIME_PICKER correctly");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 3");
        ctx->ItemClick("Button 3");
        ctx->ItemClick("Button 3");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[3], 3);
    };

    // Test decrementing time in TIME_PICKER
    t = IM_REGISTER_TEST(e, "Alarm", "should decrement time in TIME_PICKER correctly");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 2");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[3], 2);
    };

    // After pressing Next the alarm should be in DAY_PICKER state
    t = IM_REGISTER_TEST(e, "Alarm", "should be time in DAY_PICKER state after pressing Next");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 3");

        const auto currentAlarmState = TestAlarm::getState(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarmState, OswAppAlarm::AlarmState::DAY_PICKER);
    };

    // Test toggling day in DAY_PICKER
    t = IM_REGISTER_TEST(e, "Alarm", "should toggle date in DAY_PICKER correctly");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        auto currentAlarmDays = TestAlarm::getDaysOfWeek(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarmDays[0], true);

        ctx->ItemClick("Button 3");
        currentAlarmDays = TestAlarm::getDaysOfWeek(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarmDays[0], false);
    };

    // Test state after creating a new alarm
    t = IM_REGISTER_TEST(e, "Alarm", "should be in IDLE state after creating a new alarm");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 3");

        const auto currentAlarmState = TestAlarm::getState(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarmState, OswAppAlarm::AlarmState::IDLE);
    };

    // Test that we created one alarm
    t = IM_REGISTER_TEST(e, "Alarm", "should create a new alarm");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        const auto currentAlarms = TestAlarm::getAlarms(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarms.size(), 1);
    };

    // Test that we can go to LIST state when there is at least one alarm
    t = IM_REGISTER_TEST(e, "Alarm", "should be in LIST state when BUTTON_2 is pressed and there are active alarms");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 2");

        const auto currentAlarmState = TestAlarm::getState(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarmState, OswAppAlarm::AlarmState::LIST);
    };

    // Test that we can delete the alarm
    t = IM_REGISTER_TEST(e, "Alarm", "should be able to delete the alarm");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 2");

        const auto currentAlarms = TestAlarm::getAlarms(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarms.size(), 0);
    };
}