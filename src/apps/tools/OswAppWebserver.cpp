#ifdef OSW_FEATURE_WIFI
#include "./apps/tools/OswAppWebserver.h"

#include <config.h>
#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <osw_ui.h>
#include <services/OswServiceTaskWebserver.h>
#include <services/OswServiceTaskWiFi.h>
#include <services/OswServiceTasks.h>

void OswAppWebserver::setup() {}

void OswAppWebserver::loop() {
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->setTextSize(2);

    // Configuration
    OswUI::getInstance()->setTextCursor(BUTTON_UP);
    if (OswServiceAllTasks::wifi.isConnected()) {
        hal->gfx()->print(LANG_DISCONNECT);
    } else {
        if(OswServiceAllTasks::wifi.isEnabled())
            hal->gfx()->print("...");
        else {
            hal->gfx()->print(LANG_CONNECT);
            OswUI::getInstance()->setTextCursor(BUTTON_DOWN);
            if(OswConfigAllKeys::hostPasswordEnabled.get()) {
                hal->gfx()->print(LANG_WEBSRV_AP_PASSWORD_ON);
            } else {
                hal->gfx()->print(LANG_WEBSRV_AP_PASSWORD_OFF);
            }
        }
    }

    if (hal->btnHasGoneDown(BUTTON_UP)) {
        if (OswServiceAllTasks::wifi.isConnected()) {
            OswServiceAllTasks::wifi.disconnectWiFi();
            OswServiceAllTasks::wifi.disableWiFi();
        } else {
            OswServiceAllTasks::wifi.enableWiFi();
            OswServiceAllTasks::wifi.connectWiFi();
        }
    }
    if (hal->btnHasGoneDown(BUTTON_DOWN)) {
        if (!OswServiceAllTasks::wifi.isConnected()) {
            OswServiceAllTasks::wifi.toggleAPPassword();
        }
    }

    drawConnectionInfo();
}

void OswAppWebserver::drawConnectionInfo() {
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextMiddleAligned();

    if (OswServiceAllTasks::wifi.isConnected()) { // Wi-Fi connect info
        hal->gfx()->setTextCursor(120, OswServiceAllTasks::wifi.isStationEnabled() ? 60 : 90);
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextColor(ui->getPrimaryColor(), ui->getBackgroundColor());
        hal->gfx()->println("IP:");
        hal->gfx()->setTextSize(2);
        hal->gfx()->println(OswServiceAllTasks::wifi.getIP().toString());
        if (OswServiceAllTasks::wifi.isStationEnabled()) {
            hal->gfx()->setTextSize(1);
            hal->gfx()->setTextColor(ui->getInfoColor(), ui->getBackgroundColor());
            hal->gfx()->println(LANG_WEBSRV_STATION_PWD);
            hal->gfx()->setTextSize(2);
            const String pwd = OswServiceAllTasks::wifi.getStationPassword();
            hal->gfx()->println(pwd.isEmpty() ? "-" : pwd);
        }
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextColor(ui->getWarningColor(), ui->getBackgroundColor());
        hal->gfx()->println(LANG_WEBSRV_USER);
        hal->gfx()->setTextSize(2);
        hal->gfx()->setTextColor(ui->getDangerColor(), ui->getBackgroundColor());
        hal->gfx()->println("admin");
        hal->gfx()->setTextSize(1);
        hal->gfx()->println(LANG_WEBSRV_PASS);
        hal->gfx()->setTextSize(2);
        hal->gfx()->println(OswServiceAllTasks::webserver.getPassword());
        hal->gfx()->setTextColor(ui->getForegroundColor(), ui->getBackgroundColor());

    } else { // No connect
        hal->gfx()->setTextCursor(120, 120);
        hal->gfx()->print(LANG_WEBSRV_TITLE);
    }

}

void OswAppWebserver::stop() {}
#endif
