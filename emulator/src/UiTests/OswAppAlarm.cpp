#include <iostream>

// ImGUI
#include "imgui.h"

// Test engine
#include "imgui_te_engine.h"
#include "imgui_te_ui.h"
#include "imgui_te_context.h"

#include "../../../include/apps/clock/OswAppAlarm.h"
#include "../Helpers/TestAlarm.h"
#include "../Helpers/TestSwitcher.h"
#include "globals.h"

extern OswAppSwitcher clockAppSwitcher; // Used to get OswAppAlarm instance

namespace
{
    OswAppAlarm *oswAppAlarm;

    void switchToAlarm()
    {
        main_currentAppIndex = 2;
        main_clockAppIndex = 2;
    }

    void setupTests(ImGuiTestContext *ctx) {
        oswAppAlarm = TestSwitcher::getAlarm();
        switchToAlarm();

        if (TestAlarm::getState(*oswAppAlarm) != OswAppAlarm::AlarmState::IDLE) {
            TestAlarm::reset(*oswAppAlarm);
        }


        // TODO: remove alarms in a different way
        // Deleting alarms this way is tested later, so at this point we don't know if deleting works correctly
        while (TestAlarm::getAlarms(*oswAppAlarm).size() > 0) {
            ctx->SetRef("Buttons");
            ctx->ItemClick("Button 2");
        }
    }
};

// Main function with all alarm tests
void RegisterAlarmTests(ImGuiTestEngine *e)
{
    ImGuiTest *t = NULL;

    // Just open and test the initial state
    t = IM_REGISTER_TEST(e, "Alarm", "initial state should be IDLE");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        setupTests(ctx);

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
    t = IM_REGISTER_TEST(e, "Alarm", "should be in time picker state after pressing BUTTON_3");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentAlarmState = TestAlarm::getState(*oswAppAlarm);
        IM_CHECK_EQ(currentAlarmState, OswAppAlarm::AlarmState::TIME_PICKER);
    };

    // Test decrementing first digit
    t = IM_REGISTER_TEST(e, "Alarm", "should decrement first digit correctly ({0}0:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 2");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[0], 2);
    };

    // Test incrementing first digit
    t = IM_REGISTER_TEST(e, "Alarm", "should increment first digit correctly ({0}0:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[0], 0);
    };

    // Test decrementing second digit
    t = IM_REGISTER_TEST(e, "Alarm", "should decrement second digit correctly (0{0}:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 2");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[1], 9);
    };

    // Test incrementing second digit
    t = IM_REGISTER_TEST(e, "Alarm", "should increment second digit correctly (0{0}:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[1], 0);
    };

    // Test decrementing second digit when the first digit is 2
    t = IM_REGISTER_TEST(e, "Alarm", "should decrement second digit correctly (2{0}:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        
        // Set first digit to 2
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 2");

        // Decrement second digit
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 2");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[1], 3);
    };

    // Test incrementing second digit when the first digit is 2
    t = IM_REGISTER_TEST(e, "Alarm", "should increment second digit correctly (2{0}:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[1], 0);

        // Set first digit to 0
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 3");
        ctx->ItemClick("Button 1");
    };

    // Test decrementing third digit
    t = IM_REGISTER_TEST(e, "Alarm", "should decrement third digit correctly (00:{0}0)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 2");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[2], 5);
    };

    // Test incrementing third digit
    t = IM_REGISTER_TEST(e, "Alarm", "should increment third digit correctly (00:{0}0)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[2], 0);
    };

    // Test decrementing fourth digit
    t = IM_REGISTER_TEST(e, "Alarm", "should decrement fourth digit correctly (00:0{0})");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 2");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[3], 9);
    };

    // Test incrementing fourth digit
    t = IM_REGISTER_TEST(e, "Alarm", "should increment fourth digit correctly (00:0{0})");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentAlarmTimestamp = TestAlarm::getTimestamp(*oswAppAlarm);

        IM_CHECK_EQ(currentAlarmTimestamp[3], 0);
    };

    // After pressing "Next" the alarm should be in DAY_PICKER state
    t = IM_REGISTER_TEST(e, "Alarm", "should be in day picker state after pressing 'Next'");
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
    t = IM_REGISTER_TEST(e, "Alarm", "should toggle dates correctly");
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
    t = IM_REGISTER_TEST(e, "Alarm", "should show delete options when BUTTON_2 is pressed and there are active alarms");
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