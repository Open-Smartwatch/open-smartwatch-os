#include <cassert>

#include "osw_config.h"

#include <nvs_flash.h>
#ifndef OSW_EMULATOR
#include <rom/rtc.h>
#endif

#include <ArduinoJson.h>
#include "osw_config_keys.h"

#include <osw_hal.h> // For timezone reloading
#include <osw_ui.h> // For color reloading
#include "apps/watchfaces/OswAppWatchfaceDigital.h"

std::unique_ptr<OswConfig> OswConfig::instance = nullptr;

OswConfig::OswConfig() {};

/**
 * This function prepares the nvs and loads all stored config keys into
 * the cache (if supported from the key type).
 * This function must be called ONCE from the mains setup().
 * Any static implementation of this will be executed too early and fail
 * due an uninitialized nvs storage!
 * NOTE: By default this is all read-only to reduce the stress on the flash,
 * therefore any call on any setter will just be silently IGNORED!
 */
void OswConfig::setup() {
    // Init Preferences.h
    bool res = this->prefs.begin(this->configNamespace, false);
    assert(res);
    // Make sure the config version fits...
    if (this->prefs.getShort(this->configVersionKey, this->configVersionValue + 1) != this->configVersionValue) {
        //...otherwise wipe everything (we are going to fully wipe the nvs, just in case other namespaces exist)!
        OSW_LOG_W("Invalid config version detected -> starting fresh...");
        this->reset();
    }
    // Increase boot counter only if not coming from deepsleep -> https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ResetReason/ResetReason.ino
    if (rtc_get_reset_reason(0) != 5 and rtc_get_reset_reason(1) != 5) {
        res = this->prefs.putInt(this->configBootCntKey, this->prefs.getInt(this->configBootCntKey, -1) + 1);
        assert(res);
    }
    // Load all keys value into cache
    for(size_t i = 0; i < oswConfigKeysCount; i++)
        oswConfigKeys[i]->loadValueFromNVS();
    OSW_LOG_D("Config loaded! Version? ", this->prefs.getShort(this->configVersionKey));
    OSW_LOG_D("Boot count? ", this->prefs.getInt(this->configBootCntKey));
}

OswConfig* OswConfig::getInstance() {
    if(OswConfig::instance == nullptr)
        OswConfig::instance.reset(new OswConfig());
    return OswConfig::instance.get();
};

void OswConfig::resetInstance() {
    return OswConfig::instance.reset();
};

void OswConfig::enableWrite() {
    OSW_LOG_D("Enabled write mode!");
    this->readOnly = false;
}

void OswConfig::disableWrite() {
    OSW_LOG_D("Disabled write mode.");
    this->readOnly = true;
}

/**
 * Get the amount of system boots since the last config wipe.
 */
int OswConfig::getBootCount() {
    return this->prefs.getInt(this->configBootCntKey);
}

/**
 * Resets this namespace by formatting the nvs partition.
 */
void OswConfig::reset() {
    this->prefs.end();
    bool res = nvs_flash_erase() == ESP_OK;
    assert(res);
    res = nvs_flash_init() == ESP_OK;
    assert(res);
    res = this->prefs.begin(this->configNamespace, false);
    assert(res);
    res = this->prefs.putShort(this->configVersionKey, this->configVersionValue) == sizeof(short);
    assert(res);
}

OswConfig::~OswConfig() {};

String OswConfig::getConfigJSON() {
    DynamicJsonDocument config(16384); //If you suddenly start missing keys, try increasing this...
    /*
     * !!!NOTE!!!
     *
     * This could be massively optimized by using the id as
     * entry index/object key and also by using shorter key
     * names.
     */

    unsigned char i = 0;
    for (; i < oswConfigKeysCount; i++) {
        const OswConfigKey* key = oswConfigKeys[i];
        config["entries"][i]["id"] = key->id;
        config["entries"][i]["section"] = key->section;
        config["entries"][i]["label"] = key->label;
        if(key->help)
            config["entries"][i]["help"] = key->help;
        char typeBuffer[2] = {(char)(key->type), '\0'};
        config["entries"][i]["type"] = (char*) typeBuffer; // The type is "OswConfigKeyTypedUIType", so we have to create a char* as ArduinoJSON takes these (only char*!) in as a copy
        config["entries"][i]["default"] = key->toDefaultString();
        config["entries"][i]["value"] = key->toString();
    }

    String returnme;
    serializeJson(config, returnme);
    return returnme;
}

void OswConfig::parseDataJSON(const char* json) {
    /*
     * !!!NOTE!!!
     *
     * This could be massively optimized by using the id as
     * entry index/object key and also by using shorter key
     * names.
     */

    DynamicJsonDocument config(16384);
    deserializeJson(config, json);
    JsonArray entries = config["entries"].as<JsonArray>();

    for (auto it = entries.begin(); it != entries.end(); ++it) {
        // Now find the current config key instance
        JsonObject entry = it->as<JsonObject>();
        OswConfigKey* key = nullptr;
        String entryId = entry["id"];
        for (unsigned char i = 0; i < oswConfigKeysCount; i++)
            if (entryId == oswConfigKeys[i]->id) {
                key = oswConfigKeys[i];
                break;
            }
        if (!key) {
            OSW_LOG_W("Unknown key id \"", entryId, "\" provided -> ignoring...");
            continue;
        }
        OSW_LOG_D("Going to write config key id ", entry["id"].as<const char*>(), " as ", key->label, "...");
        key->fromString(entry["value"]);
    }

    this->notifyChange();
}

void OswConfig::notifyChange() {
    // Reload parts of the OS, which buffer values
    OswUI::getInstance()->resetTextColors();
    OswHal::getInstance()->updateTimezoneOffsets();
    OswAppWatchfaceDigital::refreshDateFormatCache();
}