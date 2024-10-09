#include <chrono>
#include <filesystem>
#include <signal.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "misc/cpp/imgui_stdlib.h"

#include "../../include/config_defaults.h" // For the display size

#include "../include/Display.h"
#include "../include/Emulator.hpp"

#include "globals.h"
#include "osw_ui.h"
#include "osw_config.h"
#include "osw_config_keys.h"
#include "services/OswServiceManager.h"

OswEmulator* OswEmulator::instance = nullptr;

static void shutdownEmulatorByInterruptSignal(int s) {
    if(s != SIGINT)
        return; // Why? Just why?!
    static bool called = false;
    if(!called) {
        OSW_LOG_I("Received signal SIGINT shutting down emulator...");
        if(OswEmulator::instance != nullptr)
            OswEmulator::instance->exit();
    } else {
        // Uoh, the emulator has a problem shutting down, so we just exit the process
        OSW_LOG_W("Received signal SIGINT again, terminating process...");
        std::terminate();
    }
    called = true;
}

OswEmulator::OswEmulator(bool softwareRenderer, bool headless, std::string configPath, std::string imguiPath): isHeadless(headless), isSoftwareRenderer(softwareRenderer) {
    // Initialize variables
    for(size_t i = 0; i < BTN_NUMBER; i++)
        this->buttonCheckboxes[i] = false;

    // Load emulator config
    this->configPath = configPath;
    this->imguiPath = imguiPath;
    this->config = Jzon::object();
    if(std::filesystem::exists(this->configPath)) {
        std::ifstream configStream(this->configPath, std::ios::in);
        this->config = Jzon::Parser().parseStream(configStream);
        configStream.close();
    }

    // Reset the config to default values (so we don't have to specify them on every time)
    this->autoWakeUp = this->config.get("autoWakeUp").toBool(true);

    const int width = this->config.get("window").get("width").toInt(DISP_W + this->guiPadding + this->guiWidth + this->guiPadding);
    const int height = this->config.get("window").get("height").toInt(DISP_H);
    if(this->isHeadless) {
        this->mainSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        assert(this->mainSurface && "Never fail surface creation");
        assert(this->isSoftwareRenderer && "Software renderer is required in headless mode");
        this->mainRenderer = SDL_CreateSoftwareRenderer(this->mainSurface);
    } else {
        // Init the SDL window and renderer
        this->mainWindow = SDL_CreateWindow(
                               "OSW-OS Emulator",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               width,
                               height,
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
                           );
        assert(this->mainWindow && "Never fail window creation");
        this->mainRenderer = SDL_CreateRenderer(this->mainWindow, -1, this->isSoftwareRenderer ? SDL_RENDERER_SOFTWARE : (SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED));
    }
    assert(this->mainRenderer && "Never fail renderer creation");
    fakeDisplayInstance = std::make_unique<FakeDisplay>(DISP_W, DISP_H, this->mainRenderer);

    // Create ImGUI context and initialize
    if(!this->isHeadless) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = this->imguiPath.c_str();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForSDLRenderer(this->mainWindow, this->mainRenderer);
        ImGui_ImplSDLRenderer2_Init(this->mainRenderer);
    }

    // Install CTRL+C handler in headless mode
    if(this->isHeadless) {
        struct sigaction sigIntHandler;

        sigIntHandler.sa_handler = shutdownEmulatorByInterruptSignal;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;

        sigaction(SIGINT, &sigIntHandler, NULL);
    }
}

