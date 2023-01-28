#include <filesystem>
#include <iostream>
#include <OswLogger.h>

#include "../include/nvs_flash.h"
#include "../include/Defines.h"

#include "../include/Preferences.h"

esp_err_t nvs_flash_erase() {
    std::filesystem::remove_all(Preferences::preferencesFolderName);
    OSW_LOG_I("Deleted NVS path: ", Preferences::preferencesFolderName);
    return ESP_OK;
}

esp_err_t nvs_flash_init() {
    if(!std::filesystem::exists(Preferences::preferencesFolderName)) {
        std::filesystem::create_directories(Preferences::preferencesFolderName);
        OSW_LOG_I("Created NVS path: ", Preferences::preferencesFolderName);
        return ESP_FAIL;
    }
    return ESP_OK;
}