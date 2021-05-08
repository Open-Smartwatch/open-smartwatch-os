#include "hal/esp32/spiffs_filesystem.h"

bool SPIFFSFileSystemHal::initialize() {
  // Mount the filesystem and register vfs
  return SPIFFS.begin(true, FS_MOUNT_POINT);
}
