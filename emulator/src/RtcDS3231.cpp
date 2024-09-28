#include "../include/RtcDS3231.h"

void RtcDateTime::InitWithUnix32Time(time_t time) {
    this->time = time;
}

uint8_t RtcDateTime::Hour() {
    struct tm* parts = std::localtime(&this->time);
    return parts->tm_hour;
}
uint8_t RtcDateTime::Minute() {
    struct tm* parts = std::localtime(&this->time);
    return parts->tm_min;
}
uint8_t RtcDateTime::Second() {
    struct tm* parts = std::localtime(&this->time);
    return parts->tm_sec;
}

uint8_t RtcDateTime::DayOfWeek() {
    struct tm* parts = std::localtime(&this->time);
    return parts->tm_wday;
}
uint8_t RtcDateTime::Day() {
    struct tm* parts = std::localtime(&this->time);
    return parts->tm_mday;
}
uint8_t RtcDateTime::Month() {
    struct tm* parts = std::localtime(&this->time);
    return parts->tm_mon + 1;
}
uint32_t RtcDateTime::Year() {
    struct tm* parts = std::localtime(&this->time);
    return parts->tm_year + 1900;
}