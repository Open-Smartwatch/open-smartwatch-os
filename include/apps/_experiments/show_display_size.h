#ifndef OSW_APP_SHOW_DISPLAY_SIZE_H
#define OSW_APP_SHOW_DISPLAY_SIZE_H

#include <osw_hal.h>

#include <OswAppV1.h>

class OswAppShowDisplaySize : public OswApp {
  public:
    OswAppShowDisplaySize(void) {};
    virtual void loop() override;
    ~OswAppShowDisplaySize() {};

  private:
};

#endif
