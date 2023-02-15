#ifdef OSW_FEATURE_WIFI
#ifndef OSW_SERVICE_TASKWEBSERVER_H
#define OSW_SERVICE_TASKWEBSERVER_H

#include <functional>

#include "osw_service.h"

class WebServer;

class OswServiceTaskWebserver : public OswServiceTask {
  public:
    OswServiceTaskWebserver() {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;

    void enableWebserver();
    void disableWebserver();
    bool webserverActive();

    WebServer* getTaskWebserver() const;
    String getPassword() const;

    ~OswServiceTaskWebserver() {};

  private:
    WebServer* m_webserver = nullptr;
    String m_uiPassword;
    bool m_restartRequest = false;

    void handleAuthenticated(std::function<void(void)> handler);
    void handleUnauthenticated(std::function<void(void)> handler);
    void handlePassiveOTARequest();
    void handleActiveOTARequest();
    void handleInfoJson();
    void handleOTAFile();
    void handleCategoriesJson();
    void handleFieldJson();
    void handleFieldSetter();
#ifdef RAW_SCREEN_SERVER
    void handleScreenServer();
#endif
};

#endif
#endif