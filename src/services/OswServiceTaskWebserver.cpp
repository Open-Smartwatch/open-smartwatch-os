#ifdef OSW_FEATURE_WIFI
#include <WebServer.h>
#include <Update.h> // OTA by file upload
#include <HTTPClient.h> // OTA by uri
#include <ArduinoJson.h>

#include <assets/bundle.min.css.gz.h>
#include <assets/bundle.min.js.gz.h>
#include <assets/index.html.gz.h>
#include <assets/www/index.html.gz.h>
#include <assets/www/update.html.gz.h>

#include "osw_hal.h"
#include <osw_ui.h>
#include <osw_config.h>
#include "services/OswServiceTasks.h"
#include "services/OswServiceTaskWiFi.h"
#include "services/OswServiceManager.h"
#include "./services/OswServiceTaskWebserver.h"

void OswServiceTaskWebserver::handleAuthenticated(std::function<void(void)> handler) {
#ifndef NDEBUG
  Serial.println(String(__FILE__) + ": " + this->m_webserver->uri());
#endif
  if (!this->m_webserver->authenticate("admin", this->m_uiPassword.c_str())) {
    return this->m_webserver->requestAuthentication();
  }
  handler();
}

void OswServiceTaskWebserver::handleUnauthenticated(std::function<void(void)> handler) {
#ifndef NDEBUG
  Serial.println(String(__FILE__) + ": " + this->m_webserver->uri());
#endif
  handler();
}

void OswServiceTaskWebserver::handleIndex() {
  this->m_webserver->sendHeader(F("Content-Encoding"), F("gzip"));
  this->m_webserver->send_P(200, "text/html", (const char*)index_html_gz, index_html_gz_len);
}

void OswServiceTaskWebserver::handleUpdate() {
  /**
   * When you ever need to test the ota by uri update method, just serve the
   * firmware.bin file using "python3 -m http.server" inside the respective directory.
   * IF you get CORS errors, try to use https://stackoverflow.com/a/21957017 - this is
   * a python http server, which sets the "Allow-Origin-Header" to allow your browser to
   * "predownload" the firmware.
   */
  this->m_webserver->sendHeader(F("Content-Encoding"), F("gzip"));
  this->m_webserver->send_P(200, "text/html", (const char*)update_html_gz, update_html_gz_len);
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
  Serial.println(String(__FILE__) + ": [OTA] URL: " + updateURL);

  // Perform the update
  HTTPClient http;
  http.begin(updateURL);
  http.useHTTP10(true); //To prevent any encodings

  int code = http.GET();
  int size = http.getSize();

  if(code != 200 or size == 0) {
    Serial.println(String(__FILE__) + ": [OTA] Fetch failed: " + http.errorToString(code));
    this->m_webserver->send(400, "text/plain", http.errorToString(code));
    return;
  }

  OswUI::getInstance()->startProgress("OTA Update");
  OswUI::getInstance()->getProgressBar()->setColor(OswUI::getInstance()->getDangerColor());

  if(!Update.begin(size)) {
    Serial.println(String(__FILE__) + ": [OTA] Begin failed: " + Update.errorString());
    this->m_webserver->send(400, "text/plain", Update.errorString());
    OswUI::getInstance()->stopProgress();
    return;
  }

  if(updateMD5.length()) {
    Update.setMD5(updateMD5.c_str());
    Serial.println(String(__FILE__) + ": [OTA] MD5: " + updateMD5);
  }

  if(Update.writeStream(*http.getStreamPtr()) != size) {
    Serial.println(String(__FILE__) + ": [OTA] Write failed: " + Update.errorString());
    this->m_webserver->send(400, "text/plain", Update.errorString());
    OswUI::getInstance()->stopProgress();
    return;
  }

  if(!Update.end()) {
    Serial.println(String(__FILE__) + ": [OTA] Finish failed: " + Update.errorString());
    this->m_webserver->send(400, "text/plain", Update.errorString());
    OswUI::getInstance()->stopProgress();
    return;
  }

  OswUI::getInstance()->getProgressBar()->setProgress(1.0f);
  Serial.println(String(__FILE__) + ": [OTA] Finished!");
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
    if (upload.status == UPLOAD_FILE_START) {
      Serial.println(String(__FILE__) + ": [OTA] Name: " + upload.filename);
      if (!Update.begin(UPDATE_SIZE_UNKNOWN))
        throw false;
      String updateMD5 = this->m_webserver->header("x-UpdateHash");
      if(updateMD5.length()) {
        Update.setMD5(updateMD5.c_str());
        Serial.println(String(__FILE__) + ": [OTA] MD5: " + updateMD5);
      }
      OswUI::getInstance()->startProgress("OTA Update");
      OswUI::getInstance()->getProgressBar()->setColor(OswUI::getInstance()->getDangerColor());
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      // This is maybe not the best indicator for a defective update, but it works well enough...
      if(upload.currentSize == 0)
        throw true;
      Serial.println(String(__FILE__) + ": [OTA] Next chunk: " + upload.currentSize);
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        throw true;
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true))
        Serial.println(String(__FILE__) + ": [OTA] Finished after " + upload.totalSize + " bytes!");
      else
        throw false;
    }
  } catch (bool abortUpdate) {
    if(abortUpdate)
      Update.abort();
    Serial.println(String(__FILE__) + ": [OTA] Failed: " + Update.errorString());
    OswUI::getInstance()->stopProgress();
    return;
  }
}

