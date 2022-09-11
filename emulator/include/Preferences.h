#pragma once

#include <filesystem>
#include <fstream>

#include "Jzon.h"

#include "DataTypes.h"
#include "String.h"
#include "Defines.h"

#include "nvs_flash.h"

class Preferences {
public:
    static const char* preferencesFolderName;

    Preferences() {};
    ~Preferences() {};

    bool begin(const char* name, bool);
    void end() {};

    #define _PUT_DATA(F, T) inline size_t F(const char* key, T value) { \
        if(this->isKey(key)) \
            this->node.remove(key); \
        this->node.add(key, value); \
        this->serialize(); \
        return sizeof(T); \
    };
    #define _PUT_DATA_TYPED(F, T1, T2) inline size_t F(const char* key, T1 value) { \
        if(this->isKey(key)) \
            this->node.remove(key); \
        this->node.add(key, (T2) value); \
        this->serialize(); \
        return sizeof(T1); \
    };

    _PUT_DATA(putChar, int8_t)
    _PUT_DATA(putUChar, uint8_t)
    _PUT_DATA(putShort, int16_t)
    _PUT_DATA(putUShort, uint16_t)
    _PUT_DATA(putInt, int32_t)
    _PUT_DATA(putUInt, uint32_t)
    _PUT_DATA(putLong, int32_t)
    _PUT_DATA(putULong, uint32_t)
    _PUT_DATA_TYPED(putLong64, int64_t, long long int)
    _PUT_DATA_TYPED(putULong64, uint64_t, unsigned long long int)
    _PUT_DATA(putFloat, float_t)
    _PUT_DATA(putDouble, double_t)
    _PUT_DATA(putBool, bool)
    _PUT_DATA(putString, const char*)
    _PUT_DATA(putString, String)

    #define _GET_DATA(F1, F2, T, D) inline T F1(const char* key, T defaultValue = D) { \
        if(this->isKey(key)) \
            return (T) this->node.get(key).F2(); \
        else \
            return defaultValue; \
    };

    _GET_DATA(getChar, toInt, int8_t, 0)
    _GET_DATA(getUChar, toInt, uint8_t, 0)
    _GET_DATA(getShort, toInt, int16_t, 0)
    _GET_DATA(getUShort, toInt, uint16_t, 0)
    _GET_DATA(getInt, toInt, int32_t, 0)
    _GET_DATA(getUInt, toInt, uint32_t, 0)
    _GET_DATA(getLong, toInt, int32_t, 0)
    _GET_DATA(getULong, toInt, uint32_t, 0)
    _GET_DATA(getLong64, toInt, int64_t, 0)
    _GET_DATA(getULong64, toInt, uint64_t, 0)
    _GET_DATA(getFloat, toFloat, float_t, -1.0f)
    _GET_DATA(getDouble, toDouble, double_t, -1.0f)
    _GET_DATA(getBool, toBool, double_t, false)
    _GET_DATA(getString, toString, String, String())

    inline size_t getString(const char* key, char* value, size_t maxLen) {
        String s = this->getString(key);
        strncpy(value, s.c_str(), std::min(maxLen, s.size()));
        return strlen(value);
    };

    inline bool isKey(const char* key) {
        return this->node.has(key);
    };

    size_t putBytes(const char* key, const void* value, size_t len);
    size_t getBytes(const char* key, void * buf, size_t maxLen);

    inline size_t getBytesLength(const char* key) {
        return std::filesystem::file_size(this->getBytesPath(key));
    };
private:
    std::string name;
    std::filesystem::path path;
    Jzon::Node node;

    inline std::filesystem::path getBytesPath(const char* name) {
        return std::filesystem::path(preferencesFolderName) / (this->name + "_" + name + ".bin");
    };

    void serialize();
};