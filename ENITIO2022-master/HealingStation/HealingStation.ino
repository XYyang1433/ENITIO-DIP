// upload this code whenever you want to start testing
// comment out the clearEEPROM() function when used on real game

#include <EEPROM.h>
#include "ENITIO_ir.h"
#include "ENITIO_enums.h"
#include "ENITIO_ESPNOW.h"
#include "ENITIO_NeoPixel.h"
#include "ENITIO_joystick.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...

// the font height for font size 1 is 8
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define EEPROM_SIZE 10

#define ENABLE_add 0  // 0 means Treasure has not been initialized, 1 means already initialized

#define R_ON 255
#define G_ON 0
#define B_ON 0

int HEALING_STATION_INITIAL_HP;
int HEALING_STATION_ACTION_RECV_WAIT;
int HEALING_STATION_RECOVER_DURATION;

#define HEAL_MANA 14

class HealingStation
{
  private:
    int HP;
    unsigned long last_healing_request = 0;
    unsigned long timeleftToRecover = 0;
    
  public:
    
    int OG_, ID_, En_, action_; 

    void init_treasure(){
      EEPROM.write(ENABLE_add, 1);
      EEPROM.commit();
      HP = HEALING_STATION_INITIAL_HP;
    };
    
    // void receiveAction() {
    //   int currStatus = EEPROM.read(ENABLE_add);
    //   unsigned long currTime = millis();
    //   if (currStatus == 1 && HealingStation_IR.available()) {
    //       ir_signal IRsignal_ = HealingStation_IR.read();

    //       if (currTime - last_healing_request >= HEALING_STATION_ACTION_RECV_WAIT){
    //           Serial.println("Initiate Healing Request");
    //           OG_ = IRsignal_.address.digit2;
    //           ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);
        
    //           En_ = IRsignal_.command.digit1;
    //           action_ = IRsignal_.command.digit0;
        
    //           Serial.printf("%d %d %d %d \n", action_, En_, ID_, OG_);
      
    //           if ((action_ == collect) || (action_ == heal_request)) {
    //               heal_player(OG_, ID_);
    //               HP--;
    //               Serial.print(HP); Serial.println(" Before Station Closes");
    //               if (HP == 0) {
    //                   Serial.println("Closing Healing Station");
    //                   HealingStation_NeoPixel.off_FRONT();
    //                   HealingStation_NeoPixel.off_TOP();
    //                   healing_station.display_no_hp();
    //                   EEPROM.write(ENABLE_add, 2);
    //                   EEPROM.commit();
    //               }
    //           }
    //           last_healing_request = currTime;
    //       }
    //   } else if (currStatus == 2) {
    //       timeleftToRecover = max(int(HEALING_STATION_RECOVER_DURATION - (currTime - last_healing_request)), 0);
    //       if (timeleftToRecover == 0) {
    //         Serial.println("Reopening Healing Station");
    //         HP = HEALING_STATION_INITIAL_HP;
    //         EEPROM.write(ENABLE_add, 1);
    //         EEPROM.commit();
    //         HealingStation_NeoPixel.displayRGB_FRONT(R_ON, B_ON, G_ON);
    //         HealingStation_NeoPixel.displayRGB_TOP(R_ON, B_ON, G_ON);
    //       }
    //   }
    // };

    void HealingLoop(){
      int currStatus = EEPROM.read(ENABLE_add);
      unsigned long currTime = millis();
      if (currStatus == 1){
        handleJoystick();
      }
      else if (currStatus == 2){
        timeleftToRecover = max(int(HEALING_STATION_RECOVER_DURATION - (currTime - last_healing_request)), 0);
        display_no_hp(timeleftToRecover);
        if (timeleftToRecover == 0) {
          Serial.println("Reopening Healing Station");
          HP = HEALING_STATION_INITIAL_HP;
          EEPROM.write(ENABLE_add, 1);
          EEPROM.commit();
          HealingStation_NeoPixel.displayRGB_FRONT(R_ON, B_ON, G_ON);
          HealingStation_NeoPixel.displayRGB_TOP(R_ON, B_ON, G_ON);
        }
      }
    }