void OswServiceTaskWebserver::handleConfig() {
  this->m_webserver->sendHeader(F("Content-Encoding"), F("gzip"));
  this->m_webserver->send_P(200, "text/html", (const char*)dist_data_index_html_gz, dist_data_index_html_gz_len);
}

void OswServiceTaskWebserver::handleCss() {
  this->m_webserver->sendHeader(F("Content-Encoding"), F("gzip"));
  this->m_webserver->send_P(200, "text/css", (const char*)dist_data_bundle_min_css_gz, dist_data_bundle_min_css_gz_len);
}

void OswServiceTaskWebserver::handleJs() {
  this->m_webserver->sendHeader(F("Content-Encoding"), F("gzip"));
  this->m_webserver->send_P(200, "application/javascript", (const char*)dist_data_bundle_min_js_gz,
                 dist_data_bundle_min_js_gz_len);
}

void OswServiceTaskWebserver::handleConfigJson() { this->m_webserver->send(200, "application/json", OswConfig::getInstance()->getConfigJSON()); }

void OswServiceTaskWebserver::handleInfoJson() {
  DynamicJsonDocument config(1024);
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

void OswServiceTaskWebserver::handleDataJson() {
  if (this->m_webserver->hasArg("plain") == false) {
    this->m_webserver->send(422, "application/json", "{\"error\": \"CFG_MISSING\"}");
    return;
  }

  OswConfig::getInstance()->enableWrite();
  OswConfig::getInstance()->parseDataJSON(this->m_webserver->arg("plain").c_str());
  OswConfig::getInstance()->disableWrite();

  // TODO: error handling?
  this->m_webserver->send(200, "application/json", "{\"success\":true}");
  OswUI::getInstance()->resetTextColors();
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
#ifndef NDEBUG
    Serial.println(String(__FILE__) + ": REBOOT REQUEST RECEIVED. REBOOT IN 2 SECONDS!");
#endif
    sleep(2); // Just to make sure all web requests are finished...
    ESP.restart();
  }
}

void OswServiceTaskWebserver::stop() {
  OswServiceTask::stop();
  this->disableWebserver(); //Make sure the webserver is also stopped
}

void OswServiceTaskWebserver::enableWebserver() {
  if(this->m_webserver)
    return;

  this->m_uiPassword = String(random(10000, 99999));  // Generate a random ui password on loading

  this->m_webserver = new WebServer(80);
  this->m_webserver->on("/", [this] { this->handleAuthenticated([this] { this->handleIndex(); }); });
  this->m_webserver->on("/update", [this] { this->handleAuthenticated([this] { this->handleUpdate(); }); });
  this->m_webserver->on("/config", [this] { this->handleAuthenticated([this] { this->handleConfig(); }); });
  this->m_webserver->on("/bundle.min.css", [this] { this->handleUnauthenticated([this] { this->handleCss(); }); });
  this->m_webserver->on("/bundle.min.js", [this] { this->handleUnauthenticated([this] { this->handleJs(); }); });
  this->m_webserver->on("/config.json", [this] { this->handleAuthenticated([this] { this->handleConfigJson(); }); });
  this->m_webserver->on("/data.json", HTTP_POST, [this] { this->handleAuthenticated([this] { this->handleDataJson(); }); });
  this->m_webserver->on("/api/info", [this] { this->handleAuthenticated([this] { this->handleInfoJson(); }); });
  this->m_webserver->on("/api/ota/active", [this] { this->handleAuthenticated([this] { this->handleActiveOTARequest(); }); });
  this->m_webserver->on("/api/ota/passive", HTTP_POST, [this] { this->handleAuthenticated([this] { this->handlePassiveOTARequest(); }); }, [this] { this->handleAuthenticated([this] { this->handleOTAFile(); }); });
  this->m_webserver->begin();

  /**
   * We also have to make sure to collect the following headers on any request.
   * We are using headers, as post keys could be processed after the file upload,
   * therefore we would receive the md5 AFTER the update is already complete.
   */
  const char* headers[] = { "x-UpdateHash" };
  this->m_webserver->collectHeaders(headers, 1);
  
#ifndef NDEBUG
    Serial.println(String(__FILE__) + ": Active.");
#endif
}

void OswServiceTaskWebserver::disableWebserver() {
  if (!this->m_webserver)
    return;

  this->m_webserver->stop();
  delete this->m_webserver;
  this->m_webserver = nullptr;

#ifndef NDEBUG
    Serial.println(String(__FILE__) + ": Inactive.");
#endif
}

String OswServiceTaskWebserver::getPassword() const {
  return this->m_uiPassword;
}

WebServer* OswServiceTaskWebserver::getTaskWebserver() const {
  return this->m_webserver;
}
#endif