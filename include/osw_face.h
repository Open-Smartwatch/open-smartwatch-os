#ifndef OSW_FACE_H
#define OSW_FACE_H

#include <osw_launcher.h>

class OswFace {
    public:
        virtual void setup(OswLauncher* launcher);
        virtual void stop() = 0;
        virtual void loop() = 0; 
        virtual void action() = 0; 
        virtual void action2() = 0; 
        const char name[20] = "OswFace";
    private:
    protected:
};

#endif