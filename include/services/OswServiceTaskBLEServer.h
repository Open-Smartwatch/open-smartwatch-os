#pragma once
#ifdef OSW_FEATURE_BLE_SERVER
#include "osw_service.h"

#define CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#include <NimBLEDevice.h>

class OswServiceTaskBLEServer : public OswServiceTask, NimBLEServerCallbacks {
  public:
    OswServiceTaskBLEServer() {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswServiceTaskBLEServer() {};

    void enable();
    void disable();
    bool isEnabled();

    void updateName();
  private:
    class BatteryCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        uint8_t byte = 0; // will be read from
    };

    /// apply the desired BLE state
    void updateBLEConfig();

    // ↓ NimBLEServerCallbacks
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
    uint32_t onPassKeyRequest();
    bool onConfirmPIN(uint32_t pass_key);

    // ↓ managed by NimBLE
    NimBLEServer* server = nullptr; // if set, this is considered as "enabled"
    BLEService* serviceBat = nullptr;
    NimBLECharacteristic* characteristicBat = nullptr;

    // ↓ our own stuff
    bool bootDone = false;
    bool enabled = false;
    char name[8]; // BLE advertising only support up to 8 bytes
    BatteryCharacteristicCallbacks battery;
};
#endif