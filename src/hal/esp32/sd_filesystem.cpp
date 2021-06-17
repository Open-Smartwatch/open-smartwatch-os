#include "hal/esp32/sd_filesystem.h"

#include <SD.h>

#include "osw_pins.h"

bool SDFileSystemHal::initialize() {
  // Mount the filesystem and register vfs
  SD.begin(SD_CS);

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
#ifdef DEBUG
    Serial.println("ERR_SD_MISSING");
#endif
    return false;
  } else {
    // there is a card
    if (!SD.begin(SD_CS)) {
#ifdef DEBUG
      Serial.println("ERR_SD_MOUNT_FAILED");
#endif
      return false;
    }
    return true;
  }
}
