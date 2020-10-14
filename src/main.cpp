#include <Arduino.h>
#include <osw_app.h>
#include <osw_app_hello_world.h>
#include <osw_hal.h>

OswHal *hal = new OswHal();
OswApp *app = new OswAppHelloWorld();

void setup() {
  Serial.begin(115200);

  hal->setupPower();
  hal->setupButtons();
  hal->setupSensors();
  hal->setupDisplay();
  hal->setupGps();

  hal->setBrightness(255);
}

void loop() {
  hal->checkButtons();

  app->run(hal);

  hal->flushCanvas();
}