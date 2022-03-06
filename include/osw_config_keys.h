#ifndef OSW_CONFIG_TYPES_H
#define OSW_CONFIG_TYPES_H

#include <WString.h>
#include <gfx_util.h>

#include OSW_TARGET_PLATFORM_HEADER

#include "osw_config.h"

// Forward delcarations: All OswConfigKey types
class OswConfigKeyString;
class OswConfigKeyPassword;
class OswConfigKeyDropDown;
class OswConfigKeyUnsignedLong;
class OswConfigKeyInt;
class OswConfigKeyShort;
class OswConfigKeyBool;
class OswConfigKeyDouble;
class OswConfigKeyFloat;
class OswConfigKeyRGB;

// All externally accessible keys are listed here (add them to osw_config_keys.cpp oswConfigKeys for config ui)
namespace OswConfigAllKeys {
#ifdef OSW_FEATURE_WIFI
extern OswConfigKeyString hostname;
#ifdef OSW_FEATURE_WIFI_ONBOOT
extern OswConfigKeyBool wifiBootEnabled;
#endif
extern OswConfigKeyBool wifiAlwaysNTPEnabled;
extern OswConfigKeyBool wifiAutoAP;
extern OswConfigKeyString wifiSsid;
extern OswConfigKeyPassword wifiPass;
#endif
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
extern OswConfigKeyBool settingDisplayOverlays;
extern OswConfigKeyBool settingDisplayOverlaysOnWatchScreen;
extern OswConfigKeyBool raiseToWakeEnabled;
extern OswConfigKeyShort raiseToWakeSensitivity;
extern OswConfigKeyBool tapToWakeEnabled;
extern OswConfigKeyBool lightSleepEnabled;
extern OswConfigKeyBool buttonToWakeEnabled;
extern OswConfigKeyDropDown dateFormat;
extern OswConfigKeyFloat daylightOffset;
extern OswConfigKeyBool timeFormat;
extern OswConfigKeyShort timeZone;
extern OswConfigKeyShort resetDay;
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
extern OswConfigKeyInt stepsPerDay;
extern OswConfigKeyBool stepsHistoryClear;
#endif
extern OswConfigKeyDropDown settingDisplayDefaultWatchface;
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
  virtual void fromString(const char* from) = 0;
  const char* id;
  const char* section;
  const char* label;
  const char* help;
  const char* type;

 protected:
  virtual void loadValueFromNVS() = 0;
  friend OswConfig;
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
  T val;  // This is a cached value to reduce the reading of the nvs during e.g. rendering
  virtual const T get() const { return this->val; };
  virtual void set(const T& var) { this->val = var; };
};

/**
 * A typed config key implementation for loading & storing strings -> string
 * This key type is NOT cached. Do NOT call this periodically!
 */
class OswConfigKeyString : public OswConfigKeyTyped<String> {
 public:
  OswConfigKeyString(const char* id, const char* section, const char* label, const char* help, const String& def)
      : OswConfigKeyTyped("S", id, section, label, help, String(def)) {}
  const String toDefaultString() const { return this->def; }
  const String get() const { return OswConfig::getInstance()->getString(this->id, this->def); };
  void set(const String& var) { OswConfig::getInstance()->putString(this->id, var); }
  const String toString() const { return this->get(); }
  void fromString(const char* from) { this->set(String(from)); }
  void loadValueFromNVS(){/* Ignored */};
};

/**
 * A typed config key implementation for loading & storing strings as passwords -> input:password
 * This key type is NOT cached. Do NOT call this periodically!
 */
class OswConfigKeyPassword : public OswConfigKeyTyped<String> {
 public:
  OswConfigKeyPassword(const char* id, const char* section, const char* label, const char* help, const String& def)
      : OswConfigKeyTyped("P", id, section, label, help, String(def)) {}
  const String toDefaultString() const { return this->def; }
  const String get() const { return OswConfig::getInstance()->getString(this->id, this->def); };
  void set(const String& var) { OswConfig::getInstance()->putString(this->id, var); }
  const String toString() const { return this->get(); }
  void fromString(const char* from) { this->set(String(from)); }
  void loadValueFromNVS(){/* Ignored */};
};

/**
 * A typed config key implementation for loading & storing strings as a drop down list -> string
 * This key type is NOT cached. Do NOT call this periodically!
 */
