#pragma once

#include "hal/osw_filesystem.h"
#include "SPIFFS.h"

class SPIFFSFileSystemHal: public FileSystemHal {
    public:
        SPIFFSFileSystemHal() {};
        virtual bool initialize();
        ~SPIFFSFileSystemHal() {};
};