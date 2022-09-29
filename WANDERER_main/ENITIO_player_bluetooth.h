#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>


/**  Constants **/
// This Service UUID is broadcasted when the level 2 treasure is available
#define availableServiceUUID NimBLEUUID("b8fc9a62-5c8b-4bd4-9f00-c05b558dc9ee")
#define availableCharacteristicUUID NimBLEUUID("121153c0-1a5a-43cb-9fe2-240b22dc5958")

// This Service UUID is broadcasted when a virus is currently emitted to all surrounding treasures
// The client device should trigger the virus mechanism when this advertising service is observed
#define virusServiceUUID NimBLEUUID("6059772b-1f14-4557-b467-8a9ed23032ac")
#define virusCharacteristicUUID NimBLEUUID("f978ba96-b15e-4c54-b5e5-817d15839c38")

// BLE Variables
NimBLEScan *pScan;
NimBLEServer* pServer;
NimBLEAdvertising* pAdvertising;
NimBLEService* pVirusService;
NimBLECharacteristic* pVirusCharacteristic;
int scanTime = 2;


class PlayerBluetooth {
    public:
        bool isThereVirus = false;
        void initialise() {
            NimBLEDevice::init("TestClient");
            pScan = NimBLEDevice::getScan();
            pScan->setActiveScan(true);
            pServer = NimBLEDevice::createServer();
            pAdvertising = NimBLEDevice::getAdvertising();
            pAdvertising->setScanResponse(true);
            pVirusService = pServer->createService(virusServiceUUID);
            pVirusCharacteristic = pVirusService->createCharacteristic(
                                      virusCharacteristicUUID,
                                      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
                                    );
            pVirusService->start();
        };
        void scan() {
            // Returns all devices found after scanTime seconds
            NimBLEScanResults results = pScan->start(2, false);
            int virus_device_counter = 0;
            for (int i=0; i < results.getCount(); i++) {
                NimBLEAdvertisedDevice device = results.getDevice(i);
                if (device.haveServiceUUID()) {
                    if (device.isAdvertisingService(virusServiceUUID)) {
                        // process virus
                        Serial.println("Infected with Virus!");
                        Serial.println(device.toString().c_str());
                        isThereVirus = true;
                        virus_device_counter++;
                    } else if (device.isAdvertisingService(availableServiceUUID)) {
                        // available treasure nearby
                        Serial.println("BLE Server Available");
                        Serial.println(device.toString().c_str());
                    }
                }
            }
            if (virus_device_counter == 0) isThereVirus = false;
            
        };

        void startSpreadingVirus() {
            Serial.println("Setting UUID");
            pAdvertising->addServiceUUID(pVirusService->getUUID());
            Serial.println("Starting Service");
            pAdvertising->start();
            Serial.println("Done");
        };

        void stopSpreadingVirus() {
            Serial.println("Stopping Service");
            pAdvertising->stop();
            Serial.println("Removing UUID");
            pAdvertising->removeServiceUUID(pVirusService->getUUID());
            Serial.println("Done");
        };
};

PlayerBluetooth Player_Bluetooth;
