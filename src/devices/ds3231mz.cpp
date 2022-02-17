#include <stdexcept>

#include <devices/ds3231mz.h>

void OswDevices::DS3231MZ::setup() {
  Rtc.Enable32kHzPin(false);
  if (!Rtc.LastError()) {
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    if (!Rtc.IsDateTimeValid())
      Rtc.SetDateTime(compiled);

    if (!Rtc.GetIsRunning()) {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
    }

    // Fetch the first time.
    this->update();
  } else
    throw std::runtime_error("Failed to initialize DS3231MZ!");

  // how to register interrupts:
  // pinMode(OSW_DEVICE_DS3231MZ_RTCINT, INPUT);
  // attachInterrupt(OSW_DEVICE_DS3231MZ_RTCINT, isrAlarm, FALLING);
  // Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeAlarmBoth);
  // RtcDateTime now = Rtc.GetDateTime();
  // RtcDateTime alarmTime = now + 10;  // into the future
  // DS3231AlarmOne alarm1(alarmTime.Day(), alarmTime.Hour(), alarmTime.Minute(), alarmTime.Second(),
  //                       DS3231AlarmOneControl_HoursMinutesSecondsMatch);
  // Rtc.SetAlarmOne(alarm1);
}

void OswDevices::DS3231MZ::update() {
   uint32_t temp = Rtc.GetDateTime().Epoch32Time();
  if (!Rtc.LastError()) {
    // success on first attempt
    _utcTime = temp;
    return;
  }

  // try harder
  uint8_t tries = 10;
  while (_utcTime == 0 && tries > 0) {
    temp = Rtc.GetDateTime().Epoch32Time();
    if (!Rtc.LastError()) {
      // success on n-th attempt
      _utcTime = temp;
      return;
    }
    tries--;
  }

  // fail, assume compile time as closest time in the past
  _utcTime = RtcDateTime(__DATE__, __TIME__).Epoch32Time();
}

time_t OswDevices::DS3231MZ::getUTCTime() {
  // I2C access :(
  return _utcTime;
}

void OswDevices::DS3231MZ::setUTCTime(const time_t& epoch) {
  RtcDateTime t = RtcDateTime();
  t.InitWithEpoch32Time(epoch);
  Rtc.SetDateTime(t);
}

float OswDevices::DS3231MZ::getTemperature() {
  RtcTemperature rtcTemp = Rtc.GetTemperature();
  if (Rtc.LastError())
    return 0.0f;
  return rtcTemp.AsFloatDegC();
}