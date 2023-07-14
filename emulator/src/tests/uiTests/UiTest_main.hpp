#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <algorithm>
#include <chrono>
#include <thread>
#include <globals.h>
#include <osw_ui.h>

// ImGUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

// Test engine
#include "imgui_te_engine.h"
#include "imgui_te_ui.h"
#include "imgui_te_context.h"
#include "imgui_te_exporters.h"

#include "../../include/Emulator.hpp"
#include "../helpers/TestEmulator.h"
#include "RegisterUiTests.h"

int runUiTests()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        printf("error initializing SDL: %s\n", SDL_GetError());

    // Create and run the emulator
    std::unique_ptr<OswEmulator> oswEmu = std::make_unique<OswEmulator>(false);
    OswEmulator::instance = oswEmu.get();

    // Setup test engine
    ImGuiTestEngine *engine = ImGuiTestEngine_CreateContext();
    ImGuiTestEngineIO &test_io = ImGuiTestEngine_GetIO(engine);
    test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
    test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;
    test_io.ConfigRunSpeed = ImGuiTestRunSpeed_Fast; // Default to fastest mode

    // Start test engine
    ImGuiTestEngine_Start(engine, ImGui::GetCurrentContext());
    ImGuiTestEngine_InstallDefaultCrashHandler();

    // Register tests
    std::for_each(RegisterUiTests.begin(), RegisterUiTests.end(), [engine](auto RegisterTest){ RegisterTest(engine); });

    // Main loop
    bool aborted = false;

    // Skip tutorial
    TestEmulator::newFrame();
    TestEmulator::renderGUIFrameEmulator();
    TestEmulator::wakeFromDeepSleep();
    TestEmulator::drawEmulator();
    ImGui::Render();
    OswUI::getInstance()->setRootApplication(&OswGlobals::main_mainDrawer);

    while (!aborted)
    {
        // Handle exit event
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                aborted = true;
                break;
            }
        }

        SDL_RenderClear(TestEmulator::getMainRenderer());

        // Emulator new frame
        TestEmulator::newFrame();

        TestEmulator::renderGUIFrameEmulator();

        TestEmulator::wakeFromDeepSleep();

        TestEmulator::drawEmulator();

        // Show "ImGUI Test Engine" window
        ImGuiTestEngine_ShowTestEngineWindows(engine, NULL);

        // Render
        ImGui::Render();

        // Draw ImGUI content
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

        // Update the window now with the content of the display
        SDL_RenderPresent(TestEmulator::getMainRenderer());
        SDL_UpdateWindowSurface(TestEmulator::getMainWindow());

        // Post-swap handler is REQUIRED in order to support screen capture
        ImGuiTestEngine_PostSwap(engine);
    }

    // Shutdown
    ImGuiTestEngine_Stop(engine);

    oswEmu.reset();

    // IMPORTANT: we need to destroy the Dear ImGui context BEFORE the test engine context, so .ini data may be saved.
    ImGuiTestEngine_DestroyContext(engine);

    OswEmulator::instance = nullptr;

    SDL_Quit();

    return 0;
}