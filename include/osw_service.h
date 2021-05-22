#ifndef OSW_SERVICE_H
#define OSW_SERVICE_H
#include <osw_app.h>

class OswServiceTask : public OswApp {
 public:
  OswServiceTask() : OswApp() {}
  virtual void setup(OswHal* hal);
  bool isRunning();
  virtual void stop(OswHal* hal);
  virtual ~OswServiceTask(){};

 private:
  bool taskEnabled = false;
};
#endif