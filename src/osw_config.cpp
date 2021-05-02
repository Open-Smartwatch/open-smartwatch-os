#include <nvs_flash.h>

#include "osw_config.h"

OswConfig OswConfig::instance = OswConfig();

OswConfig::OswConfig() {};

/**
 * This function must be called ONCE from the mains setup().
 * Any static implementation of this will be executed too early and fail
 * due an uninitialized nvs storage!
 * NOTE: By default this is all read-only to reduce the stress on the flash,
 * therefore any call on any setter will just be silently IGNORED!
 */
void OswConfig::setup() {
    //Init Preferences.h
    this->prefs.begin(this->configNamespace, false);
    //Make sure the config version fits...
    if(this->prefs.getShort(this->configVersionKey, this->configVersionValue + 1) != this->configVersionValue) {
        //...otherwise wipe everything (we are going to fully wipe the nvs, just in case other namespaces exist)!
#ifdef DEBUG
        Serial.println("Invalid config version detected -> starting fresh...");
#endif
        this->prefs.end();
        nvs_flash_erase();
        nvs_flash_init();
        this->prefs.begin(this->configNamespace, false);
        this->prefs.putShort(this->configVersionKey, this->configVersionValue);
    }
#ifdef DEBUG
        Serial.print("Config loaded! Version? ");
        Serial.println(this->prefs.getShort(this->configVersionKey));
#endif
}

OswConfig* OswConfig::getInstance() {
    return &OswConfig::instance;
};

void OswConfig::enableWrite() {
    this->readOnly = false;
};

void OswConfig::disableWrite() {
    this->readOnly = true;
};

OswConfig::~OswConfig() {};