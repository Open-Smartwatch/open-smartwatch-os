#ifndef OSW_CONFIG_TYPES_H
#define OSW_CONFIG_TYPES_H

#include <WString.h>
#include <gfx_util.h>

#include "osw_config.h"

// Forward delcarations: All OswConfigKey types
class OswConfigKeyString;
class OswConfigKeyPassword;
class OswConfigKeyUnsignedLong;
class OswConfigKeyInt;
class OswConfigKeyShort;
class OswConfigKeyBool;
class OswConfigKeyDouble;
class OswConfigKeyFloat;
class OswConfigKeyRGB;

// All externally accessible keys are listed here (add them to osw_config_keys.cpp oswConfigKeys for config ui)
namespace OswConfigAllKeys {
extern OswConfigKeyString wifiSsid;
extern OswConfigKeyPassword wifiPass;
extern OswConfigKeyRGB themeBackgroundColor;
extern OswConfigKeyRGB themeBackgroundDimmedColor;
extern OswConfigKeyRGB themeForegroundColor;
extern OswConfigKeyRGB themeForegroundDimmedColor;
extern OswConfigKeyRGB themePrimaryColor;
extern OswConfigKeyRGB themeInfoColor;
extern OswConfigKeyRGB themeSuccessColor;
extern OswConfigKeyRGB themeWarningColor;
extern OswConfigKeyRGB themeDangerColor;
extern OswConfigKeyShort settingDisplayTimeout;
extern OswConfigKeyShort settingDisplayBrightness;
extern OswConfigKeyString dateFormat;
extern OswConfigKeyFloat daylightOffset;
extern OswConfigKeyBool timeFormat;
extern OswConfigKeyShort timeZone;
}  // namespace OswConfigAllKeys

/**
 * Config key interface - this enforces the key must
 * be serializeale from and to Strings (used for the data.json)
 * for the configuration UI and also holds all accessible
 * metadata for displaying it.
 */
class OswConfigKey {
 public:
  OswConfigKey(const char* cType, const char* id, const char* section, const char* label, const char* help)
      : id(id), section(section), label(label), help(help), type(cType) {}
  virtual const String toString() const = 0;
  virtual const String toDefaultString() const = 0;
  virtual void fromString(const char* from) const = 0;
  const char* id;
  const char* section;
  const char* label;
  const char* help;
  const char* type;
};

// This holds a refrence to all compiled config keys, so we can e.g. iterate over them
extern const unsigned char oswConfigKeysCount;
extern OswConfigKey* oswConfigKeys[];

/**
 * This is a typed config key, this enforces any implementing key
 * must have a default and respective getter and setters.
 */
template <typename T>
class OswConfigKeyTyped : public OswConfigKey {
 public:
  OswConfigKeyTyped(const char* configUiType, const char* id, const char* section, const char* label, const char* help,
                    const T def)
      : OswConfigKey(configUiType, id, section, label, help), def(def) {
    // Nothing in here
  }
  const T def;
  virtual const T get() const = 0;
  virtual void set(const T&) const = 0;
};

/**
 * A typed config key implementation for loading & storing strings -> string
 */
class OswConfigKeyString : public OswConfigKeyTyped<String> {
 public:
  OswConfigKeyString(const char* id, const char* section, const char* label, const char* help, const String& def)
      : OswConfigKeyTyped("S", id, section, label, help, String(def)) {}
  const String toDefaultString() const { return this->def; }
  const String get() const { return OswConfig::getInstance()->getString(this->id, this->def); }
  void set(const String& var) const { OswConfig::getInstance()->putString(this->id, var); }
  const String toString() const { return this->get(); }
  void fromString(const char* from) const { this->set(String(from)); }
};

/**
 * A typed config key implementation for loading & storing strings as passwords -> input:password
 */
class OswConfigKeyPassword : public OswConfigKeyTyped<String> {
 public:
  OswConfigKeyPassword(const char* id, const char* section, const char* label, const char* help, const String& def)
      : OswConfigKeyTyped("P", id, section, label, help, String(def)) {}
  const String toDefaultString() const { return this->def; }
  const String get() const { return OswConfig::getInstance()->getString(this->id, this->def); }
  void set(const String& var) const { OswConfig::getInstance()->putString(this->id, var); }
  const String toString() const { return this->get(); }
  void fromString(const char* from) const { this->set(String(from)); }
};