OswEmulator::~OswEmulator() {
    if(!this->isHeadless) {
        // Shutdown ImGUI
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    // Close window and renderer
    SDL_DestroyRenderer(this->mainRenderer);
    int w = 0, h = 0;
    if(!this->isHeadless) {
        SDL_GetWindowSize(this->mainWindow, &w, &h);
        SDL_DestroyWindow(this->mainWindow);
    } else {
        w = this->mainSurface->w;
        h = this->mainSurface->h;
        SDL_FreeSurface(this->mainSurface);
    }

    // Store (window) config
    this->config = Jzon::object(); // Clear the current cached object
    Jzon::Node window = Jzon::object();
    window.add("width", w);
    window.add("height", h);
    this->config.add("window", window);
    this->config.add("autoWakeUp", this->autoWakeUp);
    std::ofstream configStream(this->configPath, std::ios::trunc);
    Jzon::Writer().writeStream(this->config, configStream);
    configStream.close();
}

void OswEmulator::run() {
    while(this->running) {
        SDL_RenderClear(this->mainRenderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(!this->isHeadless)
                ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT) {
                this->running = false;
                break;
            }
        }

        // Prepare ImGUI for the next frame
        if(!this->isHeadless) {
            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();
            this->renderGUIFrameEmulator();
        }

        // Revive system after deep sleep as needed
        if(this->cpustate != CPUState::active and (this->manualWakeUp or this->autoWakeUp or (this->selfWakeUpAtTimestamp > 0 and this->selfWakeUpAtTimestamp < time(nullptr)))) {
            if(this->manualWakeUp)
                this->bootReason = BootReason::byUser;
            else if(this->autoWakeUp)
                this->bootReason = BootReason::byAuto;
            else if(this->selfWakeUpAtTimestamp > 0 and this->selfWakeUpAtTimestamp < time(nullptr))
                this->bootReason = BootReason::byTimer;
            else
                this->bootReason = BootReason::undefined; // Should never happen...
            this->cpustate = CPUState::active;
            this->manualWakeUp = false;
            this->selfWakeUpAtTimestamp = 0;
            setup();

            /**
             * At the first startup - prepare the key value cache dynamically
             * We must execute this after the OswConfig::setup() call, as otherwise the key are not initialized yet
             */
            if(this->configValuesCache.empty()) {
                this->configValuesCache.resize(oswConfigKeysCount);
                for(size_t keyId = 0; keyId < oswConfigKeysCount; ++keyId) {
                    const OswConfigKey* key = oswConfigKeys[keyId];
                    switch (key->type) {
                    case OswConfigKeyTypedUIType::BOOL:
                        this->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyBool*>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::FLOAT:
                        this->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyFloat*>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::DROPDOWN:
                        this->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyDropDown*>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::SHORT:
                        this->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyShort*>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::INT:
                        this->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyInt*>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::STRING:
                        this->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyString*>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::RGB: {
                        const uint32_t color = dynamic_cast<const OswConfigKeyRGB*>(key)->get();
                        const std::array<float, 3> rgb = {
                            rgb888_red(color) / 255.f,
                            rgb888_green(color) / 255.f,
                            rgb888_blue(color) / 255.f
                        };
                        this->configValuesCache[keyId] = rgb;
                    }
                    break;
                    default:
                        throw std::runtime_error(std::string("Not implemented key type in cache: ") + (char) key->type);
                    }

                    // Now cache the section label to resolve it later to this keys id
                    std::string labelString = key->section;
                    if(this->configSectionsToIdCache.find(labelString) == this->configSectionsToIdCache.end())
                        this->configSectionsToIdCache.insert({labelString, {}});
                    this->configSectionsToIdCache.at(labelString).push_back(keyId);
                }
            }
        }

        // Next OS step
        if(this->cpustate == CPUState::active) {
            // Let the renderer now draw to the FakeDisplays surface
            int res = SDL_SetRenderTarget(this->mainRenderer, fakeDisplayInstance->getTexture());
            assert(res >= 0 && "Failed to set render target to fake display texture");
            try {
                // Run the next OS iteration
                std::chrono::time_point start = std::chrono::system_clock::now();
                loop();
                std::chrono::time_point end = std::chrono::system_clock::now();
                for(size_t keyId = 1; keyId < this->timesLoop.size(); ++keyId)
                    this->timesLoop.at(this->timesLoop.size() - keyId) = this->timesLoop.at(this->timesLoop.size() - keyId - 1);
                this->timesLoop.front() = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                // Track the amount of flushing loops per second
                if(this->lastUiFlush != OswUI::getInstance()->getLastFlush()) {
                    this->lastUiFlush = OswUI::getInstance()->getLastFlush();
                    this->frameCountsOsw.front()++;
                }
                // Process eventual sleep requests
                if(this->requestedSleepState != RequestSleepState::nothing) {
                    if(this->requestedSleepState == RequestSleepState::deep) {
                        OSW_LOG_D("Emulator performs external deep-sleep request...");
                        this->requestedSleepState = RequestSleepState::nothing; // Reset before proceeding, which will throw the EmulatorSleep exception
                        OswHal::getInstance()->deepSleep();
                    } else if(this->requestedSleepState == RequestSleepState::light) {
                        OSW_LOG_D("Emulator performs external light-sleep request...");
                        this->requestedSleepState = RequestSleepState::nothing;
                        OswHal::getInstance()->lightSleep();
                    } else
                        throw std::runtime_error("Unknown sleep state requested"); // huh?
                }
            } catch(EmulatorSleep& e) {
                // Ignore it :P
            }
            // And restore emulator surface render target
            res = SDL_SetRenderTarget(this->mainRenderer, nullptr); // nullptr = back to window surface
            assert(res >= 0 && "Failed to set render target to window surface");
        }

        // Present the fake-display texture as an ImGUI window
        if(!this->isHeadless) {
            ImGui::Begin(LANG_IMGUI_DISPLAY "###display");
            // Using ImGui::BeginChild() to set the size of the inner window properly
            ImGui::BeginChild("##FakeDisplayTexture", ImVec2(fakeDisplayInstance->width, fakeDisplayInstance->height));
            if(fakeDisplayInstance->isEnabled())
                ImGui::Image((void*) fakeDisplayInstance->getTexture(), ImVec2(fakeDisplayInstance->width, fakeDisplayInstance->height));
            else
                ImGui::Text(LANG_IMGUI_DISPLAY_NOPE);
            ImGui::EndChild();
            ImGui::End();
        }

        // If requested try to reset as much as possible
        if(this->wantCleanup)
            this->doCleanup();

        if(!this->isHeadless) {
            // Render the (emulator) gui in memory
            ImGui::Render();

            // Draw ImGUI content
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        }

        // Update the window now with the content of the display
        SDL_RenderPresent(this->mainRenderer);
        if(this->mainWindow)
            SDL_UpdateWindowSurface(this->mainWindow);

        // Update render FPS
        this->frameCountsEmulator.front()++;
        if(this->frameCountsLastUpdate != time(nullptr)) {
            // Update OSW
            for(size_t keyId = 1; keyId < this->frameCountsOsw.size(); ++keyId)
                this->frameCountsOsw.at(this->frameCountsOsw.size() - keyId) = this->frameCountsOsw.at(this->frameCountsOsw.size() - keyId - 1);
            this->frameCountsOsw.front() = 0;
            // Update Emulator
            for(size_t keyId = 1; keyId < this->frameCountsEmulator.size(); ++keyId)
                this->frameCountsEmulator.at(this->frameCountsEmulator.size() - keyId) = this->frameCountsEmulator.at(this->frameCountsEmulator.size() - keyId - 1);
            this->frameCountsEmulator.front() = 0;
            this->frameCountsLastUpdate = time(nullptr);
        }
    }
    this->doCleanup();
}

