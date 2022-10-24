#include "./apps/tools/OswAppToolsApp.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <osw_ui.h>
#include <services/OswServiceTaskWebserver.h>
#include <services/OswServiceTaskWiFi.h>
#include <services/OswServiceTasks.h>

void OswAppToolsApp::setup() {}

void OswAppToolsApp::loop() {
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->setTextSize(2);

    hal->gfx()->setTextCursor(40,60);
    hal->gfx()->print("Tools Page 1");

    hal->requestFlush();
}

void OswAppToolsApp::stop() {}
