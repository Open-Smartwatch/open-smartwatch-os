
#include "./apps/tools/config_mgmt.h"

#include <WebServer.h>
#include <assets/bundle.min.css.gz.h>
#include <assets/bundle.min.js.gz.h>
#include <assets/index.html.gz.h>
#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <osw_ui.h>

#include <services/OswServiceTasks.h>
#include <services/OswServiceTaskWiFi.h>

WebServer* server = nullptr;
String uiPassword;

#define CONIFG_AUTHENTICATE

void handleAuthenticated(std::function<void(void)> handler) {
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": " + server->uri());
#endif
  if (!server->authenticate("admin", uiPassword.c_str())) {
    return server->requestAuthentication();
  }
  handler();
}

void handleUnauthenticated(std::function<void(void)> handler) {
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": " + server->uri());
#endif
  handler();
}

void handleIndex() {
  server->sendHeader(F("Content-Encoding"), F("gzip"));
  server->send_P(200, "text/html", (const char*)dist_data_index_html_gz, dist_data_index_html_gz_len);
}

void handleCss() {
  server->sendHeader(F("Content-Encoding"), F("gzip"));
  server->send_P(200, "text/css", (const char*)dist_data_bundle_min_css_gz, dist_data_bundle_min_css_gz_len);
}

void handleJs() {
  server->sendHeader(F("Content-Encoding"), F("gzip"));
  server->send_P(200, "application/javascript", (const char*)dist_data_bundle_min_js_gz,
                 dist_data_bundle_min_js_gz_len);
}

void handleConfigJson() { server->send(200, "application/json", OswConfig::getInstance()->getConfigJSON()); }

void handleDataJson() {
  if (server->hasArg("plain") == false) {
    server->send(422, "application/json", "{\"error\": \"CFG_MISSING\"}");
    return;
  }

  OswConfig::getInstance()->enableWrite();
  OswConfig::getInstance()->parseDataJSON(server->arg("plain").c_str());
  OswConfig::getInstance()->disableWrite();

  // TODO: error handling?
  server->send(200, "application/json", "{\"success\":true}");
  OswUI::getInstance()->resetTextColors();
}

void OswAppConfigMgmt::setup(OswHal* hal) {
  uiPassword = String((int)(rand() % 90000 + 10000));  // Generate a random ui password on loading
}

void OswAppConfigMgmt::loop(OswHal* hal) {
  hal->gfx()->fill(ui->getBackgroundColor());
  hal->gfx()->setTextSize(2);

  // If we are already connected -> just (re-)start the config server now.
  if (!server && OswServiceAllTasks::wifi.isConnected()) {
    server = new WebServer(80);
    server->on("/", [] { handleAuthenticated(handleIndex); });
    server->on("/index.html", [] { handleAuthenticated(handleIndex); });
    server->on("/bundle.min.css", [] { handleUnauthenticated(handleCss); });
    server->on("/bundle.min.js", [] { handleUnauthenticated(handleJs); });
    server->on("/config.json", [] { handleAuthenticated(handleConfigJson); });
    server->on("/data.json", HTTP_POST, [] { handleAuthenticated(handleDataJson); });
    server->begin();
  }

  OswUI::getInstance()->setTextCursor(BUTTON_3);
  if (OswServiceAllTasks::wifi.isConnected()) {
    hal->gfx()->print(LANG_DISCONNECT);
  } else {
    hal->gfx()->print(LANG_CONNECT);
  }

  if (hal->btnHasGoneDown(BUTTON_3)) {
    if (OswServiceAllTasks::wifi.isConnected()) {
      OswServiceAllTasks::wifi.disconnectWiFi();
      OswServiceAllTasks::wifi.disableWiFi();
    } else {
      OswServiceAllTasks::wifi.enableWiFi();
      OswServiceAllTasks::wifi.connectWiFi();
    }
  }

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextMiddleAligned();

  if (OswServiceAllTasks::wifi.isConnected()) {
    if (server) server->handleClient();
    hal->gfx()->setTextCursor(120, 90);
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextColor(ui->getPrimaryColor(), ui->getBackgroundColor());
    hal->gfx()->println("IP:");
    hal->gfx()->setTextSize(2);
    hal->gfx()->println(OswServiceAllTasks::wifi.getIP().toString());
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextColor(ui->getWarningColor(), ui->getBackgroundColor());
    hal->gfx()->println("User:");
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextColor(ui->getDangerColor(), ui->getBackgroundColor());
    hal->gfx()->println("admin");
    hal->gfx()->setTextSize(1);
    hal->gfx()->println("Password:");
    hal->gfx()->setTextSize(2);
    hal->gfx()->println(uiPassword);
    hal->gfx()->setTextColor(ui->getForegroundColor(), ui->getBackgroundColor());

  } else {
    hal->gfx()->setTextCursor(120, 120);
    hal->gfx()->print("Configuration UI");
  }

  hal->requestFlush();
}

void OswAppConfigMgmt::stop(OswHal* hal) {
  if (server) {
    server->stop();
    delete server;
    server = nullptr;
  }
}
