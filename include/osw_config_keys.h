#ifndef OSW_CONFIG_TYPES_H
#define OSW_CONFIG_TYPES_H

#include <vector>
#include <WString.h>
#include <gfx_util.h>

#include <stdexcept>
#include OSW_TARGET_PLATFORM_HEADER

#include "osw_config.h"

// Forward declarations: All OswConfigKey types
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
extern OswConfigKeyBool hostPasswordEnabled;
extern OswConfigKeyPassword hostPass;
#ifdef OSW_FEATURE_WIFI_ONBOOT
extern OswConfigKeyBool wifiBootEnabled;
#endif
extern OswConfigKeyBool wifiAlwaysNTPEnabled;
extern OswConfigKeyBool wifiAutoAP;
extern OswConfigKeyString wifiSsid;
extern OswConfigKeyPassword wifiPass;
extern OswConfigKeyString fallbackWifiSsid1st;
extern OswConfigKeyPassword fallbackWifiPass1st;
extern OswConfigKeyString fallbackWifiSsid2nd;
extern OswConfigKeyPassword fallbackWifiPass2nd;
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
extern OswConfigKeyShort oswAppV2ButtonDoublePress;
extern OswConfigKeyShort oswAppV2ButtonLongPress;
extern OswConfigKeyShort oswAppV2ButtonVeryLongPress;
extern OswConfigKeyShort settingDisplayTimeout;
extern OswConfigKeyShort settingDisplayBrightness;
extern OswConfigKeyBool settingDisplayOverlays;
extern OswConfigKeyBool settingDisplayOverlaysForced;
extern OswConfigKeyBool settingDisplayDualHourTick;
#if OSW_PLATFORM_BLOCK_SLEEP != 1
extern OswConfigKeyBool raiseToWakeEnabled;
extern OswConfigKeyShort raiseToWakeSensitivity;
extern OswConfigKeyBool tapToWakeEnabled;
extern OswConfigKeyBool lightSleepEnabled;
extern OswConfigKeyBool buttonToWakeEnabled;
#endif
extern OswConfigKeyDropDown dateFormat;
extern OswConfigKeyBool timeFormat;
extern OswConfigKeyString timezonePrimary;
extern OswConfigKeyString timezoneSecondary;
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
extern OswConfigKeyShort configHeight;
extern OswConfigKeyShort configWeight;
extern OswConfigKeyInt stepsPerDay;
extern OswConfigKeyInt distPerDay;
extern OswConfigKeyInt kcalPerDay;
extern OswConfigKeyBool stepsHistoryClear;
extern OswConfigKeyBool settingDisplayStepsGoal;
extern OswConfigKeyShort resetDay;
#endif
extern OswConfigKeyDropDown settingDisplayDefaultWatchface;
#ifdef OSW_FEATURE_WEATHER
extern OswConfigKeyString weatherApiKey;
extern OswConfigKeyString weatherLocation1;
extern OswConfigKeyString weatherState1;
#endif
}  // namespace OswConfigAllKeys

/**
 * @brief This enum holds all known type-ids of the web interface
 *
 */
enum class OswConfigKeyTypedUIType: char {
    STRING = 'S',
    PASSWORD = 'P',
    DROPDOWN = 'd',
    ULONG = 'L',
    INT = 'I',
    SHORT = 'i',
    RGB = 'R',
    BOOL = 'C',
    DOUBLE = 'D',
    FLOAT = 'F'
};

/**
 * Config key interface - this enforces the key must
 * be serializeale from and to Strings (used for the data.json)
 * for the configuration UI and also holds all accessible
 * metadata for displaying it.
 */
class OswConfigKey {
  public:
    OswConfigKey(const OswConfigKeyTypedUIType& cType, const char* id, const char* section, const char* label, const char* help)
        : id(id), section(section), label(label), help(help), type(cType) {}
    virtual const String toString() const = 0;
    virtual const String toDefaultString() const = 0;
    virtual void fromString(const char* from) = 0;
    const char* id;
    const char* section;
    const char* label;
    const char* help;
    const OswConfigKeyTypedUIType type;

