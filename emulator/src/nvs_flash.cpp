#include <filesystem>
#include <iostream>

#include "../include/nvs_flash.h"
#include "../include/Defines.h"

#include "../include/Preferences.h"

bool nvs_flash_erase() {
    std::filesystem::remove_all(Preferences::preferencesFolderName);
    std::cout << "Deleted NVS path: " << Preferences::preferencesFolderName << std::endl;
    return true;
}

bool nvs_flash_init() {
    if(!std::filesystem::exists(Preferences::preferencesFolderName)) {
        std::filesystem::create_directories(Preferences::preferencesFolderName);
        std::cout << "Created NVS path: " << Preferences::preferencesFolderName << std::endl;
        return true;
    }
    return true;
}