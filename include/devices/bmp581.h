#pragma once
#include OSW_TARGET_PLATFORM_HEADER
#if OSW_PLATFORM_HARDWARE_BMP581 == 1

#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswPressureProvider.h>
#include <bmp5_defs.h>
#include <bmp5.h>

namespace OswDevices {
class BMP581 : public OswPressureProvider, public OswTemperatureProvider {
  public:
    BMP581() : OswPressureProvider(), OswTemperatureProvider() {};
    virtual ~BMP581() {};

    virtual void setup() override;
    virtual void update() override;
    virtual void reset() override {};
    virtual void stop() override {};

    virtual inline const char* getName() override {
        return "BMP581";
    };

    virtual float getPressure() override;
    virtual unsigned char getPressureProviderPriority() override {
        return 100;
    }; // This is a specialized device!

    virtual float getTemperature() override;
    virtual inline unsigned char getTemperatureProviderPriority() override {
        return 20;
    }; // This sensor is not sooo good...
  private:
    bmp5_dev bmp5 = {};
    bmp5_osr_odr_press_config osr_odr_press_cfg = {};
    float pressure = 0;
    float temperature = 0;
};
};
#endif