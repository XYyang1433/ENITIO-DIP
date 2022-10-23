#include <stdio.h>
#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>

bool isPaired=0;
bool isIDinput=0;

NimBLEScan *pScan;
NimBLEServer* pServer;
NimBLEService* pService;
NimBLEAdvertising* pAdvertising;
NimBLECharacteristic* pTxCharacteristic;
NimBLECharacteristic* pRxCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
std::string fullMessage = "";
bool messageStart = false;
bool messageEnd = false;
int scanTime = 2;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

std::string SERVICE_UUID; // UART service UUID 
std::string CHARACTERISTIC_UUID_RX;
std::string CHARACTERISTIC_UUID_TX ;

/**  Constants **/
// This Service UUID is broadcasted when the level 2 treasure is available
#define availableServiceUUID NimBLEUUID("b8fc9a62-5c8b-4bd4-9f00-c05b558dc9ee")
#define availableCharacteristicUUID NimBLEUUID("121153c0-1a5a-43cb-9fe2-240b22dc5958")



class PlayerUART {
	public:
  int ID = EEPROM.read(ID_add);
  int OG = EEPROM.read(OG_add);
  /*
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
        };*/
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
                    if (endPos != std::string::npos && messageStart==true) {
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
                    if (messageStart == false && messageEnd == false) {
                        fullMessage = "";
                    }
                }
            }
        };
        
        void initialise() {
            
            std::string displayString= "Device with ID:" + std::to_string(ID);
            Serial.println(ID);
            std::string UUIDHeader = "6E4"+std::to_string(OG)+std::to_string(ID);
            SERVICE_UUID = UUIDHeader +"1-B5A3-F393-E0A9-E50E24DCCA9E"; 
            CHARACTERISTIC_UUID_RX = UUIDHeader +"2-B5A3-F393-E0A9-E50E24DCCA9E";
            CHARACTERISTIC_UUID_TX = UUIDHeader +"3-B5A3-F393-E0A9-E50E24DCCA9E";
                
            NimBLEDevice::init(displayString);
            pServer = NimBLEDevice::createServer();
            pScan = NimBLEDevice::getScan();
            pScan->setActiveScan(true);
            pAdvertising = NimBLEDevice::getAdvertising();
            pAdvertising->setScanResponse(true);
            pService = pServer->createService(SERVICE_UUID);

            pTxCharacteristic = pService->createCharacteristic(
                CHARACTERISTIC_UUID_TX,
                NIMBLE_PROPERTY::READ
            );//transmit

            pRxCharacteristic = pService->createCharacteristic(
                CHARACTERISTIC_UUID_RX,             
                NIMBLE_PROPERTY::WRITE
            );//receive

            pRxCharacteristic->setCallbacks(new MyCallbacks());//if receive, get data
  
            pService->start();//servce start
            pServer->getAdvertising()->start();// Start advertising
            Serial.println("Waiting a client connection to notify...");
        }

        void SentValueToPhone() {
            //every minute need to renew a string to sent
            //read EEPROM every time
            //OG:ID, numL1Treasure, numL2Treasure, Kill1OG:Kill1ID, Kill2OG:Kill2ID..
            int tempKillID,tempKillOG,tempLocAdd,number_of_level1_treasure,number_of_level2_treasure;
            std::string DataToSent;
            
            number_of_level1_treasure=EEPROM.read(PLAYER_numL1Treasure_add);
            number_of_level2_treasure=EEPROM.read(PLAYER_numL2Treasure_add);
            
            DataToSent = std::to_string(OG) +":"+ std::to_string(ID) +","+ std::to_string(number_of_level1_treasure) +","+ std::to_string(number_of_level2_treasure);
                        
            tempLocAdd=EEPROM.read(KILL_location_add);
            if(tempLocAdd!=0)
              for(int i=20;i<20+killedCount*2;i=i+2){
                tempKillID=EEPROM.read(i);
                tempKillOG=EEPROM.read(i+1);
                DataToSent += ","+ std::to_string(tempKillOG)+":" + std::to_string(tempKillID);
              }    
        
            pTxCharacteristic->setValue(DataToSent);
            
        }
        
        void scan() {
            // Returns all devices found after scanTime seconds
            NimBLEScanResults results = pScan->start(2, false);
            int virus_device_counter = 0;
            for (int i=0; i < results.getCount(); i++) {
                NimBLEAdvertisedDevice device = results.getDevice(i);
                if (device.haveServiceUUID()) {
                  if (device.isAdvertisingService(availableServiceUUID)) {
                    // available treasure nearby
                    Serial.println("Level 2 Treasure Available");
                    Serial.println(device.toString().c_str());
                  }
                }
            }
          };
        
int LastUpdateTime=millis();
        void PlayerUARTloop() {  
            const int time_to_upload = 60000;//[ms]
            // disconnecting
            if (!deviceConnected && oldDeviceConnected) {
                isPaired=0;
                delay(500); // give the bluetooth stack the chance to get things ready
                pServer->startAdvertising(); // restart advertising
                Serial.println("Device disconnected, start advertising");
                oldDeviceConnected = deviceConnected;
            }
            // connecting
            if (deviceConnected && !oldDeviceConnected) {
                isPaired= 1;
                Serial.println("Device connected");
                oldDeviceConnected = deviceConnected;
            }
           if(isPaired){
            if(millis()-LastUpdateTime>=time_to_upload){
              SentValueToPhone();
              LastUpdateTime=millis();  
            }     
          }
                   
        }

};

PlayerUART Player_UART;
class Pairer{
    private:
    
    public:
        void handle_joystick(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos)
                { 
                case button:
                    currentProcess = MainMenuProcess;
                    Player_joystick.set_state();
                    break;

                case idle:
                    break;

                default:
                    Player_joystick.set_state();
                    break;
                }
            }
        };
        
        void PairLoop(){
            if(isPaired){
              BluetoothPair_OLED.deviceConnectedDisplay();
            }
            else{
              BluetoothPair_OLED.nodeviceDisplay();
            }          
            
            handle_joystick();
        }
};

Pairer My_Pairer;
