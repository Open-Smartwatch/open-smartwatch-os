#include <RtcDS3231.h>
#include <Wire.h>
#include <config.h>
#include <osw_config.h>
#include <time.h>

#include "osw_hal.h"
RtcDS3231<TwoWire> Rtc(Wire);

const char *dayMap[7] = {LANG_SUNDAY,   LANG_MONDAY, LANG_TUESDAY, LANG_WEDNESDAY,
                         LANG_THURSDAY, LANG_FRIDAY, LANG_SATURDAY};

void OswHal::setupTime(void) {
  Rtc.Enable32kHzPin(false);
  if (!Rtc.LastError()) {
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

    // this can mess up the time in some weird cases
    // also not helpful if you use precompiled images as the time is off by
    // a lot
    // RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    // if (!Rtc.IsDateTimeValid()) {
    //   Rtc.SetDateTime(compiled);
    // }

    if (!Rtc.GetIsRunning()) {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
    }

    //fetch the first time.
    updateRtc();
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

bool OswHal::hasDS3231(void) { return getUTCTime() > 0; }
void OswHal::updateRtc(void) {
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

uint32_t OswHal::getUTCTime(void) {
  // I2C access :(
  return _utcTime;
}

uint32_t OswHal::getLocalTime(void) {
  return getUTCTime() + 3600 * OswConfigAllKeys::timeZone.get() + (long)(3600 * OswConfigAllKeys::daylightOffset.get());
}

void OswHal::setUTCTime(long epoch) {
  RtcDateTime t = RtcDateTime();
  t.InitWithEpoch32Time(epoch);
  Rtc.SetDateTime(t);
}

void OswHal::getUTCTime(uint32_t *hour, uint32_t *minute, uint32_t *second) {
  RtcDateTime d = RtcDateTime();
  d.InitWithEpoch32Time(getUTCTime());
  *hour = d.Hour();
  *minute = d.Minute();
  *second = d.Second();
}

void OswHal::getLocalTime(uint32_t *hour, uint32_t *minute, uint32_t *second) {
  RtcDateTime d = RtcDateTime();
  d.InitWithEpoch32Time(getLocalTime());
  if (!OswConfigAllKeys::timeFormat.get()) {
    if (d.Hour() > 12) {
      *hour = d.Hour() - 12;
    } else if (d.Hour() == 0) {
      *hour = 12;
    } else {
      *hour = d.Hour();
    }
  } else {
    *hour = d.Hour();
  }
  *minute = d.Minute();
  *second = d.Second();
}

void OswHal::getLocalTime(uint32_t *hour, uint32_t *minute, uint32_t *second, bool *afterNoon) {
  RtcDateTime d = RtcDateTime();
  d.InitWithEpoch32Time(getLocalTime());
  if (!OswConfigAllKeys::timeFormat.get()) {
    if (d.Hour() > 12) {
      *hour = d.Hour() - 12;
      *afterNoon = true;
    } else if (d.Hour() == 0) {
      *hour = 12;
      *afterNoon = false;
    } else if (d.Hour() == 12) {
      *hour = d.Hour();
      *afterNoon = true;
    } else {
      *hour = d.Hour();
      *afterNoon = false;
    }
  } else {
    *hour = d.Hour();
    *afterNoon = false;
  }
  *minute = d.Minute();
  *second = d.Second();
}

void OswHal::getDate(uint32_t *day, uint32_t *weekDay) {
  RtcDateTime d = RtcDateTime();
  d.InitWithEpoch32Time(getLocalTime());
  *weekDay = d.DayOfWeek();
  *day = d.Day();
}

void OswHal::getDate(uint32_t *day, uint32_t *month, uint32_t *year) {
  RtcDateTime d = RtcDateTime();
  d.InitWithEpoch32Time(getLocalTime());
  *day = d.Day();
  *month = d.Month();
  *year = d.Year();
}

const char *OswHal::getWeekday(void) {
  uint32_t day = 0;
  uint32_t wDay = 0;
  getDate(&day, &wDay);

  return dayMap[wDay];
}

#if !defined(GPS_EDITION) && !defined(GPS_EDITION_ROTATED)
// Okay, in that case let's use the alternative way of getting the temperature by asking the RTC!
float OswHal::getTemperature() {
  RtcTemperature rtcTemp = Rtc.GetTemperature();
  if (Rtc.LastError())
    return 0.0f;
  return rtcTemp.AsFloatDegC();
}
#endif