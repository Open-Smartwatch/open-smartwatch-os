#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <RtcDS3231.h>
#include <Wire.h>

#include "BlueDot_BMA400.h"
#include "osw_hal.h"
#include "osw_pins.h"

Adafruit_BME280 bme;
BlueDot_BMA400 bma400 = BlueDot_BMA400();
RtcDS3231<TwoWire> Rtc(Wire);

void OswHal::setupSensors() {
  Wire.begin(SDA, SCL, 100000L);
  uint8_t status = bme.begin(0x76, &Wire);
  if (!status) {
    _hasBME280 = false;
  }

  // See: https://platformio.org/lib/show/7125/BlueDot%20BMA400%20Library
  bma400.parameter.I2CAddress = 0x14;        // default I2C address
  bma400.parameter.powerMode = 0x02;         // normal mode
  bma400.parameter.measurementRange = 0x00;  // 2g range
  bma400.parameter.outputDataRate = 0x07;    // 50 Hz
  bma400.parameter.oversamplingRate = 0x03;  // highest oversampling

  _hasBMA400 = bma400.init() == 0x90;

  Rtc.Begin();
  Rtc.Enable32kHzPin(false);
  _hasDS3231 = !Rtc.LastError();
  if (_hasDS3231) {
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

    if (!Rtc.IsDateTimeValid()) {
      Rtc.SetDateTime(compiled);
    }
    if (!Rtc.GetIsRunning()) {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
    }
  }
}

uint32_t OswHal::getTime() { return Rtc.GetDateTime().Epoch32Time(); }
bool OswHal::hasBME280(void) { return _hasBME280; }
bool OswHal::hasBMA400(void) { return _hasBMA400; }
bool OswHal::hasDS3231(void) { return _hasDS3231; }
float OswHal::getPressureHPa(void) { return bme.readPressure() / 100.0F; }

void OswHal::updateAccelerometer(void) { bma400.readData(); }
float OswHal::getAccelerationX(void) { return bma400.parameter.acc_x; };
float OswHal::getAccelerationY(void) { return bma400.parameter.acc_y; };
float OswHal::getAccelerationZ(void) { return bma400.parameter.acc_z; };