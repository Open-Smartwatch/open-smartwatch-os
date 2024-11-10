#ifdef OSW_FEATURE_WIFI
#include <WebServer.h>
#include <Update.h> // OTA by file upload
#include <HTTPClient.h> // OTA by uri
#include <ArduinoJson.h>

#include "osw_hal.h"
#include <osw_ui.h>
#include <osw_config.h>
#include "services/OswServiceTasks.h"
#include "services/OswServiceTaskWiFi.h"
#include "services/OswServiceManager.h"
#include "services/OswServiceTaskWebserver.h"

#include "assets/www/index.html.gz.h"
#include "assets/www/main.js.gz.h"
#include "assets/www/polyfills.js.gz.h"
#include "assets/www/runtime.js.gz.h"
#include "assets/www/styles.css.gz.h"

void OswServiceTaskWebserver::handleAuthenticated(std::function<void(void)> handler) {
    OSW_LOG_D(this->m_webserver->uri());
    if (!this->m_webserver->authenticate("admin", this->m_uiPassword.c_str())) {
        return this->m_webserver->requestAuthentication();
    }
    handler();
}

void OswServiceTaskWebserver::handleUnauthenticated(std::function<void(void)> handler) {
    OSW_LOG_D(this->m_webserver->uri());
    handler();
}

void OswServiceTaskWebserver::handleActiveOTARequest() {
    // Check if config was received
    if (this->m_webserver->hasArg("plain")== false) {
        this->m_webserver->send(400, "text/plain", "URL missing.");
        return;
    }

    // Check if we've got an MD5;URL using http
    String updateMD5;
    String updateURL;
    {
        String temp = this->m_webserver->arg("plain");
        int pos = temp.indexOf(';');
        if(pos == 32) //Okay, we assume the first 32 chars are the md5 hex digset
            updateMD5 = temp.substring(0, pos);
        updateURL = temp.substring(pos + 1);
    }
    if(!updateURL.startsWith("http://")) {
        this->m_webserver->send(400, "text/plain", "URI has wrong protocol, only HTTP is allowed.");
        return;
    }
    OSW_LOG_I("[OTA] URL: ", updateURL);

    // Perform the update
    HTTPClient http;
    http.begin(updateURL);
    http.useHTTP10(true); //To prevent any encodings

    int code = http.GET();
    int size = http.getSize();

    if(code != 200 or size == 0) {
        OSW_LOG_E("[OTA] Fetch failed: ", http.errorToString(code));
        this->m_webserver->send(400, "text/plain", http.errorToString(code));
        return;
    }

    OswUI::getInstance()->startProgress("OTA Update");
    OswUI::getInstance()->getProgressBar()->setColor(OswUI::getInstance()->getDangerColor());

    if(!Update.begin(size)) {
        OSW_LOG_E("[OTA] Begin failed: ", Update.errorString());
        this->m_webserver->send(400, "text/plain", Update.errorString());
        OswUI::getInstance()->stopProgress();
        return;
    }

    if(updateMD5.length()) {
        Update.setMD5(updateMD5.c_str());
        OSW_LOG_I("[OTA] MD5: ", updateMD5);
    }

    if(Update.writeStream(*http.getStreamPtr()) != size) {
        OSW_LOG_E("[OTA] Write failed: ", Update.errorString());
        this->m_webserver->send(400, "text/plain", Update.errorString());
        OswUI::getInstance()->stopProgress();
        return;
    }

    if(!Update.end()) {
        OSW_LOG_E("[OTA] Finish failed: ", Update.errorString());
        this->m_webserver->send(400, "text/plain", Update.errorString());
        OswUI::getInstance()->stopProgress();
        return;
    }

    OswUI::getInstance()->getProgressBar()->setProgress(1.0f);
    OSW_LOG_I("[OTA] Finished!");
    this->m_webserver->send(200, "text/plain", "OK");
    this->m_restartRequest = true;
}

void OswServiceTaskWebserver::handlePassiveOTARequest() {
    if(!Update.hasError()) {
        OswUI::getInstance()->getProgressBar()->setProgress(1.0f);
        this->m_webserver->send(200, "text/plain", "OK");
        this->m_restartRequest = true;
    } else {
        this->m_webserver->send(400, "text/plain", Update.errorString());
    }
}

