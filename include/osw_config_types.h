#ifndef OSW_CONFIG_TYPES_H
#define OSW_CONFIG_TYPES_H

#include <WString.h>
#include <gfx_util.h>

#include "osw_config.h"

// Forward delcarations: All OswConfigKey types
class OswConfigKeyString;
class OswConfigKeyInt;
class OswConfigKeyShort;
class OswConfigKeyRGB;

// All externally accessible keys are listed here
namespace OswConfigAllKeys {
extern OswConfigKeyString wifiSsid;
extern OswConfigKeyString wifiPass;
extern OswConfigKeyRGB appWTFprimaryColor;
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

// This holds a refrence to all compiles config keys, so we can e.g. iterate over them
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
 * A typed config key implementation for loading & storing strings
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
 * A typed config key implementation for loading & storing ints
 */
class OswConfigKeyInt : public OswConfigKeyTyped<int> {
 public:
  OswConfigKeyInt(const char* id, const char* section, const char* label, const char* help, const int& def)
      : OswConfigKeyTyped("i", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  const int get() const { return OswConfig::getInstance()->getInt(this->id, this->def); }
  void set(const int& var) const { OswConfig::getInstance()->putInt(this->id, var); }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) const { this->set(String(from).toInt()); }
};

/**
 * A typed config key implementation for loading & storing shorts
 */
class OswConfigKeyShort : public OswConfigKeyTyped<short> {
 public:
  OswConfigKeyShort(const char* id, const char* section, const char* label, const char* help, const short& def)
      : OswConfigKeyTyped("s", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  const short get() const { return OswConfig::getInstance()->getInt(this->id, this->def); }
  void set(const short& var) const { OswConfig::getInstance()->putInt(this->id, var); }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) const { this->set(String(from).toInt()); }
};

/**
 * A typed config key implementation for loading & storing rgb565 values
 */
class OswConfigKeyRGB : public OswConfigKeyTyped<uint16_t> {
 public:
  OswConfigKeyRGB(const char* id, const char* section, const char* label, const char* help, const uint16_t& def)
      : OswConfigKeyTyped("H", id, section, label, help, def) {}
  const String toDefaultString() const { return String(this->def); }
  const uint16_t get() const { return OswConfig::getInstance()->getUShort(this->id, def); }
  void set(const uint16_t& var) const { OswConfig::getInstance()->putUShort(this->id, var); }
  const String toString() const { return String(this->get()); }
  void fromString(const char* from) const { this->set(String(from).toInt()); }

  // Shamelessly copied from
  // https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
  void hsvToRgb(const unsigned char& h, const unsigned char& s, const unsigned char& v, unsigned char& r,
                unsigned char& g, unsigned char& b) const {
    unsigned char region, remainder, p, q, t;

    if (s == 0) {
      r = v;
      g = v;
      b = v;
      return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
      case 0:
        r = v;
        g = t;
        b = p;
        break;
      case 1:
        r = q;
        g = v;
        b = p;
        break;
      case 2:
        r = p;
        g = v;
        b = t;
        break;
      case 3:
        r = p;
        g = q;
        b = v;
        break;
      case 4:
        r = t;
        g = p;
        b = v;
        break;
      default:
        r = v;
        g = p;
        b = q;
        break;
    }
  }

  // Also shamelessly copied from
  // https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
  void rgbToHsv(const unsigned char& r, const unsigned char& g, const unsigned char& b, unsigned char& h,
                unsigned char& s, unsigned char& v) const {
    unsigned char rgbMin, rgbMax;

    rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

    v = rgbMax;
    if (v == 0) {
      h = 0;
      s = 0;
      return;
    }

    s = 255 * long(rgbMax - rgbMin) / v;
    if (s == 0) {
      h = 0;
      return;
    }

    if (rgbMax == r)
      h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
    else if (rgbMax == g)
      h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
    else
      h = 171 + 43 * (r - g) / (rgbMax - rgbMin);
  }
};

#endif