  protected:
    virtual void loadValueFromNVS() = 0;
    friend OswConfig;
};

// This holds a reference to all compiled config keys, so we can e.g. iterate over them
extern const unsigned char oswConfigKeysCount;
extern OswConfigKey* oswConfigKeys[];

/**
 * This is a typed config key, this enforces any implementing key
 * must have a default and respective getter and setters.
 */
template <typename T>
class OswConfigKeyTyped : public OswConfigKey {
  public:
    OswConfigKeyTyped(const OswConfigKeyTypedUIType& configUiType, const char* id, const char* section, const char* label, const char* help,
                      const T def)
        : OswConfigKey(configUiType, id, section, label, help), def(def) {
        // Nothing in here
    }
    const T def;
    T val;  // This is a cached value to reduce the reading of the nvs during e.g. rendering
    virtual const T get() const {
        return this->val;
    };
    virtual void set(const T& var) {
        this->val = var;
    };
};

/**
 * A typed config key implementation for loading & storing strings -> string
 * This key type is NOT cached. Do NOT call this periodically!
 */
class OswConfigKeyString : public OswConfigKeyTyped<String> {
  public:
    OswConfigKeyString(const char* id, const char* section, const char* label, const char* help, const String& def)
        : OswConfigKeyTyped(OswConfigKeyTypedUIType::STRING, id, section, label, help, String(def)) {}
    const String toDefaultString() const {
        return this->def;
    }
    const String get() const {
        OSW_LOG_D("Loading string from nvs: ", this->id); // Log to annoy devs to use some caching
        return OswConfig::getInstance()->prefs.getString(this->id, this->def);
    };
    void set(const String& var) {
        if(OswConfig::getInstance()->readOnly) return;
        OswConfig::getInstance()->prefs.putString(this->id, var);
    }
    const String toString() const {
        return this->get();
    }
    void fromString(const char* from) {
        this->set(String(from));
    }
    void loadValueFromNVS() {/* Ignored */};
};

/**
 * A typed config key implementation for loading & storing strings as passwords -> input:password
 * This key type is NOT cached. Do NOT call this periodically!
 */
class OswConfigKeyPassword : public OswConfigKeyTyped<String> {
  public:
    OswConfigKeyPassword(const char* id, const char* section, const char* label, const char* help, const String& def)
        : OswConfigKeyTyped(OswConfigKeyTypedUIType::PASSWORD, id, section, label, help, String(def)) {}
    const String toDefaultString() const {
        return this->def;
    }
    const String get() const {
        OSW_LOG_D("Loading password from nvs: ", this->id); // Log to annoy devs to use some caching
        return OswConfig::getInstance()->prefs.getString(this->id, this->def);
    };
    void set(const String& var) {
        if(OswConfig::getInstance()->readOnly) return;
        OswConfig::getInstance()->prefs.putString(this->id, var);
    }
    const String toString() const {
        return this->get();
    }
    void fromString(const char* from) {
        this->set(String(from));
    }
    void loadValueFromNVS() {/* Ignored */};
};

/**
 * A typed config key implementation for loading & storing strings as a drop down list -> string
 * This key type is NOT cached. Do NOT call this periodically!
 */
class OswConfigKeyDropDown : public OswConfigKeyTyped<String> {
  public:
    OswConfigKeyDropDown(const char* id, const char* section, const char* label, std::vector<const char*> options, size_t defaultOptionIndex) :
        OswConfigKeyTyped(OswConfigKeyTypedUIType::DROPDOWN, id, section, label, nullptr, String(options.at(defaultOptionIndex))) {
        this->setOptions(options);
    }
    OswConfigKeyDropDown(const char* id, const char* section, const char* label, std::vector<const char*> options, const String& def) :
        OswConfigKeyDropDown(id, section, label, options, this->getOptionIndex(options, def)) {}

    const String toDefaultString() const {
        return this->def;
    }
    const String get() const {
        OSW_LOG_D("Loading drop-down from nvs: ", this->id); // Log to annoy devs to use some caching
        return OswConfig::getInstance()->prefs.getString(this->id, this->def);
    }
    void set(const String& var) {
        this->checkValidOption(var);
        if(OswConfig::getInstance()->readOnly) return;
        OswConfig::getInstance()->prefs.putString(this->id, var);
    }
    const String toString() const {
        return this->get();
    }
    void fromString(const char* from) {
        this->set(String(from));
    }
    void loadValueFromNVS() {/* Ignored */};

