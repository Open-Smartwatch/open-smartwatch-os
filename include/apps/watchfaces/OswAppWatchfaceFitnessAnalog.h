#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV2.h>

class OswAppWatchfaceFitnessAnalog : public OswAppV2
{
public:
  constexpr static const char *APP_ID = "osw.wf.afit";

  const char *getAppId() override;
  const char *getAppName() override;

  void onStart();
  void onLoop() override;
  void onDraw() override;
  void onButton(Button id, bool up, ButtonStateNames state) override;
  void onStop() override;

  static uint32_t calculateDistance(uint32_t steps);
  
  void timeDisplay(OswHal *hal, uint32_t hour, uint32_t minute, uint32_t second);
  void timeDisplay(OswHal *hal, uint32_t hour, uint32_t minute, uint32_t second, bool afterNoon);
  void dateDisplay(OswHal *hal, uint32_t hour, uint32_t minute, uint32_t second, bool afterNoon);

  void test();
  
  ~OswAppWatchfaceFitnessAnalog() {}

private:
  time_t lastTime = 0;
  unsigned screen = 0;

  void showFitnessTracking(OswHal *hal);
  void drawWatchFace(OswHal *hal, uint32_t hour, uint32_t minute, uint32_t second, bool afterNoon);
  void drawDateFace(OswHal *hal, uint32_t hour, uint32_t minute, uint32_t second, bool afterNoon);
};
