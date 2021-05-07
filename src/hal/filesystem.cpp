#include "osw_hal.h"

void OswHal::setupFileSystem() {
    if (fileSystem != NULL && !fileSystem->initialize()) {
        perror("Failed to initialize osw filesystem");
    }
}