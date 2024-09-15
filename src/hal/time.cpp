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
    if(!this->timeProvider)
        OSW_LOG_D("No provider for Time is available!");
}

void OswHal::getUTCTime(uint32_t* hour, uint32_t* minute, uint32_t* second) {
    RtcDateTime d = RtcDateTime();
    d.InitWithUnix32Time(this->getUTCTime());
    if (hour != nullptr){
        *hour = d.Hour();
    }
    if (minute != nullptr){
        *minute = d.Minute();
    }
    if (second != nullptr){
        *second = d.Second();
    }
}

void OswHal::getTime(time_t& offset, uint32_t* hour, uint32_t* minute, uint32_t* second, bool* afterNoon) {
    RtcDateTime d = RtcDateTime();
    d.InitWithUnix32Time(this->getTime(offset));
    if (!OswConfigAllKeys::timeFormat.get()) {
        if (d.Hour() > 12) {
            if(hour != nullptr){
                *hour = d.Hour() - 12;
            }
            if (afterNoon != nullptr) *afterNoon = true;
        } else if (d.Hour() == 0) {
            if(hour != nullptr){
                *hour = 12;
            }
            if (afterNoon != nullptr) *afterNoon = false;
        } else if (d.Hour() == 12) {
            if(hour != nullptr){
                *hour = d.Hour();
            }
            if (afterNoon != nullptr) *afterNoon = true;
        } else {
            if(hour != nullptr){
                *hour = d.Hour();
            }
            if (afterNoon != nullptr) *afterNoon = false;
        }
    } else {
        if(hour != nullptr){
            *hour = d.Hour();
        }
        if (afterNoon != nullptr) *afterNoon = false;
    }
    if (minute != nullptr){
        *minute = d.Minute();
    }
    if (second != nullptr){
        *second = d.Second();
    }
}

/**
 * @brief Tries to update the cached timezone offsets for the primary and secondary timezones.
 *
 */
void OswHal::updateTimezoneOffsets() {
    // Ask primary time provider for timezone offset
    const String timezonePrimary = OswConfigAllKeys::timezonePrimary.get();
    const String timezoneSecondary = OswConfigAllKeys::timezoneSecondary.get();
    if(timezonePrimary.length() == 0)
        this->timezoneOffsetPrimary = 0;
    if(timezoneSecondary.length() == 0)
        this->timezoneOffsetSecondary = 0;
    if(timezonePrimary.length() == 0 and timezoneSecondary.length() == 0)
        return; // No timezone != UTC is set -> no need to update
    bool updated = false;
    try {
        if(this->timeProvider != nullptr) {
            if(timezonePrimary.length() != 0)
                this->timezoneOffsetPrimary = this->timeProvider->getTimezoneOffset(this->timeProvider->getUTCTime(), timezonePrimary);
            if(timezoneSecondary.length() != 0)
                this->timezoneOffsetSecondary = this->timeProvider->getTimezoneOffset(this->timeProvider->getUTCTime(), timezoneSecondary);
            updated = true;
        }
    } catch(const std::logic_error& e) {
        // Well, we tried...
    }
    // Okay, either we have no time provider or it failed to provide a timezone offset -> ask all remaining time providers to resolve it!
    if(!updated) {
        for(auto& d : *OswTimeProvider::getAllTimeDevices()) {
            if(d != this->timeProvider) {
                try {
                    if(timezonePrimary.length() != 0)
                        this->timezoneOffsetPrimary = d->getTimezoneOffset(d->getUTCTime(), timezonePrimary);
                    if(timezoneSecondary.length() != 0)
                        this->timezoneOffsetSecondary = d->getTimezoneOffset(d->getUTCTime(), timezoneSecondary);
                    updated = true;
                    break;
                } catch(const std::logic_error& e) {
                    // Well, we tried...
                }
            }
        }
    }
    if(!updated) {
        // Okay, we still don't have a timezone offset -> set it to 0
        this->timezoneOffsetPrimary = 0;
        this->timezoneOffsetSecondary = 0;
        OSW_LOG_W("Could not resolve timezone offsets (no capable provider is available)!");
    }
}

time_t OswHal::getTimezoneOffsetPrimary() {
    return this->timezoneOffsetPrimary;
}

time_t OswHal::getTimezoneOffsetSecondary() {
    return this->timezoneOffsetSecondary;
}

time_t OswHal::getTime(time_t& offset) {
    return this->getUTCTime() + offset;
}

void OswHal::getDate(time_t& offset, uint32_t* day, uint32_t* weekDay) {
    RtcDateTime d = RtcDateTime();
    d.InitWithUnix32Time(this->getTime(offset));
    if (weekDay != nullptr){
        *weekDay = d.DayOfWeek();
    }
    if (day != nullptr){
        *day = d.Day();
    }
}

void OswHal::getDate(time_t& offset, uint32_t* day, uint32_t* month, uint32_t* year) {
    RtcDateTime d = RtcDateTime();
    d.InitWithUnix32Time(this->getTime(offset));
    if (day != nullptr){
        *day = d.Day();
    }
    if (month != nullptr){
        *month = d.Month();
    }
    if (year != nullptr){
        *year = d.Year();
    }
}

const char* OswHal::getWeekday(time_t& offset, uint32_t* setWDay) {
    uint32_t day = 0;
    uint32_t wDay = 0;
    const char* dayMap[7] = {LANG_SUNDAY, LANG_MONDAY, LANG_TUESDAY, LANG_WEDNESDAY, LANG_THURSDAY, LANG_FRIDAY, LANG_SATURDAY};
    this->getDate(offset, &day, &wDay);
    if (setWDay != nullptr) wDay = *setWDay;
    return dayMap[wDay];
}
