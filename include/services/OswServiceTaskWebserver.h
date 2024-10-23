#ifdef OSW_FEATURE_WIFI
#ifndef OSW_SERVICE_TASKWEBSERVER_H
#define OSW_SERVICE_TASKWEBSERVER_H

#include <functional>

#include "Uri.h"
#include "osw_service.h"

class WebServer;

class OswServiceTaskWebserver : public OswServiceTask {
  public:
    const unsigned int apiVersion = 1;

    OswServiceTaskWebserver() {};
    ~OswServiceTaskWebserver() {};

    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;

    void enableWebserver();
    void disableWebserver();
    bool webserverActive();

    WebServer* getTaskWebserver() const;
    String getPassword() const;

  private:
    class StartWithUri : public Uri {
      public:
        explicit StartWithUri(const char* uri) : Uri(uri) {}
        explicit StartWithUri(const String& uri) : Uri(uri) {}

        Uri* clone() const override {
            return new StartWithUri(_uri);
        }

        virtual bool canHandle(const String& requestUri, std::vector<String>& pathArgs) override {
            if(Uri::canHandle(requestUri, pathArgs))
                return true;
            return requestUri.startsWith(_uri);
        };
    };

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
    void handleReboot();
    void handleConfigReset();
    void handleFieldJson();
    void handleFieldSetter();

    enum class AssetId {
        INDEX_HTML,
        MAIN_JS,
        POLYFILLS_JS,
        RUNTIME_JS,
        STYLES_CSS
    };
    void handleAsset(AssetId assId);
#ifdef RAW_SCREEN_SERVER
    void handleScreenServer();
#endif
};

#endif
#endif
