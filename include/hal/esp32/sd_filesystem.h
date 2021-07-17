#pragma once

#include "hal/osw_filesystem.h"

class SDFileSystemHal : public FileSystemHal {
 public:
  SDFileSystemHal(){};
  virtual bool initialize();
  ~SDFileSystemHal(){};
};
