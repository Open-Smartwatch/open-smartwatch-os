#pragma once

#include "../../include/Emulator.hpp"
#include "osw_ui.h"
#include "osw_config_keys.h"

// OswEmulator::instance is a friend class of OswEmulator. It is needed for UI tests of OswEmulator
class TestEmulator
{
public:
    static void newFrame()
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    static void wakeFromDeepSleep()
    {
        // Revive system after deep sleep as needed
        if (OswEmulator::instance->cpustate != OswEmulator::CPUState::active and (OswEmulator::instance->manualWakeUp or OswEmulator::instance->autoWakeUp or (OswEmulator::instance->selfWakeUpAtTimestamp > 0 and OswEmulator::instance->selfWakeUpAtTimestamp < time(nullptr))))
        {
            if (OswEmulator::instance->manualWakeUp)
                OswEmulator::instance->bootReason = OswEmulator::BootReason::byUser;
            else if (OswEmulator::instance->autoWakeUp)
                OswEmulator::instance->bootReason = OswEmulator::BootReason::byAuto;
            else if (OswEmulator::instance->selfWakeUpAtTimestamp > 0 and OswEmulator::instance->selfWakeUpAtTimestamp < time(nullptr))
                OswEmulator::instance->bootReason = OswEmulator::BootReason::byTimer;
            else
                OswEmulator::instance->bootReason = OswEmulator::BootReason::undefined; // Should never happen...
            OswEmulator::instance->cpustate = OswEmulator::CPUState::active;
            OswEmulator::instance->manualWakeUp = false;
            OswEmulator::instance->selfWakeUpAtTimestamp = 0;
            setup();

            /**
             * At the first startup - prepare the key value cache dynamically
             * We must execute OswEmulator::instance after the OswConfig::setup() call, as otherwise the key are not initialized yet
             */
            if (OswEmulator::instance->configValuesCache.empty())
            {
                OswEmulator::instance->configValuesCache.resize(oswConfigKeysCount);
                for (size_t keyId = 0; keyId < oswConfigKeysCount; ++keyId)
                {
                    const OswConfigKey *key = oswConfigKeys[keyId];
                    switch (key->type)
                    {
                    case OswConfigKeyTypedUIType::BOOL:
                        OswEmulator::instance->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyBool *>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::FLOAT:
                        OswEmulator::instance->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyFloat *>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::DROPDOWN:
                        OswEmulator::instance->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyDropDown *>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::SHORT:
                        OswEmulator::instance->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyShort *>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::INT:
                        OswEmulator::instance->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyInt *>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::STRING:
                        OswEmulator::instance->configValuesCache[keyId] = dynamic_cast<const OswConfigKeyString *>(key)->get();
                        break;
                    case OswConfigKeyTypedUIType::RGB:
                    {
                        const uint32_t color = dynamic_cast<const OswConfigKeyRGB *>(key)->get();
                        const std::array<float, 3> rgb = {
                            rgb888_red(color) / 255.f,
                            rgb888_green(color) / 255.f,
                            rgb888_blue(color) / 255.f};
                        OswEmulator::instance->configValuesCache[keyId] = rgb;
                    }
                    break;
                    default:
                        throw std::runtime_error(std::string("Not implemented key type in cache: ") + (char)key->type);
                    }

                    // Now cache the section label to resolve it later to OswEmulator::instance keys id
                    std::string labelString = key->section;
                    if (OswEmulator::instance->configSectionsToIdCache.find(labelString) == OswEmulator::instance->configSectionsToIdCache.end())
                        OswEmulator::instance->configSectionsToIdCache.insert({labelString, {}});
                    OswEmulator::instance->configSectionsToIdCache.at(labelString).push_back(keyId);
                }
            }
        }
    }

    static void drawEmulator()
    {
        // Let the renderer now draw to the FakeDisplays surface
        int res = SDL_SetRenderTarget(OswEmulator::instance->mainRenderer, fakeDisplayInstance->getTexture());
        assert(res >= 0 && "Failed to set render target to fake display texture");
        try
        {
            // Run the next OS iteration
            std::chrono::time_point start = std::chrono::system_clock::now();
            loop();
            std::chrono::time_point end = std::chrono::system_clock::now();
            for (size_t keyId = 1; keyId < OswEmulator::instance->timesLoop.size(); ++keyId)
                OswEmulator::instance->timesLoop.at(OswEmulator::instance->timesLoop.size() - keyId) = OswEmulator::instance->timesLoop.at(OswEmulator::instance->timesLoop.size() - keyId - 1);
            OswEmulator::instance->timesLoop.front() = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            // Track the amount of flushing loops per second
            if (OswEmulator::instance->lastUiFlush != OswUI::getInstance()->getLastFlush())
            {
                OswEmulator::instance->lastUiFlush = OswUI::getInstance()->getLastFlush();
                OswEmulator::instance->frameCountsOsw.front()++;
            }
        }
        catch (OswEmulator::EmulatorSleep &e)
        {
            // Ignore it :P
        }
        // And restore emulator surface render target
        res = SDL_SetRenderTarget(OswEmulator::instance->mainRenderer, nullptr); // nullptr = back to window surface
        assert(res >= 0 && "Failed to set render target to window surface");

        if (!OswEmulator::instance->isHeadless)
        {
            ImGui::Begin("Display");
            // Using ImGui::BeginChild() to set the size of the inner window properly
            ImGui::BeginChild("##FakeDisplayTexture", ImVec2(fakeDisplayInstance->width, fakeDisplayInstance->height));
            if (fakeDisplayInstance->isEnabled())
                ImGui::Image((void *)fakeDisplayInstance->getTexture(), ImVec2(fakeDisplayInstance->width, fakeDisplayInstance->height));
            else
                ImGui::Text("Display is not active.");
            ImGui::EndChild();
            ImGui::End();
        }

        // If requested try to reset as much as possible
        if (OswEmulator::instance->wantCleanup)
            OswEmulator::instance->doCleanup();
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