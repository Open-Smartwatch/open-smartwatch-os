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

    uint32_t Hour();
    uint32_t Minute();
    uint32_t Second();

    uint32_t DayOfWeek();
    uint32_t Day();
    uint32_t Month();
    uint32_t Year();
  private:
    time_t time = 0;
};