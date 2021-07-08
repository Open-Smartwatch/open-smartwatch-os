#include "./apps/main/OswAppWebserver.h"
#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <osw_ui.h>

#include <services/OswServiceTasks.h>
#include <services/OswServiceTaskWiFi.h>
#include <services/OswServiceTaskWebserver.h>

void OswAppWebserver::setup(OswHal* hal) {

}

void OswAppWebserver::loop(OswHal* hal) {
  hal->gfx()->fill(ui->getBackgroundColor());
  hal->gfx()->setTextSize(2);

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
    hal->gfx()->setTextCursor(120, OswServiceAllTasks::wifi.isStationEnabled() ? 60 : 90);
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextColor(ui->getPrimaryColor(), ui->getBackgroundColor());
    hal->gfx()->println("IP:");
    hal->gfx()->setTextSize(2);
    hal->gfx()->println(OswServiceAllTasks::wifi.getIP().toString());
    if(OswServiceAllTasks::wifi.isStationEnabled()) {
      hal->gfx()->setTextSize(1);
      hal->gfx()->setTextColor(ui->getInfoColor(), ui->getBackgroundColor());
      hal->gfx()->println("Station Password:");
      hal->gfx()->setTextSize(2);
      hal->gfx()->println(OswServiceAllTasks::wifi.getStationPassword());
    }
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextColor(ui->getWarningColor(), ui->getBackgroundColor());
    hal->gfx()->println("User:");
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextColor(ui->getDangerColor(), ui->getBackgroundColor());
    hal->gfx()->println("admin");
    hal->gfx()->setTextSize(1);
    hal->gfx()->println("Password:");
    hal->gfx()->setTextSize(2);
    hal->gfx()->println(OswServiceAllTasks::webserver.getPassword());
    hal->gfx()->setTextColor(ui->getForegroundColor(), ui->getBackgroundColor());

  } else {
    hal->gfx()->setTextCursor(120, 120);
    hal->gfx()->print("Configuration UI");
  }

  hal->requestFlush();
}

void OswAppWebserver::stop(OswHal* hal) {
}
