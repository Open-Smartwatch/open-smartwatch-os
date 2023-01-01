#ifndef NDEBUG

#include "./apps/tools/OswAppPrintDebug.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>
#include "services/OswServiceTasks.h"
#ifdef OSW_FEATURE_WIFI
#include <services/OswServiceTaskWiFi.h>
#endif

#define SERIAL_BUF_SIZE 14

void OswAppPrintDebug::printStatus(const char* setting, const char* value) {
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->setTextCursor(x, y);
    hal->gfx()->print(setting);
    hal->gfx()->print(": ");
    hal->gfx()->print(value);
    y += 8;
}

void OswAppPrintDebug::setup() {
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    OswHal::getInstance()->gpsFullOnGpsGlonassBeidu();
#endif
}
void OswAppPrintDebug::loop() {
    static String serialBuffer[SERIAL_BUF_SIZE];
    OswHal* hal = OswHal::getInstance();

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    if (hal->btnHasGoneDown(BUTTON_2)) {
        hal->setDebugGPS(!hal->isDebugGPS());
    }
#endif

    uint8_t yOrig = y;
#ifndef OSW_EMULATOR
    printStatus("RAM", (String(ESP.getHeapSize() - ESP.getFreeHeap()) + "B / " + ESP.getHeapSize() + "B").c_str());
#endif
#ifdef BOARD_HAS_PSRAM
    printStatus("PSRAM", (String(ESP.getPsramSize() - ESP.getFreePsram()) + "B / " + String(ESP.getPsramSize()) + "B").c_str());
#endif

#if OSW_PLATFORM_ENVIRONMENT == 1
#if OSW_PLATFORM_ENVIRONMENT_TEMPERATURE == 1
    printStatus("Temperature", String(hal->environment->getTemperature() + String("C")).c_str());
    for(auto& d : *OswTemperatureProvider::getAllTemperatureDevices())
        printStatus((String("  ") + d->getName()).c_str(), String(d->getTemperature() + String("C")).c_str());
#endif
#if OSW_PLATFORM_ENVIRONMENT_PRESSURE == 1
    printStatus("Pressure", String(hal->environment->getPressure() + String(" bar")).c_str());
    for(auto& d : *OswPressureProvider::getAllPressureDevices())
        printStatus((String("  ") + d->getName()).c_str(), String(d->getPressure() + String(" bar")).c_str());
#endif
#if OSW_PLATFORM_ENVIRONMENT_HUMIDITY == 1
    printStatus("Humidity", String(hal->environment->getHumidity() + String("%")).c_str());
    for(auto& d : *OswHumidityProvider::getAllHumidityDevices())
        printStatus((String("  ") + d->getName()).c_str(), String(d->getHumidity() + String("%")).c_str());
#endif
#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1
    printStatus("Magnetometer", String(String(hal->environment->getMagnetometerAzimuth()) + " deg").c_str());
    for(auto& d : *OswMagnetometerProvider::getAllMagnetometerDevices())
        printStatus((String("  ") + d->getName()).c_str(), String(d->getMagnetometerAzimuth() + String(" deg")).c_str());
    // Idea: Also print azimuth, bearing or calibration
#endif
#endif
    printStatus("UTC Time", String(String(hal->getUTCTime()) + " sec").c_str());
    for(auto& d : *OswTimeProvider::getAllTimeDevices())
        printStatus((String("  ") + d->getName()).c_str(), String(d->getUTCTime() + String(" sec")).c_str());
    printStatus("Charging", hal->isCharging() ? "Yes" : "No");
    bool wifiDisabled = true;
#ifdef OSW_FEATURE_WIFI
    wifiDisabled = !OswServiceAllTasks::wifi.isEnabled();
#endif
    printStatus("Battery (Analog)", (wifiDisabled ? String(hal->getBatteryRaw()) : String("WiFi active!")).c_str());
    String branchName(GIT_BRANCH_NAME);
    printStatus("Hash", (String(GIT_COMMIT_HASH) + " (" + (branchName.length() > 10 ? branchName.substring(0, 10) + "..." : branchName) + ")").c_str());
    printStatus("Platform", String(PIO_ENV_NAME).c_str());
    printStatus("Compiled", (String(__DATE__) + " " + String(__TIME__)).c_str());

    printStatus("Button 1", hal->btnIsDown(BUTTON_1) ? "DOWN" : "UP");
    printStatus("Button 2", hal->btnIsDown(BUTTON_2) ? "DOWN" : "UP");
    printStatus("Button 3", hal->btnIsDown(BUTTON_3) ? "DOWN" : "UP");

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    static uint8_t serialPtr = 0;

    printStatus("MicroSD present", hal->hasSD() ? "OK" : "missing");
    printStatus("MicroSD mounted", hal->isSDMounted() ? "OK" : "NO");
    printStatus("MicroSD (MB)",
                hal->isSDMounted() ? String(String((uint)(hal->sdCardSize() / (1024 * 1024))) + " MB").c_str() : "N/A");

    printStatus("GPS:", hal->hasGPS() ? "OK" : "missing");
    printStatus("Satellites: ", String(hal->gpsNumSatellites()).c_str());

    printStatus("Latitude", String(hal->gpsLat()).c_str());
    printStatus("Longitude", String(hal->gpsLon()).c_str());
    if (hal->isDebugGPS()) {
        while (hal->getSerialGPS().available()) {
            String line = hal->getSerialGPS().readStringUntil('\n');
            OSW_LOG_D(line);
            serialBuffer[serialPtr] = line;

            if (serialBuffer[serialPtr][0] == '$'     //
                    && serialBuffer[serialPtr][1] == 'G'  //
                    && serialBuffer[serialPtr][2] == 'P'  //
                    && serialBuffer[serialPtr][3] == 'G'  //
                    && serialBuffer[serialPtr][4] == 'L'  //
                    && serialBuffer[serialPtr][5] == 'L') {
                OSW_LOG_D(serialPtr, ">>>");
                serialPtr = 0;
            } else {
                serialPtr++;
                serialPtr = serialPtr >= SERIAL_BUF_SIZE ? 0 : serialPtr;
            }
        }

        hal->gfx()->setTextCursor(0, 120);
        for (uint8_t i = 0; i < SERIAL_BUF_SIZE; i++) {
            y += 8;
            hal->gfx()->setTextCursor(0, y);
            hal->gfx()->println(serialBuffer[i]);
        }
    }
#endif

    y = yOrig;
    hal->requestFlush();
}

#ifdef OSW_EMULATOR
#include "imgui.h"

void OswAppPrintDebug::loopDebug() {
    ImGui::Begin("Debug: OswAppPrintDebug");
    ImGui::InputScalar("x", ImGuiDataType_U8, &this->x);
    ImGui::InputScalar("y", ImGuiDataType_U8, &this->y);
    ImGui::End();
}
#endif

void OswAppPrintDebug::stop() {
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    OswHal::getInstance()->gpsBackupMode();
#endif
}
#endif