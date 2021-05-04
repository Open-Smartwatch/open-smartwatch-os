#ifndef OSW_PLAUNCHER_H
#define OSW_PLAUNCHER_H

#include <osw_launcher.h>
#include <osw_app.h>
#include <osw_face.h>
#include <vector>


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
        int16_t faceIndex = 0;

        int16_t faceCount = 0;
        std::vector<FaceRegistration*> faces;
        
        int16_t appCount = 0;
        std::vector<AppRegistration*> apps;

};

#endif