/**
 * @brief NEVER CALL THIS DURING loop()!!! This method tries to cleanup the OSW-OS as best as it can.
 * May not all references will be reset, but the most important ones will be.
 *
 */
void OswEmulator::doCleanup() {
    OSW_LOG_D("Emulator is performing hard cleanup - this may cause unexpected problems...");
    OswServiceManager::resetInstance();
    OswConfig::resetInstance();
    OswUI::resetInstance();
    OswHal::resetInstance();
    OswLogger::resetInstance();
    OswGlobals::resetGlobals();
    this->cpustate = CPUState::deep;
    this->wantCleanup = false;
}

void OswEmulator::cleanup() {
    this->wantCleanup = true;
}

void OswEmulator::exit() {
    this->running = false;
}

void OswEmulator::reboot() {
    this->cpustate = CPUState::deep; // This is the best we can do, as we can't really reset any global variables...
}

void OswEmulator::scheduleWakeupAfterSleep(unsigned long microseconds) {
    this->selfWakeUpInMicroseconds = microseconds;
}

void OswEmulator::enterSleep(bool toDeepSleep) {
    if(toDeepSleep) {
        this->cleanup(); // schedule cpu reset
        this->cpustate = CPUState::deep;
    } else
        this->cpustate = CPUState::light;
    // Schedule wakeup after sleep
    if(this->selfWakeUpInMicroseconds > 0) {
        this->selfWakeUpAtTimestamp = time(nullptr) + (this->selfWakeUpInMicroseconds / 1000000);
        this->selfWakeUpInMicroseconds = 0;
        OSW_LOG_I("Emulator will wake itself up at ", this->selfWakeUpAtTimestamp);
    } else {
        this->selfWakeUpAtTimestamp = 0;
    }
}

/**
 * @brief Request the emulator to perform this sleep state after the next loop() call.
 *
 * @param state
 */
