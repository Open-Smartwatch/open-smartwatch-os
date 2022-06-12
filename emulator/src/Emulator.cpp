#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "../../include/config_defaults.h" // For the display size

#include "../include/Display.h"
#include "../include/Emulator.hpp"

OswEmulator* OswEmulator::instance = nullptr;

OswEmulator::OswEmulator() {
    // Init the SDL window and renderer
    this->mainWindow = SDL_CreateWindow("OSW-OS Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISP_W, DISP_H, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    assert(this->mainWindow && "Never fail window creation");
    this->mainRenderer = SDL_CreateRenderer(this->mainWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    assert(this->mainRenderer && "Never fail renderer creation");
    fakeDisplayInstance = std::make_unique<FakeDisplay>(DISP_W, DISP_H, this->mainWindow, this->mainRenderer);

    // Create ImGUI context and initialize
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(this->mainWindow, this->mainRenderer);
    ImGui_ImplSDLRenderer_Init(this->mainRenderer);

    this->enterSleep(true); // The intial state of the watch!
}

OswEmulator::~OswEmulator() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(this->mainWindow);
}

void OswEmulator::run() {

    bool show_demo_window = true;
    while(this->running) {
        SDL_RenderClear(this->mainRenderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT)
                this->running = false;
        }

        // Prepare ImGUI for the next frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        if(show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        ImGui::Render();

        // Revive system after deep sleep as needed
        if(this->deepSleeped) {
            setup();
            this->deepSleeped = false;
        }

        // Next OS step
        try {
            loop();
        } catch(EmulatorSleep& e) {
            // Ignore it :P
        }

        // Draw ImGUI content
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

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