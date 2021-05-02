#ifndef OSW_PLAUNCHER_H
#define OSW_PLAUNCHER_H

#include <osw_hal.h>
#include <osw_launcher.h>


class OswPLauncher : public OswLauncher {
    public:
        OswPLauncher(void){};
        void setup(OswHal* hal);
        void loop(OswHal* hal);
        void stop(OswHal* hal);
        ~OswPLauncher(){};

        struct PLauncherAppRegistration
        {
            OswApp *app;
            bool isPinned;
            char name[20];
        };

    private:
        //RTC_DATA_ATTR uint8_t appPtr = 0;
        uint8_t appCount = 0;
        uint8_t appIndex = 0;
        uint8_t pinnedCount = 0;
        uint8_t *pinned;
        
        struct PLauncherAppRegistration *registrations[];

};

#endif