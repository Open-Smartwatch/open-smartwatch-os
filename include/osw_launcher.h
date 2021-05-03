#ifndef OSW_LAUNCHER_H
#define OSW_LAUNCHER_H

#include <osw_hal.h>
class OswFace; 
class OswApp; 

class OswLauncher {
  public:
    OswLauncher(void){};
    virtual void setup(OswHal* hal) = 0;
    virtual void loop() = 0;
    virtual void stop() = 0;
    virtual OswHal* getHal() = 0;
    virtual Graphics2D* getGfx2g() = 0;
    virtual void requestFlush() = 0;
    virtual void registerFace(OswFace* face) = 0;
    virtual void registerApp(OswApp* app) = 0;
    ~OswLauncher(void){};
  protected:
    OswHal* hal;
    OswApp *loadedApps[];
    uint8_t appCount;
    uint8_t currentAppIndex;
    
    void load(OswApp* app);
};

#endif