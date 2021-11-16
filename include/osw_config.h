#ifndef OSW_CONFIG_H
#define OSW_CONFIG_H

#include <Preferences.h>

#include "config_defaults.h"

// These are function defines, so we can reduce the copy-pasta for all the different types in the class OswConfig
#define _OSW_CONFIG_GETTER(T, F)               \
  inline T F(const char* id, T def) {          \
    /* Yes, we are logging any access here -> this should help reducing some devs requesting the key every loop() call! */ \
    Serial.print(String(__FILE__) + ": Accessing key id "); \
    Serial.println(id);                        \
    return this->prefs.F(id, def);             \
  }
#define _OSW_CONFIG_SETTER(T, F)           \
  inline void F(const char* id, T value) { \
    if (this->readOnly) return;            \
    this->prefs.F(id, value);              \
  }
#define _OSW_CONFIG_SET_GET(T, FG, FS) _OSW_CONFIG_GETTER(T, FG) _OSW_CONFIG_SETTER(T, FS)

/**
 * This is basically a wrapper for the ESP32 lib Preferences.h -
 * it extends it by the functionality to ensure the nvs is using
 * the correct version (see class const); if not the config will
 * be reset on loading.
 * Please see the private variables for any reserved key names.
 * DO NOT TRY TO (OVER)WRITE THEM.
 */
class OswConfig {
 public:
  const char* configNamespace = "osw-config";
  const char* configVersionKey = "ver";  // RESERVED KEY NAME - also do not use "v", as it is maybe already used.
  const short configVersionValue = 2;    // Change this when you want to clean the config on next boot
  const char* configBootCntKey = "bct";  // RESERVED KEY NAME

  static OswConfig* getInstance();
  void setup();
  void reset();
  void enableWrite();
  void disableWrite();
  int getBootCount();
  String getConfigJSON();
  void parseDataJSON(const char* json);

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
  _OSW_CONFIG_SET_GET(String, getString, putString)
  _OSW_CONFIG_SETTER(const char*, putString)

  // NOTE: Bytes support is not implemented.
  // NOTE: const char* return for Strings is not implemented due the high risk of someone creating memory leaks.
  // -> Just store the String, use it as needed & then let the String handle the cleanup on its destruction.
 private:
  static OswConfig instance;
  bool readOnly = true;
  Preferences prefs;

  OswConfig();
  ~OswConfig();
};

#endif
