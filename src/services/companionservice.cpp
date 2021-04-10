#include "./services/companionservice.h"
#include "osw_hal.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLECharacteristic.h>
#include <ArduinoJson.h>

#include <string.h>
#include "config.h"


class NotificationCallback: public BLECharacteristicCallbacks {
    public:
        NotificationCallback(OswServiceCompanion *comp) {
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
        OswServiceCompanion *companion;
};

void OswServiceCompanion::setup(OswHal* hal) {
    BLEDevice::init(BLE_DEVICE_NAME);
    bleServer = BLEDevice::createServer();
    notificationService = bleServer->createService(NOTIFICATION_SERVICE_UID);

    notificationChar = notificationService->createCharacteristic(NOTIFICATION_BRDCST_CHAR, BLECharacteristic::PROPERTY_WRITE);
    notificationChar->setCallbacks(new NotificationCallback(this));

    notificationService->start();

    bleServer->getAdvertising()->addServiceUUID(notificationService->getUUID());
}

void OswServiceCompanion::startAdvertising() {
    bleServer->startAdvertising();
}

void OswServiceCompanion::stopAdvertising() {
    bleServer->getAdvertising()->stop();
}

void OswServiceCompanion::setNotificationCallback(std::function<void(NotificationDetails)> cb) {
    notificationCallback = cb;
}

void OswServiceCompanion::loop(OswHal* hal) {
    
}

void OswServiceCompanion::stop(OswHal* hal) {
    
}