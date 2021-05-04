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
        void backToFaceMode();
        void resetButtons();
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


        enum RenderMode{
            Face,
            App,
            Menu
        };

    private:
        int16_t faceIndex = 0;
        int16_t faceCount = 0;
        std::vector<FaceRegistration*> faces;
        
        
        int16_t appIndex = 0;
        int16_t appCount = 0;
        std::vector<AppRegistration*> apps;

        bool faceMode = true;


        RenderMode renderMode;

        void handleFace();
        void handleApp();

        void handleMenu();
        void printMenuEntry(int16_t index);
        int16_t menuIndex = 0;


        
        bool btnLastState[3];
        bool btnIsDown[3];
        bool btnGoneUp[3];  
        bool btnSuppressUntilUpAgain[3];  
        bool btnGoneDown[3];   
        long btnIsDownSince[3];  
        bool btnLongPress[3];  
};

#endif