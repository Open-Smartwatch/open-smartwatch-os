#include "../include/String.h"
#include "../include/Defines.h"

StringSumHelper& operator+(const StringSumHelper& lhs, const String& rhs) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs.append(rhs.c_str());
    return mLhs;
}

StringSumHelper& operator+(const StringSumHelper& lhs, char c) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs += c;
    return mLhs;
}

StringSumHelper& operator+(const StringSumHelper& lhs, unsigned char num) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs.append(std::to_string(num));
    return mLhs;
}

StringSumHelper& operator+(const StringSumHelper& lhs, short num) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs.append(std::to_string(num));
    return mLhs;
}

StringSumHelper& operator+(const StringSumHelper& lhs, unsigned short num) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs.append(std::to_string(num));
    return mLhs;
}

StringSumHelper& operator+(const StringSumHelper& lhs, int num) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs.append(std::to_string(num));
    return mLhs;
}

StringSumHelper& operator+(const StringSumHelper& lhs, unsigned int num) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs.append(std::to_string(num));
    return mLhs;
}

StringSumHelper& operator+(const StringSumHelper& lhs, long num) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs.append(std::to_string(num));
    return mLhs;
}

StringSumHelper& operator+(const StringSumHelper& lhs, unsigned long num) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs.append(std::to_string(num));
    return mLhs;
}

StringSumHelper& operator+(const StringSumHelper& lhs, float num) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs.append(std::to_string(num));
    return mLhs;
}

StringSumHelper& operator+(const StringSumHelper& lhs, double num) {
    StringSumHelper& mLhs = const_cast<StringSumHelper&>(lhs);
    mLhs.append(std::to_string(num));
    return mLhs;
}