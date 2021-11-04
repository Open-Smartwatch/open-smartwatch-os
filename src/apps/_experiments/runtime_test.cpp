
#ifdef OSW_FEATURE_WIFI
#include "./apps/_experiments/runtime_test.h"

#include <config.h>
#include <gfx_util.h>
#include <mini-iot-client.h>
#include <osw_app.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

void OswAppRuntimeTest::setup() {
  String wifiSsid = OswConfigAllKeys::wifiSsid.get();
  String wifiPass = OswConfigAllKeys::wifiPass.get();
  miniIot = new MiniIotClient(MINI_IOT_DEVICENAME, MINI_IOT_SERVER, wifiSsid.c_str(), wifiPass.c_str());
}
void OswAppRuntimeTest::loop() {
  static long lastSend = millis();
  static uint32_t batteryRaw = 0;
  static boolean readBattery = true;
  OswHal* hal = OswHal::getInstance();

  if (readBattery) {
    readBattery = false;

    uint8_t numSamples = 20;

    // measure X times
    for (uint8_t i = 0; i < numSamples; i++) {
      batteryRaw = batteryRaw + hal->getBatteryRaw();
    }

    // return average
    batteryRaw = batteryRaw / numSamples;

    Serial.print("Raw: ");
    Serial.print(batteryRaw);
  }

  uint32_t runtimeSeconds = (millis() - lastSend) / 1000;
  Serial.println(runtimeSeconds);
  hal->gfx()->fill(rgb565(0, 0, 0));
  hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
  hal->gfx()->setTextCursor(116, 100);
  hal->gfx()->setTextSize(2);
  hal->gfx()->print(runtimeSeconds);
  hal->requestFlush();

  hal->setCPUClock(80);
  if (runtimeSeconds >= 8) {
    miniIot->setDebugStream(&Serial);
    // upload data
    miniIot->checkWifi();
    miniIot->appendWithTimestamp("battery.csv", String(String(batteryRaw)));
    delay(500);

    hal->deepSleep(15 * 60 * 1000);
  }
}
void OswAppRuntimeTest::stop() {
  delete miniIot;
  miniIot = nullptr;
}
#endif