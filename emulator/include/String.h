#pragma once

#include <string>
#include <sstream> // DO NOT use this on the real hardware, this is waaaay to fat!
#include <ArduinoJson.hpp>

#include "Defines.h"

class StringSumHelper;

class String : public std::string {
  public:
    String() : std::string() {};
    String(const char* str) : std::string(str) {};

// Whenever gcc supports std::format, we should update these defines accordingly!
#define _FAKE_STR_CONSTR(T) String(T smth) : std::string(std::to_string(smth)) { };

    _FAKE_STR_CONSTR(bool)
    _FAKE_STR_CONSTR(char)

#define _FAKE_STR_CONSTR_BASED(T) String(T smth, int base = 10) : std::string(String::numberToString(smth, base)) { };
    _FAKE_STR_CONSTR_BASED(float)
    _FAKE_STR_CONSTR_BASED(double)
    _FAKE_STR_CONSTR_BASED(unsigned char)
    _FAKE_STR_CONSTR_BASED(short)
    _FAKE_STR_CONSTR_BASED(unsigned short)
    _FAKE_STR_CONSTR_BASED(int)
    _FAKE_STR_CONSTR_BASED(unsigned int)
    _FAKE_STR_CONSTR_BASED(long)
    _FAKE_STR_CONSTR_BASED(unsigned long)

#define _FAKE_STR_CPY_CONSTR(T) String(T smth) : std::string(smth) {};
    _FAKE_STR_CPY_CONSTR(std::string)

    ~String() {};

    // For ArduinoJSON
    size_t write(uint8_t c) {
        this->append(std::to_string(c));
        return this->size();
    }
    size_t write(const uint8_t* s, size_t n) {
        for(size_t i = 0; i < n; i++)
            this->append(std::to_string(s[n]));
        return this->size();
    }

    int toInt() const {
        return strtol(this->c_str(), nullptr, 10);
    }

    float toFloat() const {
        return strtof(this->c_str(), nullptr);
    }

    double toDouble() const {
        return strtod(this->c_str(), nullptr);
    }

    String substring(int from, int to = -1) const {
        if(to == -1)
            return String(this->substr(from));
        else
            return String(this->substr(from, to - from));
    }

    /**
     * @brief This template forwards the append operation to the std::string, but ensures that the result is a String instance.
     *
     * @tparam T
     * @param smth
     * @return String
     */
    template<typename T> String operator+(const T& smth) const {
        String res(*this);
        // Whenever gcc supports std::format, we should update these defines accordingly!
        if constexpr (std::is_same<T, short>::value or std::is_same<T, int>::value or std::is_same<T, long>::value or std::is_same<T, unsigned short>::value or std::is_same<T, unsigned int>::value or std::is_same<T, unsigned long>::value or std::is_same<T, float>::value or std::is_same<T, double>::value) {
            std::stringstream sstream;
            sstream << res << smth; // this takes care of properly formatting the number (like stripping zeros)
            res = sstream.str();
        } else if constexpr (std::is_same<T, char>::value or std::is_same<T, unsigned char>::value)
            res.push_back(smth); // Not using append(), as that function does not support "char"
        else
            res.append(smth);
        return res;
    }

    /**
     * @brief This template forwards the append operation to the std::string, but ensures that the result is a String instance.
     *
     * @tparam T
     * @param smth
     * @return String
     */
    template<typename T> String& operator+=(const T& smth) {
        // Whenever gcc supports std::format, we should update these defines accordingly!
        if constexpr (std::is_same<T, float>::value or std::is_same<T, double>::value or std::is_same<T, short>::value or std::is_same<T, int>::value or std::is_same<T, long>::value or std::is_same<T, unsigned short>::value or std::is_same<T, unsigned int>::value or std::is_same<T, unsigned long>::value) {
            std::stringstream sstream;
            sstream << *this << smth; // this takes care of properly formatting the number (like stripping zeros)
            *this = sstream.str();
        } else if constexpr (std::is_same<T, char>::value or std::is_same<T, unsigned char>::value)
            this->push_back(smth); // Not using append(), as that function does not support "char"
        else
            this->append(smth);
        return *this;
    }

    friend class StringSumHelper;
    friend StringSumHelper& operator + (const StringSumHelper& lhs, const String& rhs);
    friend StringSumHelper& operator + (const StringSumHelper& lhs, char c);
    friend StringSumHelper& operator + (const StringSumHelper& lhs, unsigned char num);
    friend StringSumHelper& operator + (const StringSumHelper& lhs, short num);
    friend StringSumHelper& operator + (const StringSumHelper& lhs, unsigned short num);
    friend StringSumHelper& operator + (const StringSumHelper& lhs, int num);
    friend StringSumHelper& operator + (const StringSumHelper& lhs, unsigned int num);
    friend StringSumHelper& operator + (const StringSumHelper& lhs, long num);
    friend StringSumHelper& operator + (const StringSumHelper& lhs, unsigned long num);
    friend StringSumHelper& operator + (const StringSumHelper& lhs, float num);
    friend StringSumHelper& operator + (const StringSumHelper& lhs, double num);

    // NOTE: As this String inherits from std::string, we don't have to implement "char* + String" and "String[SumHelper] + char*" operators

  private:
    static std::string numberToString(auto num, int base) {
        // TODO extend this for binary
        std::stringstream sstream;
        switch(base) {
            case HEX:
                sstream << std::hex << num;
                break;
            case DEC:
                sstream << std::dec << num;
                break;
            case OCT:
                sstream << std::oct << num;
                break;
            default:
                throw std::invalid_argument("Unknown base for numberToString()!");
        }
        return sstream.str();
    }
};

class StringSumHelper : public String {
  public:
    StringSumHelper(char c) : String(c) {}
    StringSumHelper(unsigned char num) : String(num) {}
    StringSumHelper(short num) : String(num) {}
    StringSumHelper(unsigned short num) : String(num) {}
    StringSumHelper(int num) : String(num) {}
    StringSumHelper(unsigned int num) : String(num) {}
    StringSumHelper(long num) : String(num) {}
    StringSumHelper(unsigned long num) : String(num) {}
    StringSumHelper(float num) : String(num) {}
    StringSumHelper(double num) : String(num) {}

    // NOTE: No need to add a constructor to take in a String, as StringSumHelper inherits from String
};

// For ArduinoJSON
inline bool convertToJson(const String& t, ArduinoJson::JsonVariant variant) {
    return variant.set(t.c_str());
}