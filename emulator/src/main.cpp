#include <iostream>
#include <thread>

#include <wx/evtloop.h> // For GetMainLoop()
#include <SDL2/SDL.h>

#include "../include/Emulator.hpp"
#include "../include/gui/WxControlApp.hpp"

int main(int argc, char** argv) {
    // Start the emulator (display)
    std::unique_ptr<OswEmulator> oswEmu;
    std::unique_ptr<WxControlApp> wxControlApp = std::make_unique<WxControlApp>();
    std::thread emuThread([&oswEmu, &wxControlApp]() {
        if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
            printf("error initializing SDL: %s\n", SDL_GetError());
        oswEmu = std::make_unique<OswEmulator>();
        OswEmulator::instance = oswEmu.get();
        oswEmu->run();
        SDL_Quit();
        // Shutdown the other thread
        if(wxControlApp)
            wxControlApp->GetMainLoop()->ScheduleExit();
    });

    // Open control window
    std::thread controlThread([&argc, &argv, &oswEmu, &wxControlApp]() {
        wxApp::SetInstance(wxControlApp.get()); // WxWidgets now owns this instance, so no delete() needed!
        wxEntryStart(argc, argv);
        wxTheApp->CallOnInit();
        wxTheApp->OnRun();
        wxControlApp.release(); // OnExit will delete it anyways
        wxTheApp->OnExit();
        wxEntryCleanup();
        // Shutdown the other thread
        oswEmu->exit();
    });

    // Shutdown
    if(controlThread.joinable())
        controlThread.join();
    if(emuThread.joinable())
        emuThread.join();

    return EXIT_SUCCESS;
}