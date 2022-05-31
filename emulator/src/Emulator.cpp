#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "../include/Display.h"

#include "../include/Emulator.hpp"

OswEmulator* OswEmulator::instance = nullptr;

OswEmulator::OswEmulator() {
    this->mainWindow = SDL_CreateWindow("OSW-OS Emulator Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256, 256, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    assert(this->mainWindow && "Never fail window creation");
    this->mainRenderer = SDL_CreateRenderer(this->mainWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    assert(this->mainRenderer && "Never fail renderer creation");
    fakeDisplayInstance = std::make_unique<FakeDisplay>(256, 256, this->mainWindow, this->mainRenderer);

IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

ImGui::StyleColorsDark();
ImGui_ImplSDL2_InitForSDLRenderer(this->mainWindow, this->mainRenderer);
ImGui_ImplSDLRenderer_Init(this->mainRenderer);

    this->enterSleep(true); // The intial state of the watch!
}

OswEmulator::~OswEmulator() {
    SDL_DestroyWindow(this->mainWindow);
}

void OswEmulator::run() {

    while(this->running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                this->running = false;
        }

        if(this->deepSleeped) {
            setup();
            this->deepSleeped = false;
        }
        try {
            loop();
        } catch(EmulatorSleep& e) {
            // Ignore it :P
        }

        // Update the window now with the content of the display
        SDL_RenderPresent(this->mainRenderer);
    }
}

void OswEmulator::exit() {
    this->running = false;
}

void OswEmulator::enterSleep(bool toDeepSleep) {
    this->deepSleeped = toDeepSleep;
}

void OswEmulator::setButton(unsigned id, bool state) {
    this->buttons.at(id) = state;
};

bool OswEmulator::getButton(unsigned id) {
    return this->buttons.at(id);
};

uint8_t OswEmulator::getBatteryRaw() {
    return this->batRaw;
}

bool OswEmulator::isCharging() {
    return this->charging;
}

bool OswEmulator::fromDeepSleep() {
    return this->deepSleeped;
}