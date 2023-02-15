#include <iostream>

// ImGUI
#include "imgui.h"

// Test engine
#include "imgui_te_engine.h"
#include "imgui_te_ui.h"
#include "imgui_te_context.h"

#include "../../../include/apps/clock/OswAppTimer.h"
#include "../Helpers/TestTimer.h"
#include "../Helpers/TestSwitcher.h"
#include "globals.h"


namespace {
    OswAppTimer *oswAppTimer;
    
    void switchToTimer() {
        main_currentAppIndex = 2;
        main_clockAppIndex = 1;
    }

    void setupTests() {
        oswAppTimer = TestSwitcher::getTimer();
        switchToTimer();

        if (TestTimer::getState(*oswAppTimer) != OswAppTimer::TimerState::IDLE) {
            TestTimer::reset(*oswAppTimer);
        }
    }
};

// Main function with all timer tests
void RegisterTimerTests(ImGuiTestEngine *e)
{
    ImGuiTest *t = NULL;

    // Just open and test the initial state
    t = IM_REGISTER_TEST(e, "Timer", "initial state should be IDLE");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        setupTests();

        const auto currentTimerState = TestTimer::getState(*oswAppTimer);
        IM_CHECK_EQ(currentTimerState, OswAppTimer::TimerState::IDLE);
    };

    // Press BUTTON_3 and test the state
    t = IM_REGISTER_TEST(e, "Timer", "should be in SET_TIMER_SCREEN state after pressing BUTTON_3");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentTimerState = TestTimer::getState(*oswAppTimer);
        IM_CHECK_EQ(currentTimerState, OswAppTimer::TimerState::SET_TIMER_SCREEN);
    };

    // Press BUTTON_1 to move to the one digit right
    t = IM_REGISTER_TEST(e, "Timer", "after pressing BUTTON_1 should move to the next digit");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");

        const auto currentDigit = TestTimer::getStep(*oswAppTimer);
        IM_CHECK_EQ(currentDigit, 1);

        // Return to the first digit to continue testing
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
    };

    // Decrement the first digit
    t = IM_REGISTER_TEST(e, "Timer", "should decrement first digit correctly ({0}0:00:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 2");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[0], 9);
    };

    // Increment the first digit
    t = IM_REGISTER_TEST(e, "Timer", "should increment first digit correctly ({0}0:00:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[0], 0);
    };

    // Decrement the second digit
    t = IM_REGISTER_TEST(e, "Timer", "should decrement second digit correctly (0{0}:00:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 2");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[1], 9);
    };

    // Increment the second digit
    t = IM_REGISTER_TEST(e, "Timer", "should increment second digit correctly (0{0}:00:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[1], 0);
    };

    // Decrement the third digit
    t = IM_REGISTER_TEST(e, "Timer", "should decrement third digit correctly (00:{0}0:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 2");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[2], 5);
    };

    // Increment the third digit
    t = IM_REGISTER_TEST(e, "Timer", "should increment third digit correctly (00:{0}0:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[2], 0);
    };

    // Decrement the fourth digit
    t = IM_REGISTER_TEST(e, "Timer", "should decrement fourth digit correctly (00:0{0}:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 2");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[3], 9);
    };

    // Increment the fourth digit
    t = IM_REGISTER_TEST(e, "Timer", "should increment fourth digit correctly (00:0{0}:00)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[3], 0);
    };

    // Decrement the fifth digit
    t = IM_REGISTER_TEST(e, "Timer", "should decrement fifth digit correctly (00:00:{0}0)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 2");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[4], 5);
    };

    // Increment the fifth digit
    t = IM_REGISTER_TEST(e, "Timer", "should increment fifth digit correctly (00:00:{0}0)");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[4], 0);
    };

    // Decrement the sixth digit
    t = IM_REGISTER_TEST(e, "Timer", "should decrement sixth digit correctly (00:00:0{0})");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 2");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[5], 9);
    };

    // Increment the sixth digit
    t = IM_REGISTER_TEST(e, "Timer", "should increment sixth digit correctly (00:00:0{0})");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentTimestamp = TestTimer::getTimestamp(*oswAppTimer);
        IM_CHECK_EQ(currentTimestamp[5], 0);
    };

    // Start the timer and test the state
    t = IM_REGISTER_TEST(e, "Timer", "should be in RUNNING state after starting");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 2");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 1");
        ctx->ItemClick("Button 3");

        const auto currentTimerState = TestTimer::getState(*oswAppTimer);
        IM_CHECK_EQ(currentTimerState, OswAppTimer::TimerState::RUNNING);
    };

    // Pause the timer and test the state
    t = IM_REGISTER_TEST(e, "Timer", "should be in PAUSED state after pausing");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->ItemClick("Button 3");

        const auto currentTimerState = TestTimer::getState(*oswAppTimer);
        IM_CHECK_EQ(currentTimerState, OswAppTimer::TimerState::PAUSED);

        // Resume the timer after test
        ctx->ItemClick("Button 3");
    };

    // Start the timer and test the reset
    t = IM_REGISTER_TEST(e, "Timer", "should reset correctly");
    t->TestFunc = [](ImGuiTestContext *ctx)
    {
        ctx->SetRef("Buttons");
        ctx->MouseMove("Button 2"); // Move to the reset button
        ctx->MouseDown(0);
        ctx->SleepNoSkip(3.0f, 0.01f);
        ctx->MouseUp(0);

        const auto currentTimerState = TestTimer::getState(*oswAppTimer);
        IM_CHECK_EQ(currentTimerState, OswAppTimer::TimerState::IDLE);

        const auto timerLeftSec = TestTimer::getLeftSec(*oswAppTimer);
        IM_CHECK_EQ(timerLeftSec, std::chrono::seconds{0});
    };
}