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

/**
 * @brief Update the internal daylight offset(s).
 * As this is a time consuming operation - so make sure to respect its return value, which indicates the next time to call this method!
 * 
 * @return time_t 
 */
time_t OswHal::updateDaylightOffsets() {
    if(!OswConfigAllKeys::daylightOffset.get()) {
        this->daylightOffset = 0;
        return 0; // Come back later, nothing to do for now...
    }

    // Get now
    time_t now = time(nullptr);
    struct tm * timeinfo;
    timeinfo = localtime(&now);

    // Find first date + time (last sunday of march at 02:00)
    timeinfo->tm_mon = 2; // march
    timeinfo->tm_mday = 31;
    timeinfo->tm_hour = 2;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    time_t start = mktime(timeinfo);
    while(timeinfo->tm_wday != 6) {
        start -= 24 * 60 * 60; // Scroll back to the last sunday of the month
        timeinfo = localtime(&start);
    }

    // Find last date + time (last sunday of october at 03:00)
    timeinfo->tm_mon = 9; // october
    timeinfo->tm_mday = 31;
    timeinfo->tm_hour = 3;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    time_t end = mktime(timeinfo);
    while(timeinfo->tm_wday != 6) {
        end -= 24 * 60 * 60; // Scroll back to the last sunday of the month
        timeinfo = localtime(&end);
    }
    const bool summertime = (start <= now and now <= end);

    // Test if we are start < now < end -> set offset to 1h + return end
    if(summertime) {
        OSW_LOG_D("Daylight offset is active (1h)");
        this->daylightOffset = 60 * 60;
        return end;
    } else {
        OSW_LOG_D("Daylight offset is inactive");
        this->daylightOffset = 0;
        if(start <= now and end <= now)
            return now + 30 * 24 * 60 * 60; // Try again in a month - if we enter the next year the correct (↓) start will be returned
        else
            return start;
    }
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

time_t OswHal::getDaylightOffset() {
    return this->daylightOffset;
}

uint32_t OswHal::getTime(short timezone) {
    return this->getUTCTime() + (3600 * timezone) + this->daylightOffset;
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