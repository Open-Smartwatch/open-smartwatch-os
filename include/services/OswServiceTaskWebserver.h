#ifndef OSW_SERVICE_TASKWEBSERVER_H
#define OSW_SERVICE_TASKWEBSERVER_H

#include <functional>

#include "osw_service.h"

class WebServer;

class OswServiceTaskWebserver : public OswServiceTask {
 public:
  OswServiceTaskWebserver(){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);

  void enableWebserver();
  void disableWebserver();

  WebServer* getTaskWebserver() const;
  String getPassword() const;

  ~OswServiceTaskWebserver(){};

 private:
  WebServer* m_webserver = nullptr;
  String m_uiPassword;

  void handleAuthenticated(std::function<void(void)> handler);
  void handleUnauthenticated(std::function<void(void)> handler);
  void handleIndex();
  void handleCss();
  void handleJs();
  void handleConfigJson();
  void handleDataJson();
};

#endif