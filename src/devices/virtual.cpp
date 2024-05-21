#include <ctime>
#include <WString.h>

#include <devices/virtual.h>

namespace OswDevices {
#ifdef OSW_EMULATOR
time_t Virtual::getTimezoneOffset(const time_t& timestamp, const String& timezone) {
    /*
        This code is just copy-pasted from the ESP32, as they both use the time.h library.
    */

    bool hasOldTimezone = getenv("TZ") != nullptr;
    String oldTimezone; // Variable to hold local copy, as the value by getenv() may change after a setenv()
    if(hasOldTimezone)
        oldTimezone = getenv("TZ");

    setenv("TZ", timezone.c_str(), 1); // overwrite the TZ environment variable
    tzset();
    std::tm localTm = *std::localtime(&timestamp);

    setenv("TZ", "UTC0", 1); // overwrite the TZ environment variable
    tzset();
    std::tm utcTm = *std::localtime(&timestamp);
    std::time_t utc = std::mktime(&utcTm);
    std::time_t local = std::mktime(&localTm); // this removes the "local"/dst offsets by UTC from our localized timestamp -> we get the UTC timestamp INCLUDING the local offsets!
    OSW_LOG_I("local: ", local, " utc: ", utc);
    OSW_LOG_I("diff: ", local - utc);
    if(hasOldTimezone) {
        setenv("TZ", oldTimezone.c_str(), 1); // restore the TZ environment variable
        tzset();
    }
    if(utc == -1 or local == -1)
        throw std::logic_error("Could not represent times in std::time_t?!");
    return local - utc;
}
#endif
}
