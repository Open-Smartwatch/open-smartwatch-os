#pragma once

#include "SPIFFS.h"
#include "hal/osw_filesystem.h"

class SPIFFSFileSystemHal : public FileSystemHal {
 public:
  SPIFFSFileSystemHal(){};
  virtual bool initialize();
  ~SPIFFSFileSystemHal(){};
};
