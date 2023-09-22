#pragma once

#include <OswAppV2.h>
#include <osw_ui.h>

class OswAppV2Fixture {
  public:
    OswAppV2Fixture(OswAppV2& app): app(app) {
        this->original = OswUI::getInstance()->getRootApplication();
        OswUI::getInstance()->setRootApplication(&app);
    }

    ~OswAppV2Fixture() {
        OswUI::getInstance()->setRootApplication(original);
    }

    OswAppV2& getApp() {
        return app;
    }
  private:
    OswAppV2* original = nullptr;
    OswAppV2& app;
};