void OswServiceTaskWebserver::handleOTAFile() {
    try {
        HTTPUpload& upload = this->m_webserver->upload();
        switch(upload.status) {
        case UPLOAD_FILE_START:
            OSW_LOG_I("[OTA] Name: ", upload.filename);
            if (!Update.begin(UPDATE_SIZE_UNKNOWN))
                throw false;
            {
                String updateMD5 = this->m_webserver->header("x-UpdateHash");
                if(updateMD5.length()) {
                    Update.setMD5(updateMD5.c_str());
                    OSW_LOG_I("[OTA] MD5: ", updateMD5);
                }
            }
            OswUI::getInstance()->startProgress("OTA Update");
            OswUI::getInstance()->getProgressBar()->setColor(OswUI::getInstance()->getDangerColor());
            break;
        case UPLOAD_FILE_WRITE:
            // This is maybe not the best indicator for a defective update, but it works well enough...
            if(upload.currentSize == 0)
                throw true;
            OSW_LOG_D("[OTA] Next chunk: ", upload.currentSize);
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                throw true;
            break;
        case UPLOAD_FILE_END:
            if (Update.end(true))
                OSW_LOG_I("[OTA] Finished after ", upload.totalSize, " bytes!");
            else
                throw false;
            break;
        default:
            // Oh... What?!
            break;
        }
    } catch (bool abortUpdate) {
        if(abortUpdate)
            Update.abort();
        OSW_LOG_E("[OTA] Failed: ", Update.errorString());
        OswUI::getInstance()->stopProgress();
        return;
    }
}

void OswServiceTaskWebserver::handleInfoJson() {
    DynamicJsonDocument config(1024);
    config["X"] = String(this->apiVersion);
    config["t"] = String(__DATE__) + ", " + __TIME__;
    config["v"] = String(__VERSION__);
    config["gh"] = String(GIT_COMMIT_HASH);
    config["gt"] = String(GIT_COMMIT_TIME);
    config["gb"] = String(GIT_BRANCH_NAME);
    config["bc"] = OswConfig::getInstance()->getBootCount();
    config["pe"] = String(PIO_ENV_NAME);

    String returnme;
    serializeJson(config, returnme);
    this->m_webserver->send(200, "application/json", returnme);
}

void OswServiceTaskWebserver::handleCategoriesJson() {
    this->m_webserver->send(200, "application/json", OswConfig::getInstance()->getCategoriesJson());
}

void OswServiceTaskWebserver::handleReboot() {
    this->m_restartRequest = true; // will be processed at least with a delay of some seconds
    this->m_webserver->send(200, "application/json", "{\"success\": true}");
}

void OswServiceTaskWebserver::handleConfigReset() {
    OswConfig::getInstance()->reset(this->m_webserver->hasArg("clearNVS"));
    OswConfig::getInstance()->notifyChange();
    this->m_webserver->send(200, "application/json", "{\"success\": true}");
}

void OswServiceTaskWebserver::handleFieldJson() {
    if (this->m_webserver->hasArg("id") == false) {
        this->m_webserver->send(422, "application/json", "{\"error\": \"CFG_MISSING\"}");
        return;
    }
    this->m_webserver->send(200, "application/json", OswConfig::getInstance()->getFieldJson(this->m_webserver->arg("id").c_str()));
}

void OswServiceTaskWebserver::handleFieldSetter() {
    if (this->m_webserver->hasArg("id") == false) {
        this->m_webserver->send(422, "application/json", "{\"error\": \"CFG_MISSING\"}");
        return;
    }
    if (this->m_webserver->hasArg("value") == false) {
        this->m_webserver->send(422, "application/json", "{\"error\": \"CFG_MISSING\"}");
        return;
    }
    OswConfig::getInstance()->enableWrite();
    OswConfig::getInstance()->setField(this->m_webserver->arg("id").c_str(), this->m_webserver->arg("value").c_str());
    OswConfig::getInstance()->disableWrite();
    this->m_webserver->send(200);
}

