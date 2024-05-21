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
    void newPrompt();
    void showPrompt();
    void runPrompt();
    void showHelp();

    std::string m_inputBuffer;
    bool m_locked = false;
    unsigned char m_lockCounter = 0;
    bool m_configuring = false;
};
