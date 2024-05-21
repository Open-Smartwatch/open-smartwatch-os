#pragma once
#include <array>
#include <optional>

#include <OswAppV1.h>
#include <OswAppV2.h>

class OswAppV2Compat: public OswAppV2 {
  public:
    OswAppV2Compat(const char* id, const char* name, OswAppV1& app, bool keepScreenOn = true, std::optional<std::reference_wrapper<const OswIcon>> icon = std::nullopt);

    const char* getAppId() override;
    const char* getAppName() override;
    const OswIcon& getAppIcon() override;

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
    const OswIcon& icon;
    OswAppV1& app;
    bool keepScreenOn;

    // For UI testing purposes (to access private member "app")
    friend class TestAppV2Compat;
};
