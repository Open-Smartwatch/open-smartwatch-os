#include <chrono>
#include <filesystem>

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
    // Load emulator config
    this->config = Jzon::object();
    if(std::filesystem::exists(this->configPath)) {
        std::ifstream configStream(this->configPath, std::ios::in);
        this->config = Jzon::Parser().parseStream(configStream);
        configStream.close();
    }

    // Reset the config to default values (so we don't have to specify them on every time)
    this->autoWakeUp = this->config.get("autoWakeUp").toBool(true);

    // Init the SDL window and renderer
    this->mainWindow = SDL_CreateWindow(
                           "OSW-OS Emulator",
                           SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED,
                           this->config.get("window").get("width").toInt(DISP_W + this->guiPadding + this->guiWidth + this->guiPadding),
                           this->config.get("window").get("height").toInt(DISP_H),
                           SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
                       );
    assert(this->mainWindow && "Never fail window creation");
    this->mainRenderer = SDL_CreateRenderer(this->mainWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    assert(this->mainRenderer && "Never fail renderer creation");
    fakeDisplayInstance = std::make_unique<FakeDisplay>(DISP_W, DISP_H, this->mainWindow, this->mainRenderer);

    // Create ImGUI context and initialize
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(this->mainWindow, this->mainRenderer);
    ImGui_ImplSDLRenderer_Init(this->mainRenderer);
}

OswEmulator::~OswEmulator() {
    // Shutdown ImGUI
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // Close window and renderer
    SDL_DestroyRenderer(this->mainRenderer);
    int w = 0, h = 0;
    SDL_GetWindowSize(this->mainWindow, &w, &h);
    SDL_DestroyWindow(this->mainWindow);

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
        std::chrono::time_point loopStart = std::chrono::system_clock::now();
        SDL_RenderClear(this->mainRenderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT) {
                this->running = false;
                break;
            }
        }
    
        // Prepare ImGUI for the next frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        this->renderGUIFrameEmulator();

        // Revive system after deep sleep as needed
        if(this->cpustate == CPUState::deepSleep and (this->wakeUpNow or this->autoWakeUp)) {
            setup();
            this->cpustate = CPUState::active;
            this->wakeUpNow = false;
            OswUI::getInstance()->mEnableTargetFPS = false; // Disable FPS limiter of the UI iteself

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
        try {
            if(this->cpustate == CPUState::active) {
                std::chrono::time_point start = std::chrono::system_clock::now();
                loop();
                std::chrono::time_point end = std::chrono::system_clock::now();
                for(size_t keyId = 1; keyId < this->timesLoop.size(); ++keyId)
                    this->timesLoop.at(this->timesLoop.size() - keyId) = this->timesLoop.at(this->timesLoop.size() - keyId - 1);
                this->timesLoop.front() = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            }
        } catch(EmulatorSleep& e) {
            // Ignore it :P
        }

        // Render the (emulator) gui in memory
        ImGui::Render();

        // Draw ImGUI content
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

        // Update the window now with the content of the display
        SDL_RenderPresent(this->mainRenderer);
        SDL_UpdateWindowSurface(this->mainWindow);

        // Update render FPS
        std::chrono::time_point loopEnd = std::chrono::system_clock::now();
        for(size_t keyId = 1; keyId < this->timesFrames.size(); ++keyId)
            this->timesFrames.at(this->timesFrames.size() - keyId) = this->timesFrames.at(this->timesFrames.size() - keyId - 1);
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
    this->buttonCheckboxes.at(id) = state;
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
    for(size_t buttonId = 0; buttonId < this->buttons.size(); ++buttonId) {
        ImGui::Button(("Button " + std::to_string(buttonId + 1)).c_str());
        if(ImGui::IsItemActivated() or ImGui::IsItemDeactivated()) // Only use the button to control the button state, if it changed during the last frame
            this->buttonCheckboxes.at(buttonId) = ImGui::IsItemActive();
        if(ImGui::IsItemDeactivated() and this->buttonResetAfterMultiPress) {
            for(size_t bId = 0; bId < this->buttonCheckboxes.size(); ++bId)
                if(this->buttonCheckboxes.at(bId))
                    this->setButton(bId, false);
        }
        ImGui::SameLine();
        ImGui::Checkbox(("##btn" + std::to_string(buttonId + 1)).c_str(), &this->buttonCheckboxes.at(buttonId)); // "##" as prefix hides the label, but still allows for unique ids
        this->setButton(buttonId, this->buttonCheckboxes.at(buttonId));
    }
    ImGui::Checkbox("Release after multi-press", &this->buttonResetAfterMultiPress);
    this->addGUIHelp("Whenever you press-and-hold any butten(s) by activating their checkbox(es) and then click-and-release any button normally, all other held buttons will also be released.");
    ImGui::End();

    // Virtual Sensors
    ImGui::Begin("Virtual Sensors");
    if(OswHal::getInstance()->devices and OswHal::getInstance()->devices->virtualDevice) {
        ImGui::InputFloat("Temperature", &OswHal::getInstance()->devices->virtualDevice->values.temperature, 1, 10);
        ImGui::InputFloat("Pressure", &OswHal::getInstance()->devices->virtualDevice->values.pressure, 1, 10);
        ImGui::InputFloat("Humidity", &OswHal::getInstance()->devices->virtualDevice->values.humidity, 1, 10);
        ImGui::InputFloat("Acceleration X", &OswHal::getInstance()->devices->virtualDevice->values.accelerationX, 0.1f, 10);
        ImGui::InputFloat("Acceleration Y", &OswHal::getInstance()->devices->virtualDevice->values.accelerationY, 0.1f, 10);
        ImGui::InputFloat("Acceleration Z", &OswHal::getInstance()->devices->virtualDevice->values.accelerationZ, 0.1f, 10);
        ImGui::InputInt("Magnetometer Azimuth", &OswHal::getInstance()->devices->virtualDevice->values.magnetometerAzimuth, 1, 10);
        ImGui::InputInt("Steps", (int*) &OswHal::getInstance()->devices->virtualDevice->values.steps, 1, 10); // Warning - negative values will cause an underflow... ImGui has no conventient way of limiting the input range...
    } else
        ImGui::Text("The virtual sensors are only available, while the virtual device is active.");
    ImGui::End();

    ImGui::Begin("Configuration");
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
                                bool isSelected = currentOption == i;
                                if (ImGui::Selectable(options[i].c_str(), &isSelected))
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
                    default:
                        throw std::runtime_error(std::string("Not implemented key type in view: ") + (char) key->type);
                    }

                    if(key->help and key->type != OswConfigKeyTypedUIType::DROPDOWN)
                        // As said before, the dropdown has no "help"!
                        this->addGUIHelp(key->help);
                }
        }

        ImGui::Button("Save");
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
                default:
                    throw std::runtime_error(std::string("Not implemented key type in save: ") + (char) key->type);
                }
            }
            OswConfig::getInstance()->disableWrite();
            OswConfig::getInstance()->notifyChange();
        }
    } else
        ImGui::Text("The configuration is not initialized yet.");
    ImGui::End();
}