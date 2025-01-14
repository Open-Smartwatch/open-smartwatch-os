#pragma once
#ifdef OSW_FEATURE_BLE_SERVER
#include "osw_service.h"
#include "services/NotifierClient.h"

#define CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#include <NimBLEDevice.h>

class OswServiceTaskBLEServer : public OswServiceTask {
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
    class ServerCallbacks: public NimBLEServerCallbacks {
      public:
        ServerCallbacks(OswServiceTaskBLEServer* task): task(task) {};

      private:
        void onConnect(BLEServer* pServer);
        void onDisconnect(BLEServer* pServer);
        uint32_t onPassKeyRequest();
        bool onConfirmPIN(uint32_t pass_key) {
            return false; // we only report display-only
        };

        OswServiceTaskBLEServer* task;
    };
    class BatteryLevelCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        uint8_t byte = 0; // will be read from
    };
    class BatteryLevelStatusCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        uint8_t bytes[1+2+1]; // will be read from (1 flag, 2 power-state, 1 battery-level)
    };
    class CurrentTimeCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        uint8_t bytes[9+1]; // will be read from (9 exact-time-256, 1 reason)
    };
    class FirmwareRevisionCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        String value = "Open-Smartwatch OS";
    };
    class HardwareRevisionCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        String value;
    };
    class SoftwareRevisionCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        String value;
    };
    class ToastCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
      public:
        ToastCharacteristicCallbacks(OswServiceTaskBLEServer* task): task(task) {};

      private:
        void onWrite(NimBLECharacteristic* pCharacteristic);

        OswServiceTaskBLEServer* task;
    };
    class StepsTodayCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        uint8_t bytes[4]; // this is a 4-byte array of a uint_32_t number
    };
    class StepsTotalWeekCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        uint8_t bytes[4]; // this is a 4-byte array of a uint_32_t number
    };
    class StepsTotalCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        uint8_t bytes[4]; // this is a 4-byte array of a uint_32_t number
    };
    class StepsAverageCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        uint8_t bytes[4]; // this is a 4-byte array of a uint_32_t number
    };
    class StepsDayHistoryCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic);
        uint8_t bytes[4 * 7]; // this is a 28-byte array of seven uint_32_t numbers
    };


    /// apply the desired BLE state
    void updateBLEConfig();

    // ↓ managed by NimBLE
    NimBLEServer* server = nullptr; // if set, this is considered as "enabled"
    NimBLEService* serviceBat = nullptr;
    NimBLECharacteristic* characteristicBat = nullptr;
    NimBLECharacteristic* characteristicBatStat = nullptr;
    NimBLEService* serviceTime = nullptr;
    NimBLECharacteristic* characteristicCurTime = nullptr;
    NimBLEService* serviceDevice = nullptr;
    NimBLECharacteristic* characteristicFirmRev = nullptr;
    NimBLECharacteristic* characteristicHardRev = nullptr;
    NimBLECharacteristic* characteristicSoftRev = nullptr;
    NimBLEService* serviceOsw = nullptr;
    NimBLECharacteristic* characteristicToast = nullptr;
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    NimBLECharacteristic* characteristicStepCountTotal = nullptr;
    NimBLECharacteristic* characteristicStepCountTotalWeek = nullptr;
    NimBLECharacteristic* characteristicStepCountToday = nullptr;
#ifdef OSW_FEATURE_STATS_STEPS
    NimBLECharacteristic* characteristicStepCountAverage = nullptr;
    NimBLECharacteristic* characteristicStepCountHistory = nullptr;
#endif
#endif

    // ↓ our own stuff
    NotifierClient notify = NotifierClient("osw.ble.server");
    bool bootDone = false;
    bool enabled = false;
    char name[8]; // BLE advertising only support up to 8 bytes
};
#endif
