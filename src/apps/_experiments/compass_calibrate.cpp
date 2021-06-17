
#include "./apps/_experiments/compass_calibrate.h"
#ifdef GPS_EDITION

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_ui.h>
// define global scope variables

void OswAppCompassCalibrate::setup(OswHal* hal) {
  // this is where you initialise code, gets called when this app is shown
  hal->setupCompass();
}

// source: https://github.com/mprograms/QMC5883LCompass/blob/master/examples/calibration/calibration.ino
void OswAppCompassCalibrate::loop(OswHal* hal) {
  static int calibrationData[3][2];
  static bool changed = false;
  static bool done = false;
  static bool running = false;
  static int t = 0;
  static int c = 0;

  int x, y, z;

  // Read compass values
  hal->updateCompass();
  hal->gfx()->fill(rgb888to565(OswConfigAllKeys::themeBackgroundColor.get()));

  hal->gfx()->setTextSize(3);
  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextCursor(120, 80);
  hal->gfx()->println(String(hal->getCompassAzimuth()).c_str());
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
    x = hal->getCompassX();
    y = hal->getCompassY();
    z = hal->getCompassZ();

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
      hal->setCompassCalibration(calibrationData[0][0], calibrationData[0][1], calibrationData[1][0],
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

void OswAppCompassCalibrate::stop(OswHal* hal) {
  // this is where you de-initialize stuff, gets called when another app is shown
  hal->stopCompass();
}

#endif
