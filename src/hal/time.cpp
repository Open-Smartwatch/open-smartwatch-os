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

void OswHal::getUTCTime(uint32_t* hour, uint32_t* minute, uint32_t* second) {
    RtcDateTime d = RtcDateTime();
    d.InitWithEpoch32Time(this->getUTCTime());
    *hour = d.Hour();
    *minute = d.Minute();
    *second = d.Second();
}

void OswHal::getTime(short timezone, uint32_t* hour, uint32_t* minute, uint32_t* second, bool* afterNoon) {
    RtcDateTime d = RtcDateTime();
    d.InitWithEpoch32Time(this->getTime(timezone));
    if (!OswConfigAllKeys::timeFormat.get()) {
        if (d.Hour() > 12) {
            *hour = d.Hour() - 12;
            if (afterNoon != nullptr) *afterNoon = true;
        } else if (d.Hour() == 0) {
            *hour = 12;
            if (afterNoon != nullptr) *afterNoon = false;
        } else if (d.Hour() == 12) {
            *hour = d.Hour();
            if (afterNoon != nullptr) *afterNoon = true;
        } else {
            *hour = d.Hour();
            if (afterNoon != nullptr) *afterNoon = false;
        }
    } else {
        *hour = d.Hour();
        if (afterNoon != nullptr) *afterNoon = false;
    }
    *minute = d.Minute();
    *second = d.Second();
}

uint32_t OswHal::getTime(short timezone) {
    return this->getUTCTime() + (3600 * timezone) + (long)(3600 * OswConfigAllKeys::daylightOffset.get());
}

void OswHal::getDate(short timezone, uint32_t* day, uint32_t* weekDay) {
    RtcDateTime d = RtcDateTime();
    d.InitWithEpoch32Time(this->getTime(timezone));
    *weekDay = d.DayOfWeek();
    *day = d.Day();
}

void OswHal::getDate(short timezone, uint32_t* day, uint32_t* month, uint32_t* year) {
    RtcDateTime d = RtcDateTime();
    d.InitWithEpoch32Time(this->getTime(timezone));
    *day = d.Day();
    *month = d.Month();
    *year = d.Year();
}

const char* OswHal::getWeekday(short timezone, uint32_t* setWDay) {
    uint32_t day = 0;
    uint32_t wDay = 0;
    this->getDate(timezone, &day, &wDay);

    const char* dayMap[7] = {LANG_SUNDAY, LANG_MONDAY, LANG_TUESDAY, LANG_WEDNESDAY, LANG_THURSDAY, LANG_FRIDAY, LANG_SATURDAY};
    if (setWDay != nullptr) wDay = *setWDay;
    return dayMap[wDay];
}