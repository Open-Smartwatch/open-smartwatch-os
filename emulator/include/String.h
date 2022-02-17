#pragma once

#include <string>
#include <ArduinoJson.hpp>

#include "Defines.h"

class StringSumHelper;

class String : public std::string {
public:
    String() : std::string() {};
    String(const char* str) : std::string(str) {};

    #define _FAKE_STR_CONSTR(T) String(T smth) { \
        std::string(std::to_string(smth)); \
    };

    _FAKE_STR_CONSTR(float)
    _FAKE_STR_CONSTR(double)
    _FAKE_STR_CONSTR(bool)
    _FAKE_STR_CONSTR(int)
    _FAKE_STR_CONSTR(unsigned int)
    _FAKE_STR_CONSTR(short int)
    _FAKE_STR_CONSTR(short unsigned int)
    _FAKE_STR_CONSTR(long unsigned int)
    _FAKE_STR_CONSTR(long int)
    _FAKE_STR_CONSTR(char)
    _FAKE_STR_CONSTR(unsigned char)

    #define _FAKE_STR_CPY_CONSTR(T) String(T smth) : std::string(smth) {};
    _FAKE_STR_CPY_CONSTR(std::string)

    ~String() {};

    // For ArduinoJSON
    size_t write(uint8_t c) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        this->append(std::to_string(c));
        return this->size();
    }
    size_t write(const uint8_t* s, size_t n) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        for(size_t i = 0; i < n; i++)
            this->append(std::to_string(s[n]));
        return this->size();
    }

    // For FakePrint
    operator const char*() const { return this->c_str(); }

    int toInt() const {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return strtol(this->c_str(), nullptr, 10);
    }

    float toFloat() const {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return strtof(this->c_str(), nullptr);
    }

    double toDouble() const {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return strtod(this->c_str(), nullptr);
    }


	//friend StringSumHelper & operator + (const StringSumHelper &lhs, const String &rhs);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, const char *cstr) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return *const_cast<StringSumHelper*>(&lhs);
    };
	friend StringSumHelper & operator + (const StringSumHelper &lhs, char c) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return *const_cast<StringSumHelper*>(&lhs);
    };
	friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned char num) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return *const_cast<StringSumHelper*>(&lhs);
    };
	friend StringSumHelper & operator + (const StringSumHelper &lhs, int num) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return *const_cast<StringSumHelper*>(&lhs);
    };
	friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned int num) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return *const_cast<StringSumHelper*>(&lhs);
    };
	friend StringSumHelper & operator + (const StringSumHelper &lhs, long num) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return *const_cast<StringSumHelper*>(&lhs);
    };
	friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned long num) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return *const_cast<StringSumHelper*>(&lhs);
    };
	friend StringSumHelper & operator + (const StringSumHelper &lhs, float num) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return *const_cast<StringSumHelper*>(&lhs);
    };
	friend StringSumHelper & operator + (const StringSumHelper &lhs, double num) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return *const_cast<StringSumHelper*>(&lhs);
    };
	//friend StringSumHelper & operator + (const StringSumHelper &lhs, const __FlashStringHelper *rhs);
};

class StringSumHelper : public String
{
public:
	StringSumHelper(const String &s) : String(s) {}
	StringSumHelper(const char *p) : String(p) {}
	StringSumHelper(char c) : String(c) {}
	StringSumHelper(unsigned char num) : String(num) {}
	StringSumHelper(int num) : String(num) {}
	StringSumHelper(unsigned int num) : String(num) {}
	StringSumHelper(long num) : String(num) {}
	StringSumHelper(unsigned long num) : String(num) {}
	StringSumHelper(float num) : String(num) {}
	StringSumHelper(double num) : String(num) {}
};

// For ArduinoJSON
bool convertToJson(const String& t, ArduinoJson::JsonVariant variant);