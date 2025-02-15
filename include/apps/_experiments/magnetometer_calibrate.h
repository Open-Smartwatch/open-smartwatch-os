#pragma once

#include <osw_hal.h>
#include <OswAppV1.h>

#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1 && (OSW_PLATFORM_HARDWARE_QMC5883L == 1 || OSW_PLATFORM_HARDWARE_QMC5883P == 1)
class OswAppMagnetometerCalibrate : public OswApp {
  public:
    OswAppMagnetometerCalibrate(void) {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppMagnetometerCalibrate() {};

  private:
};

#endif
