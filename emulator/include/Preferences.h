#pragma once

#include "DataTypes.h"
#include "String.h"
#include "Defines.h"

class Preferences {
public:
    Preferences() {};
    ~Preferences() {};

    bool begin(const char*, bool) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return false;
    }

    void end() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    }

    inline size_t putChar(const char* key, int8_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putUChar(const char* key, uint8_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putShort(const char* key, int16_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putUShort(const char* key, uint16_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putInt(const char* key, int32_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putUInt(const char* key, uint32_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putLong(const char* key, int32_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putULong(const char* key, uint32_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putLong64(const char* key, int64_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putULong64(const char* key, uint64_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putFloat(const char* key, float_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putDouble(const char* key, double_t value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putBool(const char* key, bool value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putString(const char* key, const char* value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putString(const char* key, String value) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t putBytes(const char* key, const void* value, size_t len) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };

    inline bool isKey(const char* key) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return false;
    };
    inline int8_t getChar(const char* key, int8_t defaultValue = 0) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline uint8_t getUChar(const char* key, uint8_t defaultValue = 0) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline int16_t getShort(const char* key, int16_t defaultValue = 0) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline uint16_t getUShort(const char* key, uint16_t defaultValue = 0) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline int32_t getInt(const char* key, int32_t defaultValue = 0) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline uint32_t getUInt(const char* key, uint32_t defaultValue = 0) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline int32_t getLong(const char* key, int32_t defaultValue = 0)  {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline uint32_t getULong(const char* key, uint32_t defaultValue = 0) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline int64_t getLong64(const char* key, int64_t defaultValue = 0) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline uint64_t getULong64(const char* key, uint64_t defaultValue = 0) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline float_t getFloat(const char* key, float_t defaultValue = -1.0f) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline double_t getDouble(const char* key, double_t defaultValue = -1.0f) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline bool getBool(const char* key, bool defaultValue = false) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return false;
    };
    inline size_t getString(const char* key, char* value, size_t maxLen) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline String getString(const char* key, String defaultValue = String()) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return "";
    };
    inline size_t getBytesLength(const char* key) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
    inline size_t getBytes(const char* key, void * buf, size_t maxLen) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    };
};