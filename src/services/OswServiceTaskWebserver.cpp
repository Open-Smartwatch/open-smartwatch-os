#include <WebServer.h>

#include <assets/bundle.min.css.gz.h>
#include <assets/bundle.min.js.gz.h>
#include <assets/index.html.gz.h>

#include "osw_hal.h"
#include <osw_ui.h>
#include "services/OswServiceTasks.h"
#include "services/OswServiceTaskWiFi.h"
#include "services/OswServiceManager.h"
#include "./services/OswServiceTaskWebserver.h"

void OswServiceTaskWebserver::handleAuthenticated(std::function<void(void)> handler) {
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": " + this->m_webserver->uri());
#endif
  if (!this->m_webserver->authenticate("admin", this->m_uiPassword.c_str())) {
    return this->m_webserver->requestAuthentication();
  }
  handler();
}

void OswServiceTaskWebserver::handleUnauthenticated(std::function<void(void)> handler) {
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": " + this->m_webserver->uri());
#endif
  handler();
}

void OswServiceTaskWebserver::handleIndex() {
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

void OswServiceTaskWebserver::setup(OswHal* hal) {
  OswServiceTask::setup(hal);
  //Do not call enableWebserver here, so the loop() / developers could do this later on
}

void OswServiceTaskWebserver::loop(OswHal* hal) {
  if(OswServiceAllTasks::wifi.isConnected())
    this->enableWebserver();
  else
    this->disableWebserver();
  if (this->m_webserver) this->m_webserver->handleClient();
}

void OswServiceTaskWebserver::stop(OswHal* hal) {
  OswServiceTask::stop(hal);
  this->disableWebserver(); //Make sure the webserver is also stopped
}

void OswServiceTaskWebserver::enableWebserver() {
  if(this->m_webserver)
    return;

  this->m_uiPassword = String((int)(rand() % 90000 + 10000));  // Generate a random ui password on loading

  this->m_webserver = new WebServer(80);
  this->m_webserver->on("/", [this] { this->handleAuthenticated([this] { this->handleIndex(); }); });
  this->m_webserver->on("/index.html", [this] { this->handleAuthenticated([this] { this->handleIndex(); }); });
  this->m_webserver->on("/bundle.min.css", [this] { this->handleUnauthenticated([this] { this->handleCss(); }); });
  this->m_webserver->on("/bundle.min.js", [this] { this->handleUnauthenticated([this] { this->handleJs(); }); });
  this->m_webserver->on("/config.json", [this] { this->handleAuthenticated([this] { this->handleConfigJson(); }); });
  this->m_webserver->on("/data.json", HTTP_POST, [this] { this->handleAuthenticated([this] { this->handleDataJson(); }); });
  this->m_webserver->begin();
  
#ifdef DEBUG
    Serial.println(String(__FILE__) + ": Active.");
#endif
}

void OswServiceTaskWebserver::disableWebserver() {
  if (!this->m_webserver)
    return;

  this->m_webserver->stop();
  delete this->m_webserver;
  this->m_webserver = nullptr;

#ifdef DEBUG
    Serial.println(String(__FILE__) + ": Inactive.");
#endif
}

String OswServiceTaskWebserver::getPassword() const {
  return this->m_uiPassword;
}

WebServer* OswServiceTaskWebserver::getTaskWebserver() const {
  return this->m_webserver;
}