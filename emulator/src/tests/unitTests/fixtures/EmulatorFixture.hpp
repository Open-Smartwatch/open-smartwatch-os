#pragma once

#include <memory>
#include <thread>
#include <optional>

#include "CaptureSerialFixture.hpp"
#include "../../../include/Emulator.hpp"

class EmulatorFixture {
  public:
    enum emulatorRunResults: int { // using int, so utest can print it
        STARTING,
        RUNNING,
        STOPPED,
        FAILED
    };

    constexpr static int timeout = 10; // Seconds

    emulatorRunResults state = emulatorRunResults::STARTING;
    std::unique_ptr<OswEmulator> oswEmu;

    EmulatorFixture(bool headless) {
        // Create and run the (headless) emulator
        this->configPath = "config_" + std::to_string(rand()) + ".json";
        this->imguiPath = "imgui_" + std::to_string(rand()) + ".ini";
        oswEmu = std::make_unique<OswEmulator>(headless, headless, this->configPath, this->imguiPath);
        OswEmulator::instance = oswEmu.get();
        std::thread t([&]() {
            try {
                state = emulatorRunResults::RUNNING;
                oswEmu->run();
                state = emulatorRunResults::STOPPED;
            } catch(...) {
                state = emulatorRunResults::FAILED;
            }
        });
        t.detach(); // If we time out or fail, we don't want to wait for the thread to finish
        // Wait for the thread to start
        std::chrono::time_point startWaitStarted = std::chrono::steady_clock::now();
        while(state == emulatorRunResults::STARTING and std::chrono::steady_clock::now() - startWaitStarted < std::chrono::seconds(this->timeout)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if(state != emulatorRunResults::RUNNING) {
            throw std::runtime_error("Emulator failed to start");
        }
    }

    ~EmulatorFixture() {
        // Stop the emulator
        oswEmu->exit();
        // Wait for the thread to finish
        std::chrono::time_point stopWaitStarted = std::chrono::steady_clock::now();
        while(state == emulatorRunResults::RUNNING and std::chrono::steady_clock::now() - stopWaitStarted < std::chrono::seconds(this->timeout)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if(state == emulatorRunResults::RUNNING) {
            std::cerr << "Emulator stopped with state " << state << " - it is likely still running!" << std::endl;
            std::abort();
        }
        OswEmulator::instance = nullptr;
        oswEmu.reset(); // Delete the emulator, which may write out the config file
        bool res = std::filesystem::remove(this->configPath);
        if(!res)
            std::cerr << "Failed to remove config file -> did the emulator did not write it (and crashed)?" << std::endl;
        std::filesystem::remove(this->imguiPath);
    }
  private:
    std::string configPath;
    std::string imguiPath;
};