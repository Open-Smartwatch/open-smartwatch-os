#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <RtcDS3231.h>
#include <Wire.h>
#include <time.h>

#include "BlueDot_BMA400.h"
#include "bma400_defs.h"
#include "osw_hal.h"
#include "osw_pins.h"

Adafruit_BME280 bme;
BlueDot_BMA400 bma400 = BlueDot_BMA400();
RtcDS3231<TwoWire> Rtc(Wire);
void IRAM_ATTR isrAlarm() { Serial.println("Alarm"); }

void OswHal::setupSensors() {
  Wire.begin(SDA, SCL, 100000L);
  uint8_t status = bme.begin(0x76, &Wire);
  if (!status) {
    _hasBME280 = false;
  }

  // See: https://platformio.org/lib/show/7125/BlueDot%20BMA400%20Library
  bma400.parameter.I2CAddress = 0x14;                   // default I2C address
  bma400.parameter.powerMode = 0x02;                    // normal mode
  bma400.parameter.measurementRange = BMA400_RANGE_2G;  // 2g range
  bma400.parameter.outputDataRate = BMA400_ODR_100HZ;   // 50 Hz
  bma400.parameter.oversamplingRate = 0x03;             // highest oversampling

  _hasBMA400 = bma400.init() == 0x90;

  bma400.enableStepCounter();

  // TODO: why is chip ID 0 ?
  // Serial.println(bma400.checkID(), 16);

  // if (_hasBMA400) {
  // } else {
  //   Serial.println("has no BMA400");
  // }

  // pinMode(BMA_INT_1, INPUT);
  // pinMode(BMA_INT_2, INPUT);

  // attachInterrupt(BMA_INT_2, isrStepDetect, RISING);

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

  // how to register interrupts:
  // pinMode(RTC_INT, INPUT);
  // attachInterrupt(RTC_INT, isrAlarm, FALLING);
  // Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeAlarmBoth);
  // RtcDateTime now = Rtc.GetDateTime();
  // RtcDateTime alarmTime = now + 10;  // into the future
  // DS3231AlarmOne alarm1(alarmTime.Day(), alarmTime.Hour(), alarmTime.Minute(), alarmTime.Second(),
  //                       DS3231AlarmOneControl_HoursMinutesSecondsMatch);
  // Rtc.SetAlarmOne(alarm1);
}

void OswHal::getTime(uint32_t *hour, uint32_t *minute, uint32_t *second) {
  RtcDateTime d = RtcDateTime();
  d.InitWithEpoch32Time(getTime());
  *hour = d.Hour();
  *minute = d.Minute();
  *second = d.Second();
}

uint32_t OswHal::getTime() { return Rtc.GetDateTime().Epoch32Time(); }
void OswHal::setTime(long epoch) {
  RtcDateTime newDateTime = RtcDateTime();
  newDateTime.InitWithEpoch32Time(epoch);
  Rtc.SetDateTime(newDateTime);
}

void OswHal::updateTimeViaNTP(long gmtOffset_sec, int daylightOffset_sec, uint32_t timeout_sec) {
  long start = millis();
  if (getWiFi()->isConnected()) {
    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

    Serial.println("Waiting for time");

    while (!time(nullptr) && millis() - start < timeout_sec * 1000) {
      Serial.print(".");
      delay(1000);
    }

    if (time(nullptr)) {
      Serial.println("Setting time");
      Serial.println(time(nullptr));
      setTime(time(nullptr));
    }
  }
}

bool OswHal::hasBME280(void) { return _hasBME280; }
bool OswHal::hasBMA400(void) { return _hasBMA400; }
bool OswHal::hasDS3231(void) { return _hasDS3231; }
float OswHal::getPressureHPa(void) { return bme.readPressure() / 100.0F; }

void OswHal::updateAccelerometer(void) { bma400.readData(); }
float OswHal::getAccelerationX(void) { return bma400.parameter.acc_x; };
float OswHal::getAccelerationY(void) { return bma400.parameter.acc_y; };
float OswHal::getAccelerationZ(void) { return bma400.parameter.acc_z; };

uint32_t OswHal::getStepCount(void) { return bma400.readStepCount(); };
void OswHal::enableStepDetection(void) { bma400.enableStepCounter(); };
void OswHal::disableStepDetection(void) { bma400.disableStepCounter(); };
uint8_t OswHal::getActivityMode(void) { return bma400.readActivityMode(); };