#include "../include/Display.h"

#include "../include/Emulator.hpp"

OswEmulator::OswEmulator() {
    this->mainWindow = SDL_CreateWindow("OSW-OS Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256, 256, 0);
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