#include "osw_config.h"

#include <nvs_flash.h>
#include <rom/rtc.h>

#include <ArduinoJson.h>
#include "osw_config_keys.h"

#include "osw_ui.h" // For color reloading

OswConfig OswConfig::instance = OswConfig();

OswConfig::OswConfig(){};

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
#ifndef NDEBUG
    Serial.println("Invalid config version detected -> starting fresh...");
#endif
    this->reset();
  }
  // Increase boot counter only if not coming from deepsleep.
  if (rtc_get_reset_reason(0) != 5 && rtc_get_reset_reason(1) != 5) {
    res = this->prefs.putInt(this->configBootCntKey, this->prefs.getInt(this->configBootCntKey, -1) + 1);
    assert(res);
  }
  // Load all keys value into cache
  for(size_t i = 0; i < oswConfigKeysCount; i++)
    oswConfigKeys[i]->loadValueFromNVS();
#ifndef NDEBUG
  Serial.print("Config loaded! Version? ");
  Serial.println(this->prefs.getShort(this->configVersionKey));
  Serial.print("Boot count? ");
  Serial.println(this->prefs.getInt(this->configBootCntKey));
#endif
}

OswConfig* OswConfig::getInstance() { return &OswConfig::instance; };

void OswConfig::enableWrite() { this->readOnly = false; };

void OswConfig::disableWrite() { this->readOnly = true; };

/**
 * Get the amount of sytem boots since the last config wipe.
 */
int OswConfig::getBootCount() { return this->prefs.getInt(this->configBootCntKey); }

/**
 * Resets this namespace by formatting the nvs parition.
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

OswConfig::~OswConfig(){};

String OswConfig::getConfigJSON() {
  DynamicJsonDocument config(6144); //If you suddenly start missing keys, try increasing this...
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
    config["entries"][i]["type"] = key->type;
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

  DynamicJsonDocument config(6144);
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
      Serial.println("WARNING: Unknown key id \"" + String(entryId) + "\" provided -> ignoring...");
      continue;
    }
#ifndef NDEBUG
    Serial.print("Going to write config key id ");
    Serial.print(entry["id"].as<const char*>());
    Serial.print(" as ");
    Serial.print(key->label);
    Serial.println("...");
#endif
    key->fromString(entry["value"]);
#ifndef NDEBUG
    Serial.println("...done!");
#endif
  }

  // Reload UI colors
  OswUI::getInstance()->resetTextColors();
}
