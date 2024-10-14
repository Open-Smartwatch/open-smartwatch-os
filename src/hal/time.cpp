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

void OswHal::getUTCTime(OswTime& oswTime) {
    RtcDateTime d = RtcDateTime();
    d.InitWithUnix32Time(this->getUTCTime());
    oswTime.hour = d.Hour();
    oswTime.minute = d.Minute();
    oswTime.second = d.Second();
}

void OswHal::getTime(time_t& offset, OswTime& oswTime) {
    RtcDateTime d = RtcDateTime();
    d.InitWithUnix32Time(this->getTime(offset));
    if (!OswConfigAllKeys::timeFormat.get()) {
        if (d.Hour() > 12) {
            oswTime.hour = d.Hour() - 12;
            oswTime.afterNoon = true;
        } else if (d.Hour() == 0) {
            oswTime.hour = 12;
            oswTime.afterNoon = false;
        } else if (d.Hour() == 12) {
            oswTime.hour = d.Hour();
            oswTime.afterNoon = true;
        } else {
            oswTime.hour = d.Hour();
            oswTime.afterNoon = false;
        }
    } else {
        oswTime.hour = d.Hour();
        oswTime.afterNoon = false;
    }
    oswTime.minute = d.Minute();
    oswTime.second = d.Second();
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

void OswHal::getDate(time_t& offset, OswDate& oswDate) {
    RtcDateTime d = RtcDateTime();
    d.InitWithUnix32Time(this->getTime(offset));
    oswDate.year = d.Year();
    oswDate.month = d.Month();
    oswDate.day = d.Day();
    oswDate.weekDay = d.DayOfWeek();
}