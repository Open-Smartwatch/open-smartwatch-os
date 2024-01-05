#pragma once

#include "../../../include/apps/OswAppV2Compat.h"

class TestAppV2Compat {
  public:
    static OswApp* getAppV1(OswAppV2Compat& appV2Compat) {
        return &appV2Compat.app;
    }
};