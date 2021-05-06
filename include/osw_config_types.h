#ifndef OSW_CONFIG_TYPES_H
#define OSW_CONFIG_TYPES_H

#include <WString.h>
#include <gfx_util.h>

#include "osw_config.h"

//Forward delcarations: All OswConfigKey types
class OswConfigKeyString;
class OswConfigKeyInt;
class OswConfigKeyShort;
class OswConfigKeyRGB;

//All externally accessible keys are listed here
namespace OswConfigAllKeys {
extern OswConfigKeyString wifiSsid;
extern OswConfigKeyString wifiPass;
extern OswConfigKeyRGB appWTFprimaryColor;
}

/**
 * Config key interface - this enforces the key must
 * be serializeale from and to Strings (used for the data.json)
 * for the configuration UI and also holds all accessible
 * metadata for displaying it.
 */
class OswConfigKey {
  public:
  OswConfigKey(const char& cType, const char* id, const char* section, const char* label, const char* help):
  id(id), section(section), label(label), help(help), type(cType) {}
    virtual const String toString() const = 0;
    virtual void fromString(const char* from) const = 0;
    const char* id;
    const char* section;
    const char* label;
    const char* help;
    const char type;
};

//This holds a refrence to all compiles config keys, so we can e.g. iterate over them
extern OswConfigKey* OswConfigKeys[];

/**
 * This is a typed config key, this enforces any implementing key
 * must have a default and respective getter and setters.
 */
template<typename T>
class OswConfigKeyTyped : public OswConfigKey {
  public:
    OswConfigKeyTyped(const char configUiType, const char* id, const char* section, const char* label, const T def)
        : OswConfigKey(configUiType, id, section, label, help), def(def) {
        //Nothing in here
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
    OswConfigKeyString(const char* id, const char* section, const char* label, const String& def) :
        OswConfigKeyTyped('S', id, section, label, String(def)) {

    }
    const String get() const {
        return OswConfig::getInstance()->getString(this->id, this->def);
    }
    void set(const String& var) const {
        OswConfig::getInstance()->putString(this->id, var);
    }
    const String toString() const {
      return this->get();
    }
    void fromString(const char* from) const {
      this->set(String(from));
    }
};

/**
 * A typed config key implementation for loading & storing ints
 */
class OswConfigKeyInt : public OswConfigKeyTyped<int> {
  public:
    OswConfigKeyInt(const char* id, const char* section, const char* label, const int& def) :
        OswConfigKeyTyped('i', id, section, label, def) {

    }
    const int get() const {
        return OswConfig::getInstance()->getInt(this->id, this->def);
    }
    void set(const int& var) const {
        OswConfig::getInstance()->putInt(this->id, var);
    }
    const String toString() const {
      return String(this->get());
    }
    void fromString(const char* from) const {
      this->set(String(from).toInt());
    }
};

/**
 * A typed config key implementation for loading & storing shorts
 */
class OswConfigKeyShort : public OswConfigKeyTyped<short> {
  public:
    OswConfigKeyShort(const char* id, const char* section, const char* label, const short& def) :
        OswConfigKeyTyped('s', id, section, label, def) {}
    const short get() const {
        return OswConfig::getInstance()->getInt(this->id, this->def);
    }
    void set(const short& var) const {
        OswConfig::getInstance()->putInt(this->id, var);
    }
    const String toString() const {
      return String(this->get());
    }
    void fromString(const char* from) const {
      this->set(String(from).toInt());
    }
};

/**
 * A typed config key implementation for loading & storing rgb565 values
 */
class OswConfigKeyRGB : public OswConfigKeyTyped<uint16_t> {
  public:
    OswConfigKeyRGB(const char* id, const char* section, const char* label, const uint16_t& def) :
        OswConfigKeyTyped('h', id, section, label, def) {}
    const uint16_t get() const {
      //TODO
        return rgb565(20, 20, 20);
    }
    void set(const uint16_t& var) const {
      //TODO
    }
    const String toString() const {
      return String("TODO");
    }
    void fromString(const char* from) const {
      //TODO
    }
};

#endif