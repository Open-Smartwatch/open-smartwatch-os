#include <stdexcept>

#include <RtcDS3231.h> // ONLY used for the RtcDateTime class, because direct include is buggy!
#include <osw_config.h>

#include <osw_hal.h>

time_t OswHal::getUTCTime() {
  if(!this->timeProvider)
    throw std::runtime_error("No time provider!");
  return this->timeProvider->getUTCTime();
}

void OswHal::setUTCTime(const time_t& epoch) {
  if(!this->timeProvider)
    throw std::runtime_error("No time provider!");
  this->timeProvider->setUTCTime(epoch);
}

void OswHal::updateTimeProvider() {
  for(auto& d : *OswTimeProvider::getAllTimeDevices()) {
    if(this->timeProvider == nullptr or this->timeProvider->getTimeProviderPriority() < d->getTimeProviderPriority())
      this->timeProvider = d;
  }
#ifndef NDEBUG
  if(!this->timeProvider)
    Serial.println(String(__FILE__) + ": Temperature API enabled, but no provider available!");
#endif
}

uint32_t OswHal::getLocalTime() {
  return this->getUTCTime() + 3600 * OswConfigAllKeys::timeZone.get() + (long)(3600 * OswConfigAllKeys::daylightOffset.get());
}
void OswHal::getUTCTime(uint32_t *hour, uint32_t *minute, uint32_t *second) {
  RtcDateTime d = RtcDateTime();
  d.InitWithEpoch32Time(this->getUTCTime());
  *hour = d.Hour();
  *minute = d.Minute();
  *second = d.Second();
}

void OswHal::getLocalTime(uint32_t *hour, uint32_t *minute, uint32_t *second) {
  RtcDateTime d = RtcDateTime();
  d.InitWithEpoch32Time(this->getLocalTime());
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
  d.InitWithEpoch32Time(this->getLocalTime());
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
  d.InitWithEpoch32Time(this->getLocalTime());
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

const char *OswHal::getWeekday() {
  uint32_t day = 0;
  uint32_t wDay = 0;
  this->getDate(&day, &wDay);

  const char *dayMap[7] = {LANG_SUNDAY,   LANG_MONDAY, LANG_TUESDAY, LANG_WEDNESDAY,
                         LANG_THURSDAY, LANG_FRIDAY, LANG_SATURDAY};
  return dayMap[wDay];
}