/**
 * A typed config key implementation for loading & storing unsigned longs -> input:number
 */
class OswConfigKeyUnsignedLong : public OswConfigKeyTyped<unsigned long> {
 public:
  OswConfigKeyUnsignedLong(const char* id, const char* section, const char* label, const char* help,
                           const unsigned long& def)
      : OswConfigKeyTyped("L", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  const unsigned long get() const { return OswConfig::getInstance()->getULong(this->id, this->def); }
  void set(const int& var) const { OswConfig::getInstance()->putULong(this->id, var); }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) const { this->set(String(from).toInt()); }
};

/**
 * A typed config key implementation for loading & storing ints -> input:number
 */
class OswConfigKeyInt : public OswConfigKeyTyped<int> {
 public:
  OswConfigKeyInt(const char* id, const char* section, const char* label, const char* help, const int& def)
      : OswConfigKeyTyped("I", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  const int get() const { return OswConfig::getInstance()->getInt(this->id, this->def); }
  void set(const int& var) const { OswConfig::getInstance()->putInt(this->id, var); }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) const { this->set(String(from).toInt()); }
};

/**
 * A typed config key implementation for loading & storing shorts -> input:number
 */
class OswConfigKeyShort : public OswConfigKeyTyped<short> {
 public:
  OswConfigKeyShort(const char* id, const char* section, const char* label, const char* help, const short& def)
      : OswConfigKeyTyped("i", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  const short get() const { return OswConfig::getInstance()->getShort(this->id, this->def); }
  void set(const short& var) const { OswConfig::getInstance()->putShort(this->id, var); }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) const { this->set(String(from).toInt()); }
};

/**
 * A typed config key implementation for loading & storing rgb888 values -> HTML5 ColorPicker
 */
class OswConfigKeyRGB : public OswConfigKeyTyped<uint32_t> {
 public:
  OswConfigKeyRGB(const char* id, const char* section, const char* label, const char* help, const uint32_t& def)
      : OswConfigKeyTyped("R", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  const uint32_t get() const { return OswConfig::getInstance()->getUInt(this->id, def); }
  void set(const uint32_t& var) const { OswConfig::getInstance()->putUInt(this->id, var); }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) const { this->set(String(from).toInt()); }
};

/**
 * A typed config key implementation for loading & storing bools -> input:checkbox
 */
class OswConfigKeyBool : public OswConfigKeyTyped<bool> {
 public:
  OswConfigKeyBool(const char* id, const char* section, const char* label, const char* help, const bool& def)
      : OswConfigKeyTyped("C", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  const bool get() const { return OswConfig::getInstance()->getBool(this->id, this->def); }
  void set(const bool& var) const { OswConfig::getInstance()->putBool(this->id, var); }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) const { this->set(String(from).toInt()); }
};

/**
 * A typed config key implementation for loading & storing doubles -> input:number
 */
class OswConfigKeyDouble : public OswConfigKeyTyped<double> {
 public:
  OswConfigKeyDouble(const char* id, const char* section, const char* label, const char* help, const double& def)
      : OswConfigKeyTyped("D", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  const double get() const { return OswConfig::getInstance()->getDouble(this->id, this->def); }
  void set(const double& var) const { OswConfig::getInstance()->putDouble(this->id, var); }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) const { this->set(String(from).toDouble()); }
};

/**
 * A typed config key implementation for loading & storing floats -> input:number
 */
class OswConfigKeyFloat : public OswConfigKeyTyped<float> {
 public:
  OswConfigKeyFloat(const char* id, const char* section, const char* label, const char* help, const float& def)
      : OswConfigKeyTyped("F", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  const float get() const { return OswConfig::getInstance()->getFloat(this->id, this->def); }
  void set(const float& var) const { OswConfig::getInstance()->putFloat(this->id, var); }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) const { this->set(String(from).toFloat()); }
};

#endif
