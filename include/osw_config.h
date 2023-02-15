#ifndef OSW_CONFIG_H
#define OSW_CONFIG_H

#ifdef OSW_EMULATOR
#include <FakeMe.h> // Only used for Serial.*
#endif
#include <Preferences.h>

#include <OswLogger.h>
#include "config_defaults.h"

/**
 * This is basically a wrapper for the ESP32 lib Preferences.h -
 * it extends it by the functionality to ensure the nvs is using
 * the correct version (see class const); if not the config will
 * be reset on loading.
 * Please see the private variables for any reserved key names.
 * DO NOT TRY TO (OVER)WRITE THEM.
 */

class OswConfigKeyString;
class OswConfigKeyPassword;
class OswConfigKeyDropDown;
class OswConfigKeyUnsignedLong;
class OswConfigKeyInt;
class OswConfigKeyShort;
class OswConfigKeyRGB;
class OswConfigKeyBool;
class OswConfigKeyDouble;
class OswConfigKeyFloat;

class OswConfig {
  public:
    const char* configNamespace = "osw-config";
    const char* configVersionKey = "ver";  // RESERVED KEY NAME - also do not use "v", as it is maybe already used.
    const short configVersionValue = 2;    // Change this when you want to clean the config on next boot
    const char* configBootCntKey = "bct";  // RESERVED KEY NAME

    static OswConfig* getInstance();
    static void resetInstance();

    void setup();
    void reset(bool clearWholeNVS);
    void enableWrite();
    void disableWrite();
    int getBootCount();
    String getCategoriesJson();
    String getFieldJson(String id);
    void setField(String id, String value);
    void notifyChange();
  protected:
    Preferences prefs; // for the config keys accessible
    bool readOnly = true; // explicit variable, as Preferences does not have a "read only" mode, which can be controlled without a end/begin call again
    ~OswConfig();

    friend std::unique_ptr<OswConfig>::deleter_type;
    friend OswConfigKeyString;
    friend OswConfigKeyPassword;
    friend OswConfigKeyDropDown;
    friend OswConfigKeyUnsignedLong;
    friend OswConfigKeyInt;
    friend OswConfigKeyShort;
    friend OswConfigKeyRGB;
    friend OswConfigKeyBool;
    friend OswConfigKeyDouble;
    friend OswConfigKeyFloat;
  private:
    static std::unique_ptr<OswConfig> instance;

    OswConfig();
    void loadAllKeysFromNVS();
};

#endif
