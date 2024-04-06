#include "./services/OswServiceTaskConsole.h"
#include "osw_hal.h"

void OswServiceTaskConsole::setup() {
    OswServiceTask::setup();
    OSW_LOG_I("Console is now enabled.");
#ifdef NDEBUG
    this-> m_locked = true // in release mode the console is locked by default
#endif
                      this->newPrompt();
}

void OswServiceTaskConsole::loop() {
    while(true) {
        if(!Serial.available()) break;
        char c = Serial.read();
        if(32 <= c and c <= 126) { // printable characters
            this->m_inputBuffer += c;
            Serial.print(c); // echo the entered character back
        } else if(c == 10 or c == 13) { // CR or LF
            this->runPrompt();
            this->newPrompt();
        } else if(c == 8 or c == 127 or c == 27) { // backspace / delete / escape
            if(this->m_inputBuffer.length() > 0) {
                this->m_inputBuffer.pop_back();
            }
            Serial.print(c);
        } else if(c == 9) { // tab
            Serial.println(); // finish the prompt line
            this->showHelp();
            this->showPrompt();
        } else {
            Serial.print((char) 0x07); // bell
            OSW_LOG_D("Unprocessable character (",(int) c, "): ", c);
        }
    }
}

void OswServiceTaskConsole::newPrompt() {
    this->m_inputBuffer.clear();
    if(this->m_locked) return;
    this->showPrompt();
}

void OswServiceTaskConsole::showPrompt() {
    if (this->m_configuring) {
        Serial.print("OSW (configure) > ");
    } else {
        Serial.print("OSW > ");
    }
    if(!this->m_inputBuffer.empty()) {
        Serial.print(this->m_inputBuffer.c_str());
    }
}

void OswServiceTaskConsole::runPrompt() {
    Serial.println(); // finish the prompt line
    if(this->m_inputBuffer.empty()) return;
    if(this->m_locked) {
        Serial.println("Console is locked! Enter \"unlock\" to unlock.");
        if(this->m_inputBuffer == "unlock") {
            this->m_locked = false;
        }
        return;
    }
    // command convention: show only what was asked (preferably machine readable), only on error be verbose
    bool processed = true;
    if (this->m_configuring) {
        if (this->m_inputBuffer == "clear") {
            OswConfig::getInstance()->reset(false);
        } else if (this->m_inputBuffer == "exit") {
            this->m_configuring = false;
        } else if (this->m_inputBuffer.find("get ") == 0 and this->m_inputBuffer.length() > 4) {
            auto key = this->m_inputBuffer.substr(4);
            for (auto i = 0; i < oswConfigKeysCount; i++) {
                if(oswConfigKeys[i]->id == key) {
                    Serial.println(oswConfigKeys[i]->toString());
                    break;
                }
            }
        } else if (this->m_inputBuffer.find("info ") == 0 and this->m_inputBuffer.length() > 5) {
            auto key = this->m_inputBuffer.substr(5);
            Serial.println(OswConfig::getInstance()->getFieldJson(key.c_str()));
        } else if (this->m_inputBuffer == "help") {
            this->showHelp();
        } else if (this->m_inputBuffer == "list") {
            for (auto i = 0; i < oswConfigKeysCount; i++) {
                Serial.println(oswConfigKeys[i]->id);
            }
        } else if (this->m_inputBuffer.find("reset ") == 0 and this->m_inputBuffer.length() > 7) {
            auto key = this->m_inputBuffer.substr(7);
            OswConfig::getInstance()->enableWrite();
            OswConfig::getInstance()->resetField(key.c_str());
            OswConfig::getInstance()->disableWrite();
        } else if (this->m_inputBuffer.find("set ") == 0 and this->m_inputBuffer.length() > 5) {
            auto key = this->m_inputBuffer.substr(4); // "<id> <value>"
            auto space = key.find(" ", 0);
            if (space == std::string::npos) {
                Serial.println("Invalid command.");
                return;
            }
            auto value = key.substr(space + 1); // "<value>"
            key = key.substr(0, space); // "<id>"
            OswConfig::getInstance()->enableWrite();
            OswConfig::getInstance()->setField(key.c_str(), value.c_str());
            OswConfig::getInstance()->disableWrite();
        } else {
            processed = false;
        }
    } else {
        if (this->m_inputBuffer == "configure") {
            this->m_configuring = true;
        } else if (this->m_inputBuffer == "help") {
            this->showHelp();
        } else if (this->m_inputBuffer == "hostname") {
            Serial.println(OswConfigAllKeys::hostname.get());
        } else if (this->m_inputBuffer == "lock") {
            this->m_locked = true;
        } else if (this->m_inputBuffer == "reboot") {
            ESP.restart();
        } else if (this->m_inputBuffer == "time") {
            Serial.println(OswHal::getInstance()->getUTCTime());
        } else if (this->m_inputBuffer == "wipe") {
            OswConfig::getInstance()->reset(true);
        }  else {
            processed = false;
        }
    }
    // show help if the command was not processed
    if (!processed) {
        Serial.println("Unknown command.");
        Serial.println();
        this->showHelp();
        this->m_lockCounter++;
    }
    // in case of garbage input, lock the console after 16 attempts
    if (this->m_lockCounter > 16) {
        this->m_locked = true;
        this->m_lockCounter = 0;
    }
}

void OswServiceTaskConsole::showHelp() {
    Serial.println("Available commands:");
    // let's try to be civil and show the commands in alphabetical order
    if (this->m_configuring) {
        Serial.println("  clear             - reset all keys to default values");
        Serial.println("  exit              - leave configuration mode");
        Serial.println("  get <id>          - get a value for a key");
        Serial.println("  help              - show this help");
        Serial.println("  info <id>         - show more information about a key");
        Serial.println("  list              - show all keys");
        Serial.println("  reset <id>        - reset a key to default value");
        Serial.println("  set <id> <value>  - set a value for a key (value is everything until the end of the line)");
    } else {
        Serial.println("  configure - enter configuration mode");
        Serial.println("  help      - show this help");
        Serial.println("  hostname  - show the device hostname");
        Serial.println("  lock      - lock the console");
        Serial.println("  reboot    - warm-start the device forcefully");
        Serial.println("  time      - show current UTC time");
        Serial.println("  wipe      - format NVS partition and reset configuration");
    }
}

void OswServiceTaskConsole::stop() {
    OSW_LOG_I("Console is now disabled.");
    OswServiceTask::stop();
}