#ifdef RAW_SCREEN_SERVER
void OswServiceTaskWebserver::handleScreenServer() {
    // Note that we are using the screen buffer here, but is could be inactive right now -> activate and wait a while if it is
    bool disableBufferAgain = false;
    if(!OswHal::getInstance()->displayBufferEnabled()) {
        disableBufferAgain = true;
        {
            std::lock_guard<std::mutex> noRender(*OswUI::getInstance()->drawLock);
            OswHal::getInstance()->enableDisplayBuffer();
        }
        sleep(1); // After this a new frame should be available!
    }

    long contentLength = DISP_W * DISP_H * 3;
    uint8_t buf[3 * DISP_W];

    this->m_webserver->client().write("HTTP/1.1 200 OK");
    this->m_webserver->client().write((String("\r\nContent-Length: ") + String(contentLength)).c_str());
    this->m_webserver->client().write("\r\nContent-Type: application/octet-stream");
    this->m_webserver->client().write("\r\nConnection: close");
    this->m_webserver->client().write("\r\n\r\n");  // empty line for header<->body delimiter

    // Fetch the screenshot itself
    {
        std::lock_guard<std::mutex> noRender(*OswUI::getInstance()->drawLock);
        for (int y = 0; y < DISP_H; y++) {
            for (int x = 0; x < DISP_W; x++) {
                uint16_t rgb = OswHal::getInstance()->gfx()->getPixel(x, y);
                buf[x * 3 + 0] = rgb565_red(rgb);
                buf[x * 3 + 1] = rgb565_green(rgb);
                buf[x * 3 + 2] = rgb565_blue(rgb);
            }
            this->m_webserver->client().write(buf, 3 * DISP_W);
            yield();
        }
    }

    // Disable the buffer if it was inactive before
    if(disableBufferAgain) {
        std::lock_guard<std::mutex> noRender(*OswUI::getInstance()->drawLock);
        OswHal::getInstance()->disableDisplayBuffer();
    }
    OSW_LOG_D("Sent RAW screenshot!");
}
#endif

void OswServiceTaskWebserver::handleAsset(AssetId assId) {
    this->m_webserver->sendHeader("Content-Encoding", "gzip");
#ifndef NDEBUG
    // Cache the web ui for release builds
    this->m_webserver->sendHeader("Cache-Control", "max-age=86400, public, immutable"); // Cache for 1 day, allow shared caches and content will not change during ttl
#endif
    if(assId == OswServiceTaskWebserver::AssetId::INDEX_HTML)
        this->m_webserver->send_P(200, "text/html", (const char*)index_html_gz, index_html_gz_len);
    else if(assId == OswServiceTaskWebserver::AssetId::MAIN_JS)
        this->m_webserver->send_P(200, "text/javascript", (const char*)main_js_gz, main_js_gz_len);
    else if(assId == OswServiceTaskWebserver::AssetId::POLYFILLS_JS)
        this->m_webserver->send_P(200, "text/javascript", (const char*)polyfills_js_gz, polyfills_js_gz_len);
    else if(assId == OswServiceTaskWebserver::AssetId::RUNTIME_JS)
        this->m_webserver->send_P(200, "text/javascript", (const char*)runtime_js_gz, runtime_js_gz_len);
    else if(assId == OswServiceTaskWebserver::AssetId::STYLES_CSS)
        this->m_webserver->send_P(200, "text/css", (const char*)styles_css_gz, styles_css_gz_len);
    else
        throw std::runtime_error("Unknown asset requested!");
}

void OswServiceTaskWebserver::setup() {
    OswServiceTask::setup();
    //Do not call enableWebserver() here, so the loop() / developers could do this later on as needed
}

void OswServiceTaskWebserver::loop() {
    if(OswServiceAllTasks::wifi.isConnected())
        this->enableWebserver();
    else
        this->disableWebserver();
    if (this->m_webserver) this->m_webserver->handleClient();
    if(this->m_restartRequest) {
        OSW_LOG_W("REBOOT REQUEST RECEIVED. REBOOT IN 2 SECONDS!");
        sleep(2); // Just to make sure all web requests are finished...
        ESP.restart();
    }
}

void OswServiceTaskWebserver::stop() {
    this->disableWebserver(); //Make sure the webserver is also stopped
    OswServiceTask::stop();
}

