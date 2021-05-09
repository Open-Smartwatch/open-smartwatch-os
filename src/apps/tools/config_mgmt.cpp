
#include "./apps/tools/config_mgmt.h"

#include <WebServer.h>
#include <assets/bundle.min.css.gz.h>
#include <assets/bundle.min.js.gz.h>
#include <assets/index.html.gz.h>
#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_types.h>
#include <osw_hal.h>

WebServer* server;

void handleIndex() {
#if defined(DEBUG)
  Serial.println("CFGSERVER: /");
#endif
  server->sendHeader(F("Content-Encoding"), F("gzip"));
  server->send_P(200, "text/html", (const char*)dist_data_index_html_gz, dist_data_index_html_gz_len);
}

void handleCss() {
#if defined(DEBUG)
  Serial.println("CFGSERVER: /css");
#endif
  server->sendHeader(F("Content-Encoding"), F("gzip"));
  server->send_P(200, "text/css", (const char*)dist_data_bundle_min_css_gz, dist_data_bundle_min_css_gz_len);
}

void handleJs() {
#if defined(DEBUG)
  Serial.println("CFGSERVER: /js");
#endif
  server->sendHeader(F("Content-Encoding"), F("gzip"));
  server->send_P(200, "application/javascript", (const char*)dist_data_bundle_min_js_gz,
                 dist_data_bundle_min_js_gz_len);
}

void handleConfigJson() {  //
#if defined(DEBUG)
  Serial.println("CFGSERVER: /config.json");
#endif
  server->send(200, "application/json", OswConfig::getInstance()->getConfigJSON());
}

void handleDataJson() {
#if defined(DEBUG)
  Serial.println("CFGSERVER: /data.json");
#endif
  if (server->hasArg("plain") == false) {
    server->send(422, "application/json", "{\"error\": \"CFG_MISSING\"}");
    return;
  }

  OswConfig::getInstance()->enableWrite();
  OswConfig::getInstance()->parseDataJSON(server->arg("plain").c_str());
  OswConfig::getInstance()->disableWrite();

  // TODO: error handling?
  server->send(200, "application/json", "{\"success\":true}");
}

void OswAppConfigMgmt::setup(OswHal* hal) {}

void OswAppConfigMgmt::loop(OswHal* hal) {
  static bool lateSetup = false;

  hal->gfx()->fill(rgb565(0, 0, 0));
  hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
  hal->gfx()->setTextSize(2);

  if (hal->btnHasGoneDown(BUTTON_3)) {
    if (!lateSetup) {
      lateSetup = true;
      hal->getWiFi()->joinWifi();

      if (hal->getWiFi()->isConnected()) {
        server = new WebServer(80);
        server->on("/", handleIndex);
        server->on("/index.html", handleIndex);
        server->on("/bundle.min.css", handleCss);
        server->on("/bundle.min.js", handleJs);
        server->on("/config.json", handleConfigJson);
        server->on("/data.json", HTTP_POST, handleDataJson);
        server->begin();
      }
    }
  }

  hal->gfx()->setTextCursorBtn3();
  if (!lateSetup) {
    hal->gfx()->print("Start");
  }

  if (hal->getWiFi()->isConnected()) {
    hal->getWiFi()->checkWifi();
    server->handleClient();
    hal->gfx()->setTextCursor(120, 120);
    hal->gfx()->setTextColor(rgb565(0, 255, 0), rgb565(100, 100, 100));
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->print(hal->getWiFi()->getIp().toString());
  }

  hal->requestFlush();
}

void OswAppConfigMgmt::stop(OswHal* hal) {
  server->stop();
  delete server;
}
