#include <stdio.h>
#include <NimBLEDevice.h>

BLEServer* pServer = NULL;
BLECharacteristic* pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
std::string fullMessage = "";
bool messageStart = false;
bool messageEnd = false;


void SentValueToPhone(std::string dataToSent);
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"


class PlayerUART {
	public:
        class MyServerCallbacks : public BLEServerCallbacks {
            void onConnect(BLEServer* pServer) {
                deviceConnected = true;
            };
            void onDisconnect(BLEServer* pServer) {
                deviceConnected = false;
            }
            
            uint32_t onPassKeyRequest() {
                Serial.println("Server PassKeyRequest");
                return 123456;
            }

            bool onConfirmPIN(uint32_t pass_key) {
                Serial.print("The passkey YES/NO number: "); Serial.println(pass_key);
                return true;
            }

            void onAuthenticationComplete(ble_gap_conn_desc desc) {
                Serial.println("Starting BLE work!");
            }
        };
        class MyCallbacks : public BLECharacteristicCallbacks {
            void onWrite(BLECharacteristic* pCharacteristic) {
                std::string rxValue = pCharacteristic->getValue();

                if (rxValue.length() > 0) {

                    std::string startSymbol = "!";
                    std::string endSymbol = "@";
                    std::size_t startPos = rxValue.find("!");//find starting position

                    if (startPos != std::string::npos) {//find() will return string::npos if no match
                        messageStart = true;
                        rxValue = rxValue.substr(startPos + 1, rxValue.length() - 1);
                        /*
                        Serial.println("message start:");
                         for (int i = 0; i < rxValue.length(); i++){
                          Serial.print(rxValue[i]);
                         }
                        Serial.println("");
                        */
                    }
                    std::size_t endPos = rxValue.find("@");//find end position
                    if (endPos != std::string::npos) {
                        messageEnd = true;
                        rxValue = rxValue.substr(0, endPos);
                        /*
                        Serial.println("message end:");
                        for (int i = 0; i < rxValue.length(); i++){
                          Serial.print(rxValue[i]);
                         }
                        Serial.println("");
                        */
                    }
                    fullMessage += rxValue;

                    if (messageStart == false && messageEnd == false) {
                        fullMessage = "";
                    }
                    if (messageStart == true && messageEnd == true) {

                        Serial.println("*********");
                        Serial.print("Received Value: ");
                        for (int i = 0; i < fullMessage.length(); i++)
                            Serial.print(fullMessage[i]);

                        Serial.println();
                        Serial.println("*********");
                        messageStart = false;
                        messageEnd = false;

                    }
                }
            }
        };
        
        void initialise() {
            
            BLEDevice::init("UART Service");
            pServer = BLEDevice::createServer();
            pServer->setCallbacks(new MyServerCallbacks());
            BLEService* pService = pServer->createService(SERVICE_UUID);

            pTxCharacteristic = pService->createCharacteristic(
                CHARACTERISTIC_UUID_TX,
                NIMBLE_PROPERTY::READ
            );//transmit

            BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
                CHARACTERISTIC_UUID_RX,             
                NIMBLE_PROPERTY::WRITE
            );//receive

            pRxCharacteristic->setCallbacks(new MyCallbacks());//if receive, get data

            pService->start();//servce start
            pServer->getAdvertising()->start();// Start advertising
            Serial.println("Waiting a client connection to notify...");
        }

        void SentValueToPhone(std::string dataToSent) {
            pTxCharacteristic->setValue(dataToSent);
        }
        

        void PlayerUARTloop() {

            // disconnecting
            if (!deviceConnected && oldDeviceConnected) {
                delay(500); // give the bluetooth stack the chance to get things ready
                pServer->startAdvertising(); // restart advertising
                Serial.println("start advertising");
                oldDeviceConnected = deviceConnected;
            }
            // connecting
            if (deviceConnected && !oldDeviceConnected) {
                Serial.println("Device connected");
                oldDeviceConnected = deviceConnected;
            }
        }

};

PlayerUART Player_UART;