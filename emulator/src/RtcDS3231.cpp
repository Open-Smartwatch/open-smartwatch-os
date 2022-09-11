#include "../include/RtcDS3231.h"

void RtcDateTime::InitWithEpoch32Time(time_t time) {
    this->time = time;
}

uint32_t RtcDateTime::Hour() {
    struct tm *parts = std::localtime(&this->time);
    return parts->tm_hour;
}
uint32_t RtcDateTime::Minute() {
    struct tm *parts = std::localtime(&this->time);
    return parts->tm_min;
}
uint32_t RtcDateTime::Second() {
    struct tm *parts = std::localtime(&this->time);
    return parts->tm_sec;
}

uint32_t RtcDateTime::DayOfWeek() {
    struct tm *parts = std::localtime(&this->time);
    return parts->tm_wday;
}
uint32_t RtcDateTime::Day() {
    struct tm *parts = std::localtime(&this->time);
    return parts->tm_mday;
}
uint32_t RtcDateTime::Month() {
    struct tm *parts = std::localtime(&this->time);
    return parts->tm_mon + 1;
}
uint32_t RtcDateTime::Year() {
    struct tm *parts = std::localtime(&this->time);
    return parts->tm_year + 1900;
}