    std::vector<const char*> getOptions() const {
        return this->options;
    }
  private:
    std::vector<const char*> options;
    std::string optionsStr; // Used as cache reference for the c_str() call by setOptions

    static size_t getOptionIndex(const std::vector<const char*>& options, const String& option) {
        for (size_t i = 0; i < options.size(); i++) {
            if (option == options.at(i))
                return i;
        }
        throw std::invalid_argument("Option not found in options list");
    }

    void setOptions(const std::vector<const char*>& options) {
        this->options = options;
        this->optionsStr.clear();
        for(auto it = this->options.begin(); it != this->options.end(); ++it) {
            this->optionsStr.append(*it);
            if(std::next(it) != this->options.end())
                this->optionsStr.append(",");
        }
        this->help = this->optionsStr.c_str();
    }

    void checkValidOption(const String& val) {
        // Check if var is in the list of options
        for(auto it = this->options.begin(); it != this->options.end(); ++it) {
            if (val == *it)
                return;
        }
        // If we reach this line, the value was not valid!
        throw std::invalid_argument("Invalid option value for dropdown key!");
    }
};

/**
 * A typed config key implementation for loading & storing unsigned longs -> input:number
 */
class OswConfigKeyUnsignedLong : public OswConfigKeyTyped<unsigned long> {
  public:
    OswConfigKeyUnsignedLong(const char* id, const char* section, const char* label, const char* help,
                             const unsigned long& def)
        : OswConfigKeyTyped(OswConfigKeyTypedUIType::ULONG, id, section, label, help, def) {}
    const String toDefaultString() const {
        return String(this->def);
    }
    void set(const unsigned long& var) {
        if(OswConfig::getInstance()->readOnly or this->val == var) return;
        OswConfigKeyTyped::set(var);
        OswConfig::getInstance()->prefs.putULong(this->id, var);
    }
    const String toString() const {
        return String(this->get());
    }
    void fromString(const char* from) {
        this->set(String(from).toInt());
    }
    void loadValueFromNVS() {
        this->val = OswConfig::getInstance()->prefs.getULong(this->id, this->def);
    };
};

/**
 * A typed config key implementation for loading & storing ints -> input:number
 */
class OswConfigKeyInt : public OswConfigKeyTyped<int> {
  public:
    OswConfigKeyInt(const char* id, const char* section, const char* label, const char* help, const int& def)
        : OswConfigKeyTyped(OswConfigKeyTypedUIType::INT, id, section, label, help, def) {}
    const String toDefaultString() const {
        return String(this->def);
    }
    void set(const int& var) {
        if(OswConfig::getInstance()->readOnly or this->val == var) return;
        OswConfigKeyTyped::set(var);
        OswConfig::getInstance()->prefs.putInt(this->id, var);
    }
    const String toString() const {
        return String(this->get());
    }
    void fromString(const char* from) {
        this->set(String(from).toInt());
    }
    void loadValueFromNVS() {
        this->val = OswConfig::getInstance()->prefs.getInt(this->id, this->def);
    };
};

/**
 * A typed config key implementation for loading & storing shorts -> input:number
 */
class OswConfigKeyShort : public OswConfigKeyTyped<short> {
  public:
    OswConfigKeyShort(const char* id, const char* section, const char* label, const char* help, const short& def)
        : OswConfigKeyTyped(OswConfigKeyTypedUIType::SHORT, id, section, label, help, def) {}
    const String toDefaultString() const {
        return String(this->def);
    }
    void set(const short& var) {
        if(OswConfig::getInstance()->readOnly or this->val == var) return;
        OswConfigKeyTyped::set(var);
        OswConfig::getInstance()->prefs.putShort(this->id, var);
    }
    const String toString() const {
        return String(this->get());
    }
    void fromString(const char* from) {
        this->set(String(from).toInt());
    }
    void loadValueFromNVS() {
        this->val = OswConfig::getInstance()->prefs.getShort(this->id, this->def);
    };
};

