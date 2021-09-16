#include "osw_service.h"

void OswServiceTask::setup() {
  this->taskEnabled = true;
}
void OswServiceTask::stop() {
  this->taskEnabled = false;
}

bool OswServiceTask::isRunning() { return this->taskEnabled; }