#ifndef OSW_BASICFACE_H
#define OSW_BASICFACE_H

#include <osw_face.h>

class OswBasicFace : public OswFace{

    public:
        OswBasicFace(void){};
        void setup(OswLauncher* launcher);
        void stop();
        void loop(); 
        void action(); 
        void action2(); 
        ~OswBasicFace(){};

    protected:
        OswLauncher* _launcher;
        uint32_t second = 0;
        uint32_t minute = 0;
        uint32_t hour = 0;
        void getTime();
        virtual void draw(Graphics2D* gfx2d);
};

#endif