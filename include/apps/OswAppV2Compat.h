#pragma once
#include <array>

#include <OswAppV1.h>
#include <OswAppV2.h>

class OswAppV2Compat: public OswAppV2 {
  public:
    OswAppV2Compat(const char* id, const char* name, OswAppV1& app, bool keepScreenOn = true);

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onDrawOverlay() override;
    void onStop() override;

#ifdef OSW_EMULATOR
    void onLoopDebug() override;
#endif

  private:
    const char* id;
    const char* name;
    OswAppV1& app;
    bool keepScreenOn;
};