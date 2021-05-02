#ifndef OSW_CONFIG_H
#define OSW_CONFIG_H

#include <Preferences.h>

#include "config.h"

/**
 * Config keys - they consist of:
 * * id: an as-short-as-possible string, commonly only one char
 * * type: the type for the web ui (when unused: nullptr), otherwise choose from:
 *   * I = int
 *   * s = short
 *   * S = String
 * * default value: what we should return if we've got no value stored (yet)
 *
 * If you've got any need to store information persistently (e.g. from an app), you are
 * welcome to append your own line below!
 */
#define OSW_CONFIG_WTF_COLOR_R "a", "s", 179
#define OSW_CONFIG_WTF_COLOR_G "b", "s", 107
#define OSW_CONFIG_WTF_COLOR_B "c", "s", 0
#define OSW_CONFIG_WIFI_SSID "d", "S", WIFI_SSID
#define OSW_CONFIG_WIFI_PASS "e", "S", WIFI_PASS

//These are function defines, so we can reduce the copy-pasta for all the different types in the class below
#define _OSW_CONFIG_GETTER(T, F) inline T F(const char* id, const char* type, T def) {return this->prefs.F(id, def);}
#define _OSW_CONFIG_SETTER(T, F) inline void F(const char* id, const char* type, T def, T value) {if(this->readOnly) return; this->prefs.F(id, value);}
#define _OSW_CONFIG_SET_GET(T, FG, FS) _OSW_CONFIG_GETTER(T, FG) _OSW_CONFIG_SETTER(T, FS)

/**
 * This is basically a wrapper for the ESP32 lib Preferences.h -
 * it extends it by the functionality to ensure the nvs is using
 * the correct version (see class const); if not the config will
 * be reset on loading.
 * Please see the private variables for any reserved keys. DO NOT
 * TRY TO WRITE THEM.
 */
class OswConfig {
  public:
    const char* configNamespace = "ows-config";
    const char* configVersionKey = "ver"; //RESERVED KEY NAME - also do not use "v", as it is maybe already used.
    const short configVersionValue = 0; //Change this when you want to clean the config on next boot
    const char* configBootCntKey = "bct"; //RESERVED KEY NAME

    static OswConfig* getInstance();
    void setup();
    void reset();
    void enableWrite();
    void disableWrite();
    int getBootCount();

    _OSW_CONFIG_SET_GET(int8_t, getChar, putChar)
    _OSW_CONFIG_SET_GET(uint8_t, getUChar, putUChar)
    _OSW_CONFIG_SET_GET(int16_t, getShort, putShort)
    _OSW_CONFIG_SET_GET(uint16_t, getUShort, putUShort)
    _OSW_CONFIG_SET_GET(int32_t, getInt, putInt)
    _OSW_CONFIG_SET_GET(uint32_t, getUInt, putUInt)
    _OSW_CONFIG_SET_GET(int32_t, getLong, putLong)
    _OSW_CONFIG_SET_GET(uint32_t, getULong, putULong)
    _OSW_CONFIG_SET_GET(int64_t, getLong64, putLong64)
    _OSW_CONFIG_SET_GET(uint64_t, getULong64, putULong64)
    _OSW_CONFIG_SET_GET(float_t, getFloat, putFloat)
    _OSW_CONFIG_SET_GET(double_t, getDouble, putDouble)
    _OSW_CONFIG_SET_GET(bool, getBool, putBool)
    _OSW_CONFIG_SETTER(const char*, putString)
    _OSW_CONFIG_SET_GET(String, getString, putString)

    //NOTE: Bytes support is not implemented.
    //NOTE: const char* return for stings is not implemented due the high risk of someone creating memory leaks.
    // -> Just store the string, use it as needed & then let the string handle the cleanup.
  private:
    static OswConfig instance;
    bool readOnly = true;
    Preferences prefs;

    OswConfig();
    ~OswConfig();
};

#endif