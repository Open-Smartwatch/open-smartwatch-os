#ifndef OSW_SERVICE_H
#define OSW_SERVICE_H
#include <osw_app.h>

class OswServiceTask : public OswApp {
 public:
  OswServiceTask() : OswApp() {}
  virtual void setup() override;
  bool isRunning();
  virtual void stop() override;
  virtual ~OswServiceTask(){};

 private:
  bool taskEnabled = false;
};
#endif