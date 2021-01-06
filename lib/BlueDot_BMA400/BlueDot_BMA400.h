

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SPI.h"
#include "Wire.h"

#define BMA400_CHIP_ID 0x00

enum Coefficients {
  BMA400_STATUS = 0x03,

  BMA400_ACC_X_LSB = 0x04,
  BMA400_ACC_X_MSB = 0x05,
  BMA400_ACC_Y_LSB = 0x06,
  BMA400_ACC_Y_MSB = 0x07,
  BMA400_ACC_Z_LSB = 0x08,
  BMA400_ACC_Z_MSB = 0x09,

  BMA400_TEMP_ADDR = 0x11,
  BMA400_ACC_CONFIG0 = 0x19,
  BMA400_ACC_CONFIG1 = 0x1A,

};

struct BMA400_Coefficients {};

struct DeviceParameter {
  uint8_t I2CAddress;
  uint8_t powerMode;
  uint8_t measurementRange;
  uint8_t outputDataRate;
  uint8_t oversamplingRate;
  int16_t raw_acc_x;
  int16_t raw_acc_y;
  int16_t raw_acc_z;
  float acc_x;
  float acc_y;
  float acc_z;
};

class BlueDot_BMA400 {
 public:
  DeviceParameter parameter;
  BMA400_Coefficients bma400_coefficients;

  BlueDot_BMA400();
  uint8_t init(void);
  uint8_t checkID(void);

  void setPowerMode();
  void setMeasurementRange();
  void setOutputDataRate();
  void setOversamplingRate();
  void enableStepCounter();
  void disableStepCounter();

  uint8_t readPowerMode();
  uint8_t readMeasurementRange();
  uint8_t readOutputDataRate();
  uint8_t readOversamplingRate();
  uint32_t readStepCount();
  uint8_t readActivityMode();

  void readData();
  void convertData();

  uint8_t readByte(byte reg);
  void writeByte(byte reg, byte value);
};
