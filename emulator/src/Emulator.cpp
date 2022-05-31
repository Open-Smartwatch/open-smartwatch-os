#include "../include/Display.h"

#include "../include/Emulator.hpp"

OswEmulator* OswEmulator::instance = nullptr;

OswEmulator::OswEmulator() {
    this->mainWindow = SDL_CreateWindow("OSW-OS Emulator Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256, 256, 0);
    this->mainRenderer = SDL_CreateRenderer(this->mainWindow, -1, SDL_RENDERER_ACCELERATED);
    fakeDisplayInstance = std::make_unique<FakeDisplay>(256, 256, this->mainWindow, this->mainRenderer);
}

OswEmulator::~OswEmulator() {
    SDL_DestroyWindow(this->mainWindow);
}

void OswEmulator::run() {
    setup();

    while(this->running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                this->running = false;
        }

        loop();

        // Update the window now with the content of the display
        SDL_RenderPresent(this->mainRenderer);
    }
}

void OswEmulator::exit() {
    this->running = false;
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