    void handleJoystick(){
      joystick_pos joystick_pos = HealingStation_joystick.read_Joystick();
      if (HealingStation_joystick.get_state() == 0) {
        switch (joystick_pos){
            case button:
              heal_player();
              HealingStation_joystick.set_state();
              HP--;
              Serial.print(HP); Serial.println(" Before Station Closes");
              if (HP == 0) {
                  Serial.println("Closing Healing Station");
                  HealingStation_NeoPixel.off_FRONT();
                  HealingStation_NeoPixel.off_TOP();
                  EEPROM.write(ENABLE_add, 2);
                  EEPROM.commit();
                  last_healing_request = millis();
              }
              break;

            case idle:
              break;

            default:
              HealingStation_joystick.set_state();
              break;
          }
      }
    }

    void heal_player(){
      uint16_hex_digits address_digits, command_digits;

      address_digits.digit0 = 0;
      address_digits.digit2 = 0;

      command_digits.digit0 = heal;

      command_digits.digit1 = HEAL_MANA;

      ir_signal send_signal;

      send_signal.address = address_digits;
      send_signal.command = command_digits;

      HealingStation_IR.send(send_signal, 1);
    } ;

    void display_not_playing_yet(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Healing Station  ")); 
          
      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("Hold on!!");
      display.setCursor(0, 30);
      display.println("The game has not");
      display.println("started yet.");
      display.display();
    };

    void display_in_game(){
      int currStatus = EEPROM.read(ENABLE_add);
      if(currStatus == 1) {
        display.clearDisplay();\
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Healing Station  ")); 

        display.setCursor(0, 20);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("  Put your WANDERer  ");
        display.setCursor(0, 32);
        display.println(" towards the Station ");
        display.setCursor(0, 44);
        display.println("      and PRESS!     ");
      
        display.display();
      }
    };

    void display_no_hp(int this_timeleftToRecover){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Healing Station  ")); 
          
      display.setCursor(0, 24);
      display.setTextColor(SSD1306_WHITE); // Draw white text              |  
      display.println("     Out of HP      ");

      display.setCursor(0, 40);
      display.print("Come back in: ");
      display.println(int(this_timeleftToRecover/1000.0)+1);
      display.display();
    };

};

HealingStation healing_station;
bool gameStarted = 0;

void clearEEPROM(){
  int i;
  for (i=0; i<EEPROM_SIZE; i++){
    EEPROM.write(i,0);
  }
}

int get_game_state(){
      
      // retrieve game state from server
      // 0 mean game did not start
      // 1 mean in game
      // once the game has started then we dunnid to check anymore
   /*
   if (!gameStarted) {
     gameStarted = dbc.hasGameStarted();
     Serial.print("Game Status: "); Serial.println(gameStarted);
     if(gameStarted) {
       HealingStation_NeoPixel.displayRGB_FRONT(R_ON, B_ON, G_ON);
       HealingStation_NeoPixel.displayRGB_TOP(R_ON, B_ON, G_ON);
       healing_station.init_treasure();
     }
     return gameStarted;
     }
     */
     
    HealingStation_NeoPixel.displayRGB_FRONT(R_ON, B_ON, G_ON);
    HealingStation_NeoPixel.displayRGB_TOP(R_ON, B_ON, G_ON);
    healing_station.init_treasure();
    return 1;
}

void setup() {
  HealingStation_NeoPixel.initialize();
  HealingStation_NeoPixel.off_FRONT();
  HealingStation_NeoPixel.off_TOP();
  Serial.begin(115200);
  HealingStation_IR.enable();
  EEPROM.begin(EEPROM_SIZE);
  clearEEPROM();

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }

  HealingStation_EspNOW.enable();
  
  bool isWiFiConnected = dbc.connectToWiFi();
  while (!isWiFiConnected) {
    Serial.println("Reconnecting..");
    isWiFiConnected = dbc.connectToWiFi();
  }

  GAME_CONSTANTS game_consts = dbc.getGameConstants();
  HTTP_TIMEOUT = game_consts.HTTP_TIMEOUT;
  HEALING_STATION_INITIAL_HP = game_consts.HEALING_STATION_INITIAL_HP;
  HEALING_STATION_ACTION_RECV_WAIT = game_consts.HEALING_STATION_ACTION_RECV_WAIT;
  HEALING_STATION_RECOVER_DURATION = game_consts.HEALING_STATION_RECOVER_DURATION;
  
}

void loop() {
  // First check if ESP is connected to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    bool isWiFiConnected = dbc.connectToWiFi();
    if (!isWiFiConnected) {
        // timeout
        ESP.restart();
    }
  }
  switch (get_game_state()){
    case 0:
      healing_station.display_not_playing_yet();
      delay(10000);
      break;
    case 1:
      healing_station.HealingLoop();
      healing_station.display_in_game();
      break;
  }
}
