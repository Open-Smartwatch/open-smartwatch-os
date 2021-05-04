#ifndef OSW_LAUNCHER_H
#define OSW_LAUNCHER_H

#include <osw_hal.h>
class OswFace; 
class OswApp; 

class OswLauncher {
  public:
    OswLauncher(void){};
    virtual void setup(OswHal* hal);
    virtual void loop();
    virtual void stop() = 0;
    virtual OswHal* getHal();
    virtual Graphics2D* getGfx2g();
    virtual void requestFlush();
    virtual void registerFace(OswFace* face);
    virtual void registerApp(OswApp* app);
    ~OswLauncher(void){};
  protected:
    OswHal* _hal;
    OswApp *loadedApps[];
    uint8_t appCount;
    uint8_t currentAppIndex;
    
    void load(OswApp* app);
};

#endif