
#include "./apps/_experiments/magnetometer_calibrate.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_ui.h>

#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1 && OSW_PLATFORM_HARDWARE_QMC5883L == 1

void OswAppMagnetometerCalibrate::setup() {

}

// source: https://github.com/mprograms/QMC5883LMagnetometer/blob/master/examples/calibration/calibration.ino
void OswAppMagnetometerCalibrate::loop() {
  static int calibrationData[3][2];
  static bool changed = false;
  static bool done = false;
  static bool running = false;
  static int t = 0;
  static int c = 0;

  int x, y, z;

  // Read compass values
  OswHal* hal = OswHal::getInstance();
  hal->devices->qmc5883l->update();
  hal->gfx()->fill(rgb888to565(OswConfigAllKeys::themeBackgroundColor.get()));

  hal->gfx()->setTextSize(3);
  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextCursor(120, 80);
  hal->gfx()->println(String(hal->devices->qmc5883l->getMagnetometerAzimuth()).c_str());
  hal->gfx()->resetText();

  OswUI::getInstance()->setTextCursor(BUTTON_3);
  if (hal->btnHasGoneDown(BUTTON_3)) {
    running = true;
    done = false;
    changed = false;
  }

  if (!running) {
    hal->gfx()->print("Calibrate");
  } else {
    if (done) {
      hal->gfx()->print("done");
    } else {
      hal->gfx()->print("Calibrating");
    }

    // Return XYZ readings
    x = hal->devices->qmc5883l->getMagnetometerX();
    y = hal->devices->qmc5883l->getMagnetometerY();
    z = hal->devices->qmc5883l->getMagnetometerZ();

    changed = false;

    if (x < calibrationData[0][0]) {
      calibrationData[0][0] = x;
      changed = true;
    }
    if (x > calibrationData[0][1]) {
      calibrationData[0][1] = x;
      changed = true;
    }

    if (y < calibrationData[1][0]) {
      calibrationData[1][0] = y;
      changed = true;
    }
    if (y > calibrationData[1][1]) {
      calibrationData[1][1] = y;
      changed = true;
    }

    if (z < calibrationData[2][0]) {
      calibrationData[2][0] = z;
      changed = true;
    }
    if (z > calibrationData[2][1]) {
      calibrationData[2][1] = z;
      changed = true;
    }

    if (!done) {
      hal->gfx()->setTextCursor(0, 120);
      hal->gfx()->println("CALIBRATING... Keep moving your sensor around.");
    }

    if (changed && !done) {
      c = millis();
    }
    t = millis();

    if ((t - c > 5000) && !done) {
      done = true;
      running = false;
      hal->gfx()->setTextCursor(0, 140);
      hal->gfx()->println("DONE");
      hal->devices->qmc5883l->setMagnetometerCalibration(calibrationData[0][0], calibrationData[0][1], calibrationData[1][0],
                                 calibrationData[1][1], calibrationData[2][0], calibrationData[2][1]);
      //     Serial.print("compass.setCalibration(");
      // Serial.print(calibrationData[0][0]);
      // Serial.print(", ");
      // Serial.print(calibrationData[0][1]);
      // Serial.print(", ");
      // Serial.print(calibrationData[1][0]);
      // Serial.print(", ");
      // Serial.print(calibrationData[1][1]);
      // Serial.print(", ");
      // Serial.print(calibrationData[2][0]);
      // Serial.print(", ");
      // Serial.print(calibrationData[2][1]);
      // Serial.println(");");
    }
  }

  hal->requestFlush();  // request the screen to be updated. refreshrate is handled by main loop
}

void OswAppMagnetometerCalibrate::stop() {

}
#endif