void OswServiceTaskWebserver::enableWebserver() {
    if(this->m_webserver)
        return;

    this->m_uiPassword = String(random(10000, 99999));  // Generate a random ui password on loading

    this->m_webserver = new WebServer(80);
#ifndef NDEBUG
    // Allow CORS preflight requests without authentication against the API - do not use UriRegex, as it is HUGE (~ 10% of the binary size)
    this->m_webserver->on(StartWithUri("/api/"), HTTP_OPTIONS, [this] { this->m_webserver->send(204); });
    this->m_webserver->enableCORS(true); // Allow CORS for all requests - e.g. for developing the webinterface
    OSW_LOG_W("In non-release mode, CORS and unauthenticated HTTP options are enabled for all requests!");
#endif

    // UI Assets (UNAUTHENTICATED!!!) - because the UI will manage that by itself...
    auto indexCallback = [this] { this->handleUnauthenticated([this] { this->handleAsset(OswServiceTaskWebserver::AssetId::INDEX_HTML); }); };
    this->m_webserver->on("/", HTTP_GET, indexCallback);
    this->m_webserver->on("/index.html", HTTP_GET, indexCallback);
    this->m_webserver->onNotFound(indexCallback); // should be handled by the UI
    this->m_webserver->on("/main.js", HTTP_GET, [this] { this->handleUnauthenticated([this] { this->handleAsset(OswServiceTaskWebserver::AssetId::MAIN_JS); }); });
    this->m_webserver->on("/polyfills.js", HTTP_GET, [this] { this->handleUnauthenticated([this] { this->handleAsset(OswServiceTaskWebserver::AssetId::POLYFILLS_JS); }); });
    this->m_webserver->on("/runtime.js", HTTP_GET, [this] { this->handleUnauthenticated([this] { this->handleAsset(OswServiceTaskWebserver::AssetId::RUNTIME_JS); }); });
    this->m_webserver->on("/styles.css", HTTP_GET, [this] { this->handleUnauthenticated([this] { this->handleAsset(OswServiceTaskWebserver::AssetId::STYLES_CSS); }); });

    // API (if you change anything here, also increment this->apiVersion!)
    this->m_webserver->on("/api/info", HTTP_GET, [this] { this->handleAuthenticated([this] { this->handleInfoJson(); }); });
    this->m_webserver->on("/api/reboot", HTTP_GET, [this] { this->handleAuthenticated([this] { this->handleReboot(); }); });
    this->m_webserver->on("/api/config/reset", HTTP_GET, [this] { this->handleAuthenticated([this] { this->handleConfigReset(); }); });
    this->m_webserver->on("/api/config/categories", HTTP_GET, [this] { this->handleAuthenticated([this] { this->handleCategoriesJson(); }); });
    this->m_webserver->on("/api/config/field", HTTP_GET, [this] { this->handleAuthenticated([this] { this->handleFieldJson(); }); });
    this->m_webserver->on("/api/config/field", HTTP_PUT, [this] { this->handleAuthenticated([this] { this->handleFieldSetter(); }); });
#ifdef RAW_SCREEN_SERVER
    this->m_webserver->on("/api/screenserver", [this] { this->handleUnauthenticated([this] { this->handleScreenServer(); }); });
    OSW_LOG_D("Started RAW ScreenServer under http://", OswServiceAllTasks::wifi.getIP().toString(), "/api/screenserver");
    OSW_LOG_W("The RAW ScreenServer is enabled does NOT require any authentication, please make sure to use it in trusted environments only!");
#endif
    this->m_webserver->on("/api/ota/active", HTTP_POST, [this] { this->handleAuthenticated([this] { this->handleActiveOTARequest(); }); });
    this->m_webserver->on("/api/ota/passive", HTTP_POST, [this] { this->handleAuthenticated([this] { this->handlePassiveOTARequest(); }); }, [this] { this->handleAuthenticated([this] { this->handleOTAFile(); }); });
    this->m_webserver->begin();

    /**
     * We also have to make sure to collect the following headers on any request.
     * We are using headers, as post keys could be processed after the file upload,
     * therefore we would receive the md5 AFTER the update is already complete.
     */
    const char* headers[] = { "x-UpdateHash" };
    this->m_webserver->collectHeaders(headers, 1);

    OSW_LOG_D("Active (password is ", this->m_uiPassword, ").");
}

void OswServiceTaskWebserver::disableWebserver() {
    if (!this->m_webserver)
        return;

    this->m_webserver->stop();
    delete this->m_webserver;
    this->m_webserver = nullptr;

    OSW_LOG_D("Inactive.");
}

bool OswServiceTaskWebserver::webserverActive() {
    return this->m_webserver != nullptr;
}

String OswServiceTaskWebserver::getPassword() const {
    return this->m_uiPassword;
}

WebServer* OswServiceTaskWebserver::getTaskWebserver() const {
    return this->m_webserver;
}
#endif