void OswEmulator::requestSleep(RequestSleepState state) {
    this->requestedSleepState = state;
}

void OswEmulator::setButton(Button id, bool state) {
    this->buttonCheckboxes.at(id) = state;
    this->buttons.at(id) = state;
};

bool OswEmulator::getButton(Button id) {
    return this->buttons.at(id);
};

uint8_t OswEmulator::getBatteryRaw() {
    return this->batRaw;
}

bool OswEmulator::isCharging() {
    return this->charging;
}

bool OswEmulator::fromDeepSleep() {
    return this->cpustate == CPUState::deep;
}

OswEmulator::BootReason OswEmulator::getBootReason() {
    return this->bootReason;
}

void OswEmulator::addGUIHelp(const char* msg) {
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(msg);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void OswEmulator::renderGUIFrameEmulator() {
    // Emulator control
    ImGui::Begin(LANG_IMGUI_EMULATOR "###emulator");
    ImGui::Text("CPU: %s", this->cpustate == CPUState::active ? LANG_EMULATOR_CPU_ACTIVE : (this->cpustate == CPUState::light ? LANG_EMULATOR_CPU_LIGHT_SLEEP : LANG_EMULATOR_CPU_DEEP_SLEEP));
    ImGui::PlotLines("FPS Emulator", (float*) this->frameCountsEmulator.data() + 1, this->frameCountsEmulator.size() - 1);
    ImGui::PlotLines("FPS OSW-UI", (float*) this->frameCountsOsw.data() + 1, this->frameCountsOsw.size() - 1);
    ImGui::PlotLines("loop()", (float*) this->timesLoop.data(), this->timesLoop.size());
    ImGui::Separator();
    ImGui::Checkbox(LANG_EMULATOR_WAKELOCK, &this->autoWakeUp);
    this->addGUIHelp(LANG_EMULATOR_WAKELOCK_HELP);
    if(this->cpustate == CPUState::active) { // If false, the ui instance could be unavailable
        ImGui::Checkbox(LANG_EMULATOR_FPSLIMIT, &OswUI::getInstance()->mEnableTargetFPS);
        this->addGUIHelp(LANG_EMULATOR_FPSLIMIT_HELP);
    }
    if(!this->autoWakeUp and this->cpustate != CPUState::active and ImGui::Button("Wake Up"))
        this->manualWakeUp = true;
    ImGui::End();

    // Button Control
    ImGui::Begin(LANG_IMGUI_BUTTONS "###buttons");
    if(ImGui::Button(LANG_EMULATOR_BTN " PWR")) {
        this->enterSleep(true);
        this->manualWakeUp = true;
    }
    this->addGUIHelp(LANG_EMULATOR_BTN_PWR_HELP);
    for(size_t buttonId = 0; buttonId < this->buttons.size(); ++buttonId) {
        // Due to this checkbox-alignment they are always one frame behind the button state (but this is not a problem)
        ImGui::Checkbox(("##btn" + std::to_string(buttonId + 1)).c_str(), &this->buttonCheckboxes.at(buttonId)); // "##" as prefix hides the label, but still allows for unique ids
        ImGui::SameLine();
        ImGui::Button((std::string(LANG_EMULATOR_BTN " ") + ButtonNames[buttonId]).c_str());
        if(ImGui::IsItemActivated() or ImGui::IsItemDeactivated()) // Only use the button to control the button state, if it changed during the last frame
            this->buttonCheckboxes.at(buttonId) = ImGui::IsItemActive();
        if(ImGui::IsItemDeactivated() and this->buttonResetAfterMultiPress) {
            for(size_t bId = 0; bId < this->buttonCheckboxes.size(); ++bId)
                if(this->buttonCheckboxes.at(bId))
                    this->setButton((Button) bId, false);
        }
        this->setButton((Button) buttonId, this->buttonCheckboxes.at(buttonId));
    }
    ImGui::Checkbox(LANG_EMULATOR_MBTN, &this->buttonResetAfterMultiPress);
    this->addGUIHelp(LANG_EMULATOR_MBTN_HELP);
    ImGui::End();

    // Virtual Sensors
    ImGui::Begin(LANG_IMGUI_VIRTUAL_SENSORS "###virtual_sensors");
    if(OswHal::getInstance()->devices() and OswHal::getInstance()->devices()->virtualDevice) {
        ImGui::InputFloat("Temperature", &OswHal::getInstance()->devices()->virtualDevice->values.temperature, 1, 10);
        ImGui::InputFloat("Pressure", &OswHal::getInstance()->devices()->virtualDevice->values.pressure, 1, 10);
        ImGui::InputFloat("Humidity", &OswHal::getInstance()->devices()->virtualDevice->values.humidity, 1, 10);
        ImGui::InputFloat("Acceleration X", &OswHal::getInstance()->devices()->virtualDevice->values.accelerationX, 0.1f, 10);
        ImGui::InputFloat("Acceleration Y", &OswHal::getInstance()->devices()->virtualDevice->values.accelerationY, 0.1f, 10);
        ImGui::InputFloat("Acceleration Z", &OswHal::getInstance()->devices()->virtualDevice->values.accelerationZ, 0.1f, 10);
        ImGui::InputInt("Magnetometer X", &OswHal::getInstance()->devices()->virtualDevice->values.magnetometerX, 0.1f, 10);
        ImGui::InputInt("Magnetometer Y", &OswHal::getInstance()->devices()->virtualDevice->values.magnetometerY, 0.1f, 10);
        ImGui::InputInt("Magnetometer Z", &OswHal::getInstance()->devices()->virtualDevice->values.magnetometerZ, 0.1f, 10);
        ImGui::InputInt("Magnetometer Azimuth", &OswHal::getInstance()->devices()->virtualDevice->values.magnetometerAzimuth, 1, 10);
        ImGui::InputInt("Steps", (int*) &OswHal::getInstance()->devices()->virtualDevice->values.steps, 1, 10); // Warning - negative values will cause an underflow... ImGui has no convenient way of limiting the input range...
        // get string to display selected value
        const char* str = "unknown";
        OswAccelerationProvider::ActivityMode& activityMode = OswHal::getInstance()->devices()->virtualDevice->values.activityMode; // take reference, for easier access
        if(activityMode == OswAccelerationProvider::ActivityMode::STILL)
            str = "still";
        else if(activityMode == OswAccelerationProvider::ActivityMode::WALK)
            str = "walk";
        else if(activityMode == OswAccelerationProvider::ActivityMode::RUN)
            str = "run";
        // display activity mode
        if (ImGui::BeginCombo("Activity", str)) {
            if(ImGui::Selectable("still", activityMode == OswAccelerationProvider::ActivityMode::STILL))
                OswHal::getInstance()->devices()->virtualDevice->values.activityMode = OswAccelerationProvider::ActivityMode::STILL;
            else if(ImGui::Selectable("walk", activityMode == OswAccelerationProvider::ActivityMode::WALK))
                OswHal::getInstance()->devices()->virtualDevice->values.activityMode = OswAccelerationProvider::ActivityMode::WALK;
            else if(ImGui::Selectable("run", activityMode == OswAccelerationProvider::ActivityMode::RUN))
                OswHal::getInstance()->devices()->virtualDevice->values.activityMode = OswAccelerationProvider::ActivityMode::RUN;
            else if(ImGui::Selectable("unknown", activityMode == OswAccelerationProvider::ActivityMode::UNKNOWN))
                OswHal::getInstance()->devices()->virtualDevice->values.activityMode = OswAccelerationProvider::ActivityMode::UNKNOWN;
            ImGui::EndCombo();
        }
    } else
        ImGui::Text(LANG_IMGUI_VIRTUAL_SENSORS_NOPE);
    ImGui::End();

    ImGui::Begin(LANG_IMGUI_CONFIGURATION "###configuration");
    if(this->configValuesCache.size()) {
        for(auto& [label, keyIds] : this->configSectionsToIdCache) {
            if(ImGui::CollapsingHeader(label.c_str()))
                for(auto& keyId : keyIds) {
                    const OswConfigKey* key = oswConfigKeys[keyId];
                    switch (key->type) {
                    case OswConfigKeyTypedUIType::BOOL:
                        ImGui::Checkbox(key->label, &std::get<bool>(this->configValuesCache[keyId]));
                        break;
                    case OswConfigKeyTypedUIType::FLOAT:
                        ImGui::InputFloat(key->label, &std::get<float>(this->configValuesCache[keyId]));
                        break;
                    case OswConfigKeyTypedUIType::DROPDOWN: {
                        // The dropdowns communicate the possible options using the help field. That's hacky...
                        // Parse the help text as options list (separated by ',')
                        std::string optionsStr = key->help;
                        std::vector<std::string> options = {""};
                        for(const char& c: optionsStr)
                            if(c == ',')
                                options.push_back("");
                            else
                                options.back() += c;

                        // Determine the index of the current option
                        size_t currentOption = 0;
                        for(size_t optId = 0; optId < options.size(); ++optId)
                            if(options[optId] == std::get<std::string>(this->configValuesCache[keyId]))
                                currentOption = optId;

                        // Create the combo-box
                        if (ImGui::BeginCombo(key->label, std::get<std::string>(this->configValuesCache[keyId]).c_str())) {
                            for (size_t i = 0; i < options.size(); i++) {
                                if (ImGui::Selectable(options[i].c_str(), currentOption == i))
                                    this->configValuesCache[keyId] = options[i];
                            }
                            ImGui::EndCombo();
                        }
                    }
                    break;
                    case OswConfigKeyTypedUIType::SHORT:
                        ImGui::InputScalar(key->label, ImGuiDataType_S16, &std::get<short>(this->configValuesCache[keyId]));
                        break;
                    case OswConfigKeyTypedUIType::INT:
                        ImGui::InputInt(key->label, &std::get<int>(this->configValuesCache[keyId]));
                        break;
                    case OswConfigKeyTypedUIType::RGB:
                        ImGui::ColorEdit3(key->label, std::get<std::array<float, 3>>(this->configValuesCache[keyId]).data());
                        break;
                    case OswConfigKeyTypedUIType::STRING:
                        ImGui::InputText(key->label, &std::get<std::string>(this->configValuesCache[keyId]));
                        break;
                    default:
                        throw std::runtime_error(std::string("Not implemented key type in view: ") + (char) key->type);
                    }

                    if(key->help and key->type != OswConfigKeyTypedUIType::DROPDOWN)
                        // As said before, the dropdown has no "help"!
                        this->addGUIHelp(key->help);
                }
        }

        ImGui::Button(LANG_SAVE);
        if(ImGui::IsItemActive()) {
            OswConfig::getInstance()->enableWrite();
            for(size_t keyId = 0; keyId < oswConfigKeysCount; ++keyId) {
                OswConfigKey* key = oswConfigKeys[keyId];
                switch(key->type) {
                case OswConfigKeyTypedUIType::BOOL:
                    dynamic_cast<OswConfigKeyBool*>(key)->set(std::get<bool>(this->configValuesCache[keyId]));
                    break;
                case OswConfigKeyTypedUIType::FLOAT:
                    dynamic_cast<OswConfigKeyFloat*>(key)->set(std::get<float>(this->configValuesCache[keyId]));
                    break;
                case OswConfigKeyTypedUIType::DROPDOWN:
                    dynamic_cast<OswConfigKeyDropDown*>(key)->set(std::get<std::string>(this->configValuesCache[keyId]));
                    break;
                case OswConfigKeyTypedUIType::SHORT:
                    dynamic_cast<OswConfigKeyShort*>(key)->set(std::get<short>(this->configValuesCache[keyId]));
                    break;
                case OswConfigKeyTypedUIType::INT:
                    dynamic_cast<OswConfigKeyInt*>(key)->set(std::get<int>(this->configValuesCache[keyId]));
                    break;
                case OswConfigKeyTypedUIType::RGB: {
                    std::array<float, 3> rgb = std::get<std::array<float, 3>>(this->configValuesCache[keyId]);
                    dynamic_cast<OswConfigKeyRGB*>(key)->set(rgb888(rgb[0] * 255.0f, rgb[1] * 255.0f, rgb[2] * 255.0f));
                }
                break;
                case OswConfigKeyTypedUIType::STRING:
                    dynamic_cast<OswConfigKeyString*>(key)->set(std::get<std::string>(this->configValuesCache[keyId]));
                    break;
                default:
                    throw std::runtime_error(std::string("Not implemented key type in save: ") + (char) key->type);
                }
            }
            OswConfig::getInstance()->disableWrite();
            if(this->cpustate == CPUState::active) // Only notify the change, if the device is active (otherwise we may run into problems)
                OswConfig::getInstance()->notifyChange();
        }
    } else
        ImGui::Text(LANG_IMGUI_CONFIGURATION_NOPE);
    ImGui::End();
}

OswEmulator::CPUState OswEmulator::getCpuState() {
    return this->cpustate;
}
