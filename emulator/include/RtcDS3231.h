#pragma once

#include <chrono>
#include <ctime>

#include "time.h"

#include "Defines.h"

/// This class is only used for simpler time parsing - not for providing the time itself!
class RtcDateTime {
  public:
    RtcDateTime() {};
    virtual ~RtcDateTime() {};

    void InitWithUnix32Time(time_t time);

    uint8_t Hour();
    uint8_t Minute();
    uint8_t Second();

    uint8_t DayOfWeek();
    uint8_t Day();
    uint8_t Month();
    uint32_t Year();
  private:
    time_t time = 0;
};