#include <string>
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
int ReceivedConstant[99];

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
  int ID,OG; 
  std::string IDstring;
  size_t n_zero = 3;
  
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
            ID = EEPROM.read(ID_add);
            OG = EEPROM.read(OG_add);
                        
            IDstring = std::string(n_zero - std::min(n_zero, std::to_string(ID).length()), '0') + std::to_string(ID);
            Serial.println(IDstring.c_str());
            
            std::string displayString= "Device with OG-ID:" + std::to_string(OG)+"-"+IDstring;
            std::string UUIDHeader = "6E4"+std::to_string(OG)+ IDstring;
            
            SERVICE_UUID = UUIDHeader +"1-B5A3-F393-E0A9-E50E24DCCA9E"; 
            CHARACTERISTIC_UUID_RX = UUIDHeader +"2-B5A3-F393-E0A9-E50E24DCCA9E";
            CHARACTERISTIC_UUID_TX = UUIDHeader +"3-B5A3-F393-E0A9-E50E24DCCA9E";
            
            Serial.println(SERVICE_UUID.c_str());
            Serial.println(CHARACTERISTIC_UUID_RX.c_str());
            Serial.println(CHARACTERISTIC_UUID_TX.c_str());            
                            
            NimBLEDevice::init(displayString);
            pServer = NimBLEDevice::createServer();
            pServer->setCallbacks(new MyServerCallbacks());
            pScan = NimBLEDevice::getScan();
            pScan->setActiveScan(true);
            
            pService = pServer->createService(SERVICE_UUID);

            pTxCharacteristic = pService->createCharacteristic(
                CHARACTERISTIC_UUID_TX,
                NIMBLE_PROPERTY::READ
            );//transmit,inverse for client

            pRxCharacteristic = pService->createCharacteristic(
                CHARACTERISTIC_UUID_RX,             
                NIMBLE_PROPERTY::WRITE
            );//receive,inverse for client

            pRxCharacteristic->setCallbacks(new MyCallbacks());//if receive, get data

            pAdvertising = NimBLEDevice::getAdvertising();
            pAdvertising->setScanResponse(true);
  
            pService->start();//servce start
            delay(1000);
            pAdvertising->start();// Start advertising
            Serial.println("Waiting a client connection to notify...");
        }

        void SentValueToPhone() {
            //every minute need to renew a string to sent
            //read EEPROM every time
            //OG:ID, numL1Treasure, numL2Treasure, Kill1OG:Kill1ID, Kill2OG:Kill2ID..
            int tempKillID,tempKillOG,tempLocAdd,number_of_level1_treasure,number_of_level2_treasure;
            std::string DataToSent,tempKillStringID;
            
            number_of_level1_treasure=EEPROM.read(PLAYER_numL1Treasure_add);
            number_of_level2_treasure=EEPROM.read(PLAYER_numL2Treasure_add);
            
            
            
            DataToSent = std::to_string(OG) +"-"+ IDstring +","+ std::to_string(number_of_level1_treasure) +","+ std::to_string(number_of_level2_treasure);
                        
            int kill=EEPROM.read(KILL_location_add);
            if(kill-20>=0)
              for(int i=20;i<kill;i=i+2){
                tempKillID=EEPROM.read(i);
                tempKillOG=EEPROM.read(i+1);
                tempKillStringID = std::string(n_zero - std::min(n_zero, std::to_string(tempKillID).length()), '0') + std::to_string(tempKillID);
                DataToSent += ","+ std::to_string(tempKillOG)+"-" + tempKillStringID;
              }    
            Serial.println(DataToSent.c_str());
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

        void CSVdecoder(){
          //clear array
          int receivedMessageCount=0;
          std::size_t comma_location ;
          for (int i=0;i<99;i++){
            ReceivedConstant[i]=0;
          }
          std::string tempMessage=fullMessage;
          do{   
            comma_location = tempMessage.find(",");
            ReceivedConstant[receivedMessageCount] = stoi(tempMessage.substr(0,comma_location));
            receivedMessageCount++;
            tempMessage=tempMessage.substr(comma_location+1,tempMessage.length()-1);                            
          }while(comma_location != std::string::npos);    
          ReceivedConstant[receivedMessageCount] = stoi(tempMessage);//last one without comma
            
        }
        

        void PlayerUARTloop() {             
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
                             
        }
        int LastUpdateTime=millis();
        void PlayerKilledDataUpdateLoop(){
          const int time_to_upload = 5*1000;//[ms]
          if(isPaired){
            if(millis()-LastUpdateTime>=time_to_upload){
              SentValueToPhone();
              LastUpdateTime=millis();
              Serial.println("Data sent");  
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
