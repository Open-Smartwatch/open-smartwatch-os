#pragma once

#include <time.h>
#include <list>
#include <stdexcept>
#include <WString.h>

#include <devices/OswDevice.h>

class OswTimeProvider : public OswDevice {
  public:
    virtual time_t getUTCTime() = 0;
    virtual void setUTCTime(const time_t& epoch) = 0;

    /**
     * @brief This function shall accept any POSIX timezone string and return the offset in seconds.
     * A list of valid timezone strings can be found here:
     * - https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
     * - https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
     * Make sure to cache the returned value, as some providers may take their time to calculate the offset.
     * Also make sure to update the cached offset from time to time, as the offset may change over time (e.g.
     * depending if DST is active or not).
     *
     * @throws std::logic_error if the provider does not support timezones
     * @param timestamp timestamp to transform
     * @param timezone based on this POSIX string
     * @return time_t offset in seconds
     */
    virtual time_t getTimezoneOffset(const time_t& timestamp, const String& timezone) {
        throw std::logic_error("This provider does not support timezones.");
    };

    virtual unsigned char getTimeProviderPriority() = 0;
    static const std::list<OswTimeProvider*>* getAllTimeDevices() {
        return &allDevices;
    };
  protected:
    OswTimeProvider() : OswDevice() {
        this->allDevices.push_back(this);
    };
    ~OswTimeProvider() {
        this->allDevices.remove(this);
    };
  private:
    static std::list<OswTimeProvider*> allDevices;
};
