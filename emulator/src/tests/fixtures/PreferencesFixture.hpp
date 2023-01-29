#pragma once

#include <string>
#include "nvs_flash.h"
#include "Preferences.h"

class PreferencesFixture {
  public:
    PreferencesFixture() {
        // Change nvs name to avoid conflicts
        this->oldNamespace = Preferences::preferencesFolderName;
        // No need for nvs_flash_init(), which should be called by the Preferences as needed
        Preferences::preferencesFolderName = this->oldNamespace + "_" + std::to_string(rand());
    }

    ~PreferencesFixture() {
      nvs_flash_erase(); // Wipe this namespace again
      Preferences::preferencesFolderName = this->oldNamespace;
    }
  private:
    std::string oldNamespace;
    std::string newNamespace;
};