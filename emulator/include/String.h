#pragma once

#include <string>
#include <ArduinoJson.hpp>

#include "Defines.h"

class StringSumHelper;

class String : public std::string {
  public:
    String() : std::string() {};
    String(const char* str) : std::string(str) {};

// Whenever gcc supports std::format, we should update these defines accordingly!
#define _FAKE_STR_CONSTR(T) String(T smth) : std::string(std::to_string(smth)) { };
#define _FAKE_STR_CONSTR_STRIPZEROS(T) String(T smth) : std::string(std::to_string(smth)) { this->stripZeros(); };

    _FAKE_STR_CONSTR_STRIPZEROS(float)
    _FAKE_STR_CONSTR_STRIPZEROS(double)
    _FAKE_STR_CONSTR(bool)
    _FAKE_STR_CONSTR(char)
    _FAKE_STR_CONSTR(unsigned char)
    _FAKE_STR_CONSTR(short)
    _FAKE_STR_CONSTR(unsigned short)
    _FAKE_STR_CONSTR(int)
    _FAKE_STR_CONSTR(unsigned int)
    _FAKE_STR_CONSTR(long)
    _FAKE_STR_CONSTR(unsigned long)

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
        if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value) {
            res.append(std::to_string(smth));
            res.stripZeros();
        } else if constexpr (std::is_same<T, short>::value || std::is_same<T, int>::value || std::is_same<T, long>::value)
            res.append(std::to_string(smth));
        else
            res.append(smth);
        return res;
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
    void stripZeros() {
        this->erase(this->find_last_not_of('0') + 1, std::string::npos);
        this->erase(this->find_last_not_of('.') + 1, std::string::npos);
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