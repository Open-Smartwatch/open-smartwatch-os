#include "../include/Display.h"

#include "../include/Emulator.hpp"

OswEmulator* OswEmulator::instance = nullptr;

OswEmulator::OswEmulator() {
    this->mainWindow = SDL_CreateWindow("OSW-OS Emulator Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256, 256, 0);
    this->mainRenderer = SDL_CreateRenderer(this->mainWindow, -1, SDL_RENDERER_ACCELERATED);
    fakeDisplayInstance = std::make_unique<FakeDisplay>(256, 256, this->mainWindow, this->mainRenderer);
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