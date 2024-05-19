#pragma once
#include "osw_service.h"

class OswServiceTaskConsole : public OswServiceTask {
  public:
    OswServiceTaskConsole() {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswServiceTaskConsole() {};

  private:
    enum class Mode {
        Generic,
        Configuration,
#ifdef OSW_FEATURE_BLE_SERVER
        Ble
#endif
    };

    void newPrompt();
    void showPrompt();
    void runPrompt();
    void showHelp();

    std::string m_inputBuffer;
    bool m_locked = false;
    unsigned char m_lockCounter = 0;
    Mode m_mode = Mode::Generic;
};
