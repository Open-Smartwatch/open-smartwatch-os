#include "./services/OswServiceTaskRawScreenServer.h"

#include <WebServer.h>
#include <services/OswServiceTaskWiFi.h>
#include <services/OswServiceTasks.h>

#include "osw_hal.h"

// fixme: this is a concept that is still broken
// the idea is to have a web api to pull raw screen bitmaps for screen shots and documentation

WebServer* rawScreenServer = nullptr;
Graphics2D* rawScreenGfx = nullptr;

void handleRawScreen() {
  long contentLength = rawScreenGfx->getWidth() * rawScreenGfx->getHeight() * 3;
  rawScreenServer->client().write("HTTP/1.1 200 OK");
  rawScreenServer->client().write((String("\r\nContent-Length: ") + String(contentLength)).c_str());
  rawScreenServer->client().write("\r\nContent-Type: application/octet-stream");
  rawScreenServer->client().write("\r\nConnection: close");

  for (int y = 0; y < rawScreenGfx->getHeight(); y++) {
    for (int x = 0; x < rawScreenGfx->getWidth(); x++) {
      uint16_t rgb = rawScreenGfx->getPixel(x, y);
      uint8_t r = rgb565_red(rgb);
      uint8_t g = rgb565_green(rgb);
      uint8_t b = rgb565_blue(rgb);
      rawScreenServer->client().write(r);
      rawScreenServer->client().write(g);
      rawScreenServer->client().write(b);
    }
  }
}

void OswServiceTaskRawScreenServer::setup(OswHal* hal) {
  OswServiceTask::setup(hal);

  if (!OswServiceAllTasks::wifi.isConnected()) {
    OswServiceAllTasks::wifi.connectWiFi();
  }


  if (!rawScreenGfx) {
    rawScreenGfx = hal->gfx();
  }
}

void OswServiceTaskRawScreenServer::loop(OswHal* hal) {
    if (!rawScreenServer && OswServiceAllTasks::wifi.isConnected()) {
      rawScreenServer = new WebServer(8080);
      rawScreenServer->on("/screen.raw", [] { handleRawScreen(); });
      rawScreenServer->begin();
  #ifdef DEBUG
      Serial.println("Started Raw ScreenServer");
      Serial.print("http://");
      Serial.print(OswServiceAllTasks::wifi.getIP().toString());
      Serial.println(":8080/screen.raw");
  #endif
    }

    if (rawScreenServer) {
      rawScreenServer->handleClient();
    }
}

void OswServiceTaskRawScreenServer::stop(OswHal* hal) {
  OswServiceTask::stop(hal);

  OswServiceAllTasks::wifi.disconnectWiFi();
}