/**
 * A typed config key implementation for loading & storing rgb888 values -> HTML5 ColorPicker
 */
class OswConfigKeyRGB : public OswConfigKeyTyped<uint32_t> {
  public:
    OswConfigKeyRGB(const char* id, const char* section, const char* label, const char* help, const uint32_t& def)
        : OswConfigKeyTyped(OswConfigKeyTypedUIType::RGB, id, section, label, help, def) {}
    const String toDefaultString() const {
        return String(this->def);
    }
    void set(const uint32_t& var) {
        if(OswConfig::getInstance()->readOnly or this->val == var) return;
        OswConfigKeyTyped::set(var);
        OswConfig::getInstance()->prefs.putUInt(this->id, var);
    }
    const String toString() const {
        return "#" + String(this->get(), HEX);
    }
    void fromString(const char* from) {
        if(strlen(from) > 2)
            from += 1; // skip the leading #
        this->set((uint32_t)(strtol(from, NULL, 16))); // parse the hex-string to a number
    }
    void loadValueFromNVS() {
        this->val = OswConfig::getInstance()->prefs.getUInt(this->id, this->def);
    };
};

/**
 * A typed config key implementation for loading & storing bools -> input:checkbox
 */
class OswConfigKeyBool : public OswConfigKeyTyped<bool> {
  public:
    OswConfigKeyBool(const char* id, const char* section, const char* label, const char* help, const bool& def)
        : OswConfigKeyTyped(OswConfigKeyTypedUIType::BOOL, id, section, label, help, def) {}
    const String toDefaultString() const {
        return String(this->def);
    }
    void set(const bool& var) {
        if(OswConfig::getInstance()->readOnly or this->val == var) return;
        OswConfigKeyTyped::set(var);
        OswConfig::getInstance()->prefs.putBool(this->id, var);
    }
    const String toString() const {
        return this->get() ? "true" : "false";
    }
    void fromString(const char* from) {
        this->set(String(from) == "true");
    }
    void loadValueFromNVS() {
        this->val = OswConfig::getInstance()->prefs.getBool(this->id, this->def);
    };
};

/**
 * A typed config key implementation for loading & storing doubles -> input:number
 */
class OswConfigKeyDouble : public OswConfigKeyTyped<double> {
  public:
    OswConfigKeyDouble(const char* id, const char* section, const char* label, const char* help, const double& def)
        : OswConfigKeyTyped(OswConfigKeyTypedUIType::DOUBLE, id, section, label, help, def) {}
    const String toDefaultString() const {
        return String(this->def);
    }
    void set(const double& var) {
        if(OswConfig::getInstance()->readOnly or this->val == var) return;
        OswConfigKeyTyped::set(var);
        OswConfig::getInstance()->prefs.putDouble(this->id, var);
    }
    const String toString() const {
        return String(this->get());
    }
    void fromString(const char* from) {
        this->set(String(from).toDouble());
    }
    void loadValueFromNVS() {
        this->val = OswConfig::getInstance()->prefs.getDouble(this->id, this->def);
    };
};

/**
 * A typed config key implementation for loading & storing floats -> input:number
 */
class OswConfigKeyFloat : public OswConfigKeyTyped<float> {
  public:
    OswConfigKeyFloat(const char* id, const char* section, const char* label, const char* help, const float& def)
        : OswConfigKeyTyped(OswConfigKeyTypedUIType::FLOAT, id, section, label, help, def) {}
    const String toDefaultString() const {
        return String(this->def);
    }
    void set(const float& var) {
        if(OswConfig::getInstance()->readOnly or this->val == var) return;
        OswConfigKeyTyped::set(var);
        OswConfig::getInstance()->prefs.putFloat(this->id, var);
    }
    const String toString() const {
        return String(this->get());
    }
    void fromString(const char* from) {
        this->set(String(from).toFloat());
    }
    void loadValueFromNVS() {
        this->val = OswConfig::getInstance()->prefs.getFloat(this->id, this->def);
    };
};

#endif
