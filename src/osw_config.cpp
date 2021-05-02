#include <nvs_flash.h>
#include <rom/rtc.h>

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
        this->reset();
    }
    //Increase boot counter only if not coming from deepsleep.
    if(rtc_get_reset_reason(0) != 5 && rtc_get_reset_reason(1) != 5)
        this->prefs.putInt(this->configBootCntKey, this->prefs.getInt(this->configBootCntKey, -1) + 1);
#ifdef DEBUG
    Serial.print("Config loaded! Version? ");
    Serial.println(this->prefs.getShort(this->configVersionKey));
    Serial.print("Boot count? ");
    Serial.println(this->prefs.getInt(this->configBootCntKey));
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

/**
 * Get the amount of sytem boots since the last config wipe.
 */
int OswConfig::getBootCount() {
    return this->prefs.getInt(this->configBootCntKey);
}

/**
 * Resets this namespace by formatting the nvs parition.
 */
void OswConfig::reset() {
    this->prefs.end();
    nvs_flash_erase();
    nvs_flash_init();
    this->prefs.begin(this->configNamespace, false);
    this->prefs.putShort(this->configVersionKey, this->configVersionValue);
}

OswConfig::~OswConfig() {};