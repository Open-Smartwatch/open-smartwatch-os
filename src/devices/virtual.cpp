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

    time_t rawtime = time(NULL);
    struct tm *ptm = gmtime(&rawtime);
    time_t gmt = mktime(ptm);
    ptm = localtime(&rawtime);
    time_t offset = rawtime - gmt + (ptm->tm_isdst ? 3600 : 0);

    OSW_LOG_I("local: ", ptm, " utc: ", gmt);
    OSW_LOG_I("diff: ", offset);
    
    if(gmt == -1 or rawtime == -1)
        throw std::logic_error("Could not represent times in std::time_t?!");
    return offset;
}
#endif
}
