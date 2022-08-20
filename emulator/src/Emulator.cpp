#include <chrono>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "../../include/config_defaults.h" // For the display size

#include "../include/Display.h"
#include "../include/Emulator.hpp"

#include "osw_ui.h"
#include "osw_config.h"
#include "osw_config_keys.h"

OswEmulator* OswEmulator::instance = nullptr;

OswEmulator::OswEmulator() {
    // Init the SDL window and renderer
    this->mainWindow = SDL_CreateWindow("smartwatch virtual device", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISP_W + this->guiPadding + this->guiWidth + this->guiPadding, DISP_H, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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
}

OswEmulator::~OswEmulator() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(this->mainWindow);
}

void OswEmulator::run() {
int lastFrame = 0;
    while(this->running) {
        std::chrono::time_point loopStart = std::chrono::system_clock::now();
        SDL_RenderClear(this->mainRenderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT)
                this->running = false;
        }
        this->renderGUIFrame();

        // Revive system after deep sleep as needed
        if(this->cpustate == CPUState::deepSleep and (this->wakeUpNow or this->autoWakeUp)) {
            setup();
            this->cpustate = CPUState::active;
            this->wakeUpNow = false;
            OswUI::getInstance()->mEnableTargetFPS = false; // Disable FPS limiter of the UI iteself
        }

        // Next OS step
        try {
            if(this->cpustate == CPUState::active) {
                std::chrono::time_point start = std::chrono::system_clock::now();
                loop();
                std::chrono::time_point end = std::chrono::system_clock::now();
                for(size_t i = 1; i < this->timesLoop.size(); ++i)
                    this->timesLoop.at(this->timesLoop.size() - i) = this->timesLoop.at(this->timesLoop.size() - i - 1);
                this->timesLoop.front() = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            }
        } catch(EmulatorSleep& e) {
            // Ignore it :P
        }

        // Draw ImGUI content
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

        // Update the window now with the content of the display
        SDL_RenderPresent(this->mainRenderer);
        SDL_UpdateWindowSurface(this->mainWindow);

        // Update render FPS
        std::chrono::time_point loopEnd = std::chrono::system_clock::now();
        for(size_t i = 1; i < this->timesFrames.size(); ++i)
            this->timesFrames.at(this->timesFrames.size() - i) = this->timesFrames.at(this->timesFrames.size() - i - 1);
        const float frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(loopEnd - loopStart).count();
        if(frameTime > 0)
            this->timesFrames.front() = 1000 / frameTime;
    }
}

void OswEmulator::exit() {
    this->running = false;
}

void OswEmulator::reboot() {
    this->cpustate = CPUState::deepSleep; // This is the best we can do, as we can't really reset any global variables...
}

void OswEmulator::enterSleep(bool toDeepSleep) {
    this->cpustate = toDeepSleep ? CPUState::deepSleep : CPUState::lightSpleep;
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
    return this->cpustate == CPUState::deepSleep;
}

void OswEmulator::addGUIHelp(const char* msg) {
    ImGui::SameLine(); ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(msg);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void OswEmulator::renderGUIFrame() {
    // Prepare ImGUI for the next frame
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Emulator control
    ImGui::Begin("Emulator");
    ImGui::Text("CPU: %s", this->cpustate == CPUState::active ? "Active" : (this->cpustate == CPUState::lightSpleep ? "Light Sleep" : "Deep Sleep"));
    ImGui::PlotLines("FPS", (float*) this->timesFrames.data(), this->timesFrames.size());
    ImGui::PlotLines("loop()", (float*) this->timesLoop.data(), this->timesLoop.size());
    ImGui::Separator();
    ImGui::Checkbox("Keep-Awake", &this->autoWakeUp);
    this->addGUIHelp("This will always wakeup the watch for the next frame.");
    if(!this->autoWakeUp and this->cpustate != CPUState::active and ImGui::Button("Wake Up"))
        this->wakeUpNow = true;
    ImGui::End();

    // Button Control
    ImGui::Begin("Buttons");
    if(ImGui::Button("Button PWR")) {
        this->enterSleep(true);
        this->wakeUpNow = true;
    }
    this->addGUIHelp("This button will interrupt the power to the CPU and reset the OS (as from deep sleep).");
    ImGui::Button("Button 1");
    this->setButton(0, ImGui::IsItemActive());
    ImGui::Button("Button 2");
    this->setButton(1, ImGui::IsItemActive());
    ImGui::Button("Button 3");
    this->setButton(2, ImGui::IsItemActive());
    ImGui::End();

    if(OswHal::getInstance()->devices and OswHal::getInstance()->devices->virtualDevice) {
        // Virtual Sensors
        ImGui::Begin("Virtual Sensors");
        ImGui::InputFloat("Temperature", &OswHal::getInstance()->devices->virtualDevice->values.temperature, 1, 10);
        ImGui::InputFloat("Pressure", &OswHal::getInstance()->devices->virtualDevice->values.pressure, 1, 10);
        ImGui::InputFloat("Humidity", &OswHal::getInstance()->devices->virtualDevice->values.humidity, 1, 10);
        ImGui::InputFloat("Acceleration X", &OswHal::getInstance()->devices->virtualDevice->values.accelerationX, 0.1f, 10);
        ImGui::InputFloat("Acceleration Y", &OswHal::getInstance()->devices->virtualDevice->values.accelerationY, 0.1f, 10);
        ImGui::InputFloat("Acceleration Z", &OswHal::getInstance()->devices->virtualDevice->values.accelerationZ, 0.1f, 10);
        ImGui::InputInt("Magnetometer Azimuth", &OswHal::getInstance()->devices->virtualDevice->values.magnetometerAzimuth, 1, 10);
        ImGui::InputInt("Steps", (int*) &OswHal::getInstance()->devices->virtualDevice->values.steps, 1, 10); // Warning - negative values will cause an underflow... ImGui has no conventient way of limiting the input range...
        ImGui::End();
    }

    // Web-interface
    ImGui::Begin("Web-interface");
    // Magic code to represent the osywConfig keys

    for (size_t keyId = 0; keyId < oswConfigKeysCount; ++keyId) {
      const OswConfigKey* key = oswConfigKeys[keyId];
      if(!strcmp(key->type,"C")){ // CheckBox
          if (!strcmp(key->id, "g")) {
            ImGui::Checkbox(key->label, &this->checkBoxTimeFormat);
            this->addGUIHelp(key->help);
          }
          else if (!strcmp(key->id, "s3")) {
            ImGui::Checkbox(key->label, &this->checkBoxSettingDisplayOverlays);
            this->addGUIHelp(key->help);
          }
          else if (!strcmp(key->id, "s4")) {
            ImGui::Checkbox(key->label, &this->checkBoxSettingDisplayOverlaysOnWatchScreen);
            this->addGUIHelp(key->help);
          }

      } 
    }
    ImGui::Button("Save");
    if(ImGui::IsItemActive()){
      OswConfig::getInstance()->enableWrite();
      OswConfigAllKeys::timeFormat.set(checkBoxTimeFormat);
      OswConfigAllKeys::settingDisplayOverlays.set(checkBoxSettingDisplayOverlays);
      OswConfigAllKeys::settingDisplayOverlaysOnWatchScreen.set(checkBoxSettingDisplayOverlaysOnWatchScreen);
      OswConfig::getInstance()->disableWrite();
    }
    ImGui::End();

    // Render the gui in memory
    ImGui::Render();
}
