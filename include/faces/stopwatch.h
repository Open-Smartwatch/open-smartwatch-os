#ifndef OSW_APP_STOPWATCH_H
#define OSW_APP_STOPWATCH_H

#include <osw_face.h>

class OswStopwatchFace : public OswFace {
    public:
        OswStopwatchFace(void){};
        void setup(OswLauncher* launcher);
        void loop();
        void stop();
        void action(); 
        void action2(); 
        ~OswStopwatchFace(){};
    private:
        OswLauncher* _launcher;
};

#endif