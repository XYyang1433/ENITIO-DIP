#include <NimBLEDevice.h>

/**  Constants **/
// This Service UUID is broadcasted when the level 2 treasure is available
#define availableServiceUUID NimBLEUUID("b8fc9a62-5c8b-4bd4-9f00-c05b558dc9ee")
#define availableCharacteristicUUID NimBLEUUID("121153c0-1a5a-43cb-9fe2-240b22dc5958")

// This Service UUID is broadcasted when a virus is currently emitted to all surrounding treasures
// The client device should trigger the virus mechanism when this advertising service is observed
#define virusServiceUUID NimBLEUUID("6059772b-1f14-4557-b467-8a9ed23032ac")
#define virusCharacteristicUUID NimBLEUUID("f978ba96-b15e-4c54-b5e5-817d15839c38")

NimBLEServer* pServer;
NimBLEAdvertising* pAdvertising;
NimBLEService* pAvailableService;
NimBLECharacteristic* pAvailableCharacteristic;
NimBLEService* pVirusService;
NimBLECharacteristic* pVirusCharacteristic;

class TreasureBluetooth {
    private:
        String treasureName;

    public:
        void initialise(int treasureID) {
            treasureName = "TREASURE" + String(treasureID);

            NimBLEDevice::init(treasureName.c_str());

            // Optional: set the transmit power, default is 3db
            NimBLEDevice::setPower(ESP_PWR_LVL_N9); /** -9db */

            pServer = NimBLEDevice::createServer();

            Serial.print(treasureName); Serial.println(" Initialised"); Serial.println(treasureID);

            // Create Available Service
            pAvailableService = pServer->createService(availableServiceUUID);
            pAvailableCharacteristic = pAvailableService->createCharacteristic(
                                          availableCharacteristicUUID,
                                          NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
                                        );
            pAvailableService->start();
          
            // Create Virus Service
            pVirusService = pServer->createService(virusServiceUUID);
            pVirusCharacteristic = pVirusService->createCharacteristic(
                                      virusCharacteristicUUID,
                                      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
                                    );
            pVirusService->start();

            // Prepare Advertising
            pAdvertising = NimBLEDevice::getAdvertising();
            
            /** If your device is battery powered you may consider setting scan response
             *  to false as it will extend battery life at the expense of less data sent.
            */
            pAdvertising->setScanResponse(true);
        };

        String getTreasureName() {
            return treasureName;
        }

        void startAdvertisingService(NimBLEService* targetService) {
            pAdvertising->addServiceUUID(targetService->getUUID());
            pAdvertising->start();
        };

        void stopAdvertisingService(NimBLEService* targetService) {
            pAdvertising->stop();
            pAdvertising->removeServiceUUID(targetService->getUUID());
        };
};

TreasureBluetooth TreasureLevel2_Bluetooth;
