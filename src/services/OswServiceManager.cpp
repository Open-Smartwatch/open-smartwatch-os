#include "./services/OswServiceManager.h"

#include "./services/OswServiceTasks.h"

void OswServiceManager::setup(OswHal *hal) {
  for (unsigned char i = 0; i < oswServiceTasksCount; i++) oswServiceTasks[i]->setup(hal);
}

void OswServiceManager::loop(OswHal *hal) {
  for (unsigned char i = 0; i < oswServiceTasksCount; i++)
    if (oswServiceTasks[i]->isRunning()) oswServiceTasks[i]->loop(hal);
}

void OswServiceManager::stop(OswHal *hal) {
  for (unsigned char i = 0; i < oswServiceTasksCount; i++) oswServiceTasks[i]->stop(hal);
}