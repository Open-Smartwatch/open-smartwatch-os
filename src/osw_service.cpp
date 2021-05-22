#include "osw_service.h"

void OswServiceTask::setup(OswHal* hal) {
  this->taskEnabled = true;
}
void OswServiceTask::stop(OswHal* hal) {
  this->taskEnabled = false;
}

bool OswServiceTask::isRunning() { return this->taskEnabled; }