class OswConfigKeyDropDown : public OswConfigKeyTyped<String> {
 public:
  OswConfigKeyDropDown(const char* id, const char* section, const char* label, const char* help, const String& def)
      : OswConfigKeyTyped("d", id, section, label, help, String(def)) {}
  const String toDefaultString() const { return this->def; }
  const String get() const { return OswConfig::getInstance()->getString(this->id, this->def); };
  void set(const String& var) { OswConfig::getInstance()->putString(this->id, var); }
  const String toString() const { return this->get(); }
  void fromString(const char* from) { this->set(String(from)); }
  void loadValueFromNVS(){/* Ignored */};
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
  void set(const int& var) {
    OswConfigKeyTyped::set(var);
    OswConfig::getInstance()->putULong(this->id, var);
  }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) { this->set(String(from).toInt()); }
  void loadValueFromNVS() { this->val = OswConfig::getInstance()->getULong(this->id, this->def); };
};

/**
 * A typed config key implementation for loading & storing ints -> input:number
 */
class OswConfigKeyInt : public OswConfigKeyTyped<int> {
 public:
  OswConfigKeyInt(const char* id, const char* section, const char* label, const char* help, const int& def)
      : OswConfigKeyTyped("I", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  void set(const int& var) {
    OswConfigKeyTyped::set(var);
    OswConfig::getInstance()->putInt(this->id, var);
  }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) { this->set(String(from).toInt()); }
  void loadValueFromNVS() { this->val = OswConfig::getInstance()->getInt(this->id, this->def); };
};

/**
 * A typed config key implementation for loading & storing shorts -> input:number
 */
class OswConfigKeyShort : public OswConfigKeyTyped<short> {
 public:
  OswConfigKeyShort(const char* id, const char* section, const char* label, const char* help, const short& def)
      : OswConfigKeyTyped("i", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  void set(const short& var) {
    OswConfigKeyTyped::set(var);
    OswConfig::getInstance()->putShort(this->id, var);
  }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) { this->set(String(from).toInt()); }
  void loadValueFromNVS() { this->val = OswConfig::getInstance()->getShort(this->id, this->def); };
};

/**
 * A typed config key implementation for loading & storing rgb888 values -> HTML5 ColorPicker
 */
class OswConfigKeyRGB : public OswConfigKeyTyped<uint32_t> {
 public:
  OswConfigKeyRGB(const char* id, const char* section, const char* label, const char* help, const uint32_t& def)
      : OswConfigKeyTyped("R", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  void set(const uint32_t& var) {
    OswConfigKeyTyped::set(var);
    OswConfig::getInstance()->putUInt(this->id, var);
  }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) { this->set(String(from).toInt()); }
  void loadValueFromNVS() { this->val = OswConfig::getInstance()->getUInt(this->id, this->def); };
};

/**
 * A typed config key implementation for loading & storing bools -> input:checkbox
 */
class OswConfigKeyBool : public OswConfigKeyTyped<bool> {
 public:
  OswConfigKeyBool(const char* id, const char* section, const char* label, const char* help, const bool& def)
      : OswConfigKeyTyped("C", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  void set(const bool& var) {
    OswConfigKeyTyped::set(var);
    OswConfig::getInstance()->putBool(this->id, var);
  }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) { this->set(String(from).toInt()); }
  void loadValueFromNVS() { this->val = OswConfig::getInstance()->getBool(this->id, this->def); };
};

/**
 * A typed config key implementation for loading & storing doubles -> input:number
 */
class OswConfigKeyDouble : public OswConfigKeyTyped<double> {
 public:
  OswConfigKeyDouble(const char* id, const char* section, const char* label, const char* help, const double& def)
      : OswConfigKeyTyped("D", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  void set(const double& var) {
    OswConfigKeyTyped::set(var);
    OswConfig::getInstance()->putDouble(this->id, var);
  }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) { this->set(String(from).toDouble()); }
  void loadValueFromNVS() { this->val = OswConfig::getInstance()->getDouble(this->id, this->def); };
};

/**
 * A typed config key implementation for loading & storing floats -> input:number
 */
class OswConfigKeyFloat : public OswConfigKeyTyped<float> {
 public:
  OswConfigKeyFloat(const char* id, const char* section, const char* label, const char* help, const float& def)
      : OswConfigKeyTyped("F", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  void set(const float& var) {
    OswConfigKeyTyped::set(var);
    OswConfig::getInstance()->putFloat(this->id, var);
  }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) { this->set(String(from).toFloat()); }
  void loadValueFromNVS() { this->val = OswConfig::getInstance()->getFloat(this->id, this->def); };
};

#endif
