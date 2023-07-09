#pragma once

#include "imgui_te_engine.h"
#include <vector>
#include <functional>

// Declaration of UI tests
// IMPORTANT: If you add new UI test, add it's declaration here
void RegisterTimerTests(ImGuiTestEngine *e);
void RegisterAlarmTests(ImGuiTestEngine *e);

// Vector of all UI tests. These tests will be displayed in the testing engine
// IMPORTANT: If you added UI test declaration above, then you should also pointer to your function to this vector
std::vector<std::function<void(ImGuiTestEngine*)>> RegisterUiTests{
    &RegisterTimerTests,
    &RegisterAlarmTests
};