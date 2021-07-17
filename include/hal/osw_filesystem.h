#pragma once

#define FS_MOUNT_POINT "/data"

class FileSystemHal {
 public:
  FileSystemHal(){};
  virtual bool initialize() { return true; };
  virtual ~FileSystemHal(){};
};
