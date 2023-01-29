#pragma once

#include "../../include/Emulator.hpp"

// This is a friend class of OswEmulator. It is needed for UI tests of OswEmulator
class TestEmulator
{
public:
    static void newFrame()
    {
        OswEmulator::instance->newFrame();
    }

    static void wakeFromDeepSleep()
    {
        OswEmulator::instance->wakeFromDeepSleep();
    }

    static void drawEmulator()
    {
        OswEmulator::instance->drawEmulator();
    }

    static void renderGUIFrameEmulator()
    {
        OswEmulator::instance->renderGUIFrameEmulator();
    }

    static SDL_Renderer *getMainRenderer()
    {
        return OswEmulator::instance->mainRenderer;
    }
    static SDL_Window *getMainWindow()
    {
        return OswEmulator::instance->mainWindow;
    }
};