#ifndef OSW_DEFAULTLAUNCHER_H
#define OSW_DEFAULTLAUNCHER_H

#include <osw_hal.h>
#include <osw_launcher.h>


class OswDefaultLauncher : public OswLauncher {
    public:
        OswDefaultLauncher(void){};
        virtual void setup(OswHal* hal);
        void loop();
        void stop();
        ~OswDefaultLauncher(){};
    private:
        OswHal* _hal;
};

#endif