#ifndef OSW_APP_HELLO_WORLD_H
#define OSW_APP_HELLO_WORLD_H

#include <osw_hal.h>

#include <OswAppV2.h>

class OswAppTutorial : public OswAppV2 {
  public:
    OswAppTutorial() {};
    ~OswAppTutorial() {};

    const char* getAppId() override;
    const char* getAppName() override;

    // void onStart() override;
    // void onLoop() override;
    // void onDraw() override;
    // void onStop() override;

  private:
};

#endif
