#include "./services/OswServiceTaskBLECompanion.h"
#include "osw_hal.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLECharacteristic.h>
#include <ArduinoJson.h>

#include <string.h>
#include "config_defaults.h"

#ifdef OSW_FEATURE_WIFI
#if SERVICE_BLE_COMPANION == 1
#error "The RAM on all current OSW models is not big enough to hold both WiFi AND Bluetooth stacks during runtime. This WILL lead to a crash. Please only use one of these features simultaneously!"
#endif
#endif

class NotificationCallback: public BLECharacteristicCallbacks {
    public:
        NotificationCallback(OswServiceTaskBLECompanion *comp) {
            companion = comp;
        }

        virtual ~NotificationCallback() {};

        virtual void onRead(BLECharacteristic* pCharacteristic) {};
        virtual void onWrite(BLECharacteristic* pCharacteristic) {
            //Parse message as JSON object
            Serial.println(pCharacteristic->getValue().c_str());
            DynamicJsonDocument doc(5000); //ArduinoJSON no longer allows Dynamic allocation to grow

            deserializeJson(doc, pCharacteristic->getValue());

            //Pull values from JSON
            const char *app = "", *contents = "";
            unsigned int uid = 0;

            if (doc.containsKey("app"))
                app = doc["app"];

            if (doc.containsKey("contents"))
                contents = doc["contents"];

            if (doc.containsKey("uid"))
                uid = doc["uid"];

            NotificationDetails details {uid, app, contents};

            //Notify our client about the new notificaiton
            if (companion->notificationCallback) {
                companion->notificationCallback(details);
            }
        };

        virtual void onNotify(BLECharacteristic* pCharacteristic) {};
        virtual void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) {};

    private:
        OswServiceTaskBLECompanion *companion;
};

void OswServiceTaskBLECompanion::setup() {
    OswServiceTask::setup();
    BLEDevice::init(BLE_DEVICE_NAME);
    bleServer = BLEDevice::createServer();
    notificationService = bleServer->createService(NOTIFICATION_SERVICE_UID);

    notificationChar = notificationService->createCharacteristic(NOTIFICATION_BRDCST_CHAR, BLECharacteristic::PROPERTY_WRITE);
    notificationChar->setCallbacks(new NotificationCallback(this));

    notificationService->start();

    bleServer->getAdvertising()->addServiceUUID(notificationService->getUUID());
}

void OswServiceTaskBLECompanion::startAdvertising() {
    bleServer->startAdvertising();
}

void OswServiceTaskBLECompanion::stopAdvertising() {
    bleServer->getAdvertising()->stop();
}

void OswServiceTaskBLECompanion::setNotificationCallback(std::function<void(NotificationDetails)> cb) {
    notificationCallback = cb;
}

void OswServiceTaskBLECompanion::loop() {

}

void OswServiceTaskBLECompanion::stop() {
    OswServiceTask::stop();
}
