#ifndef OSW_PLAUNCHER_H
#define OSW_PLAUNCHER_H

#include <osw_launcher.h>
#include <osw_app.h>
#include <osw_face.h>


class OswPLauncher : public OswLauncher {
    public:
        OswPLauncher(void){};
        void setup(OswHal* hal);
        void loop();
        void stop();
        void requestFlush();
        OswHal* getHal();
        Graphics2D* getGfx2g();
        void registerApp(OswApp* app);
        void registerFace(OswFace* face);
        ~OswPLauncher(){};

        struct FaceRegistration
        {
            OswFace *face;
            bool isDefault;
            char name[20];
        };
        
        struct AppRegistration
        {
            OswApp *app;
            char name[20];
        };


    private:
        uint8_t faceIndex = 0;

        uint8_t faceCount = 0;
        struct FaceRegistration *faces[];
        
        uint8_t appCount = 0;
        struct AppRegistration *apps[];

};

#endif