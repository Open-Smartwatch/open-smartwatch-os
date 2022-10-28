#include "./apps/tools/OswAppToolsPage2App.h"

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

void OswAppToolsPage2App::setup() {}

void OswAppToolsPage2App::loop() {
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->setTextSize(2);

     hal->gfx()->setTextCursor(40,60);
    hal->gfx()->print("Tools Page 2");
    
    hal->requestFlush();
}

void OswAppToolsPage2App::stop() {}
