#include "hal/esp32/sd_filesystem.h"

#include <SD.h>

#include "osw_pins.h"

bool _hasSD = false;
bool _isSDMounted = false;

bool SDFileSystemHal::initialize() {
  // Mount the filesystem and register vfs
  SD.begin(SD_CS);

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    _hasSD = false;
#ifndef NDEBUG
    Serial.println("ERR_SD_MISSING");
#endif
    return false;
  } else {
    _hasSD = true;
    // there is a card
    if (!SD.begin(SD_CS)) {
#ifndef NDEBUG
      Serial.println("ERR_SD_MOUNT_FAILED");
#endif
      _isSDMounted = false;

      return false;
    }
    _isSDMounted = true;
    return true;
  }
}
