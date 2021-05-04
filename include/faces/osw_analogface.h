#ifndef OSW_ANALOGFACE_H
#define OSW_ANALOGFACE_H

#include "osw_basicface.h"

class OswAnalogFace : public OswBasicFace{

    public:
        void action(); 
        void action2(); 
        void setup(OswLauncher* launcher);
        void loop(); 
        void stop();
    protected:
        void draw(Graphics2D* gfx2d) override;
};

#endif