// upload this code whenever you want to start testing
// comment out the clearEEPROM() function when used on real game

#include <EEPROM.h>
#include "ENITIO_ir.h"
#include "ENITIO_enums.h"
#include "ENITIO_bluetooth.h"
#include "ENITIO_EspNOW.h"
#include "ENITIO_NeoPixel.h"
#include "ENITIO_joystick.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define R_ON 255
#define G_ON 255
#define B_ON 255

int id;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...

// the font height for font size 1 is 8
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned char enitioLogo [] PROGMEM = { 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x1f, 0x80, 0x0c, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0x9f, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x07, 0x9f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0xfc, 0x7f, 0xff, 0xf8, 0x00, 0x00, 0x78, 0x1f, 0x9f, 0x0f, 0x80, 0x07, 0x8f, 0x80, 0x00, 
  0x01, 0xfc, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x7c, 0x1f, 0x9f, 0x0f, 0x80, 0x7f, 0x8f, 0xf0, 0x00, 
  0x01, 0xfc, 0x7f, 0xff, 0x07, 0xf8, 0x00, 0x7c, 0x1f, 0x9f, 0x0f, 0x81, 0xff, 0x8f, 0xfc, 0x00, 
  0x01, 0xfc, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x7c, 0x1f, 0x9f, 0x0f, 0x83, 0xfe, 0x07, 0xfe, 0x00, 
  0x01, 0xfc, 0x40, 0x00, 0x00, 0x7f, 0x80, 0x7c, 0x1f, 0x9f, 0x0f, 0x87, 0xf8, 0x00, 0xff, 0x00, 
  0x01, 0xfc, 0x7f, 0xf8, 0x04, 0x1f, 0xe0, 0x7c, 0x1f, 0x9f, 0x0f, 0x87, 0xf0, 0x00, 0x7f, 0x00, 
  0x01, 0xfc, 0x7f, 0xf8, 0x07, 0x07, 0xf8, 0x3c, 0x1f, 0x9f, 0x0f, 0x87, 0xf0, 0x00, 0x3f, 0x00, 
  0x01, 0xfc, 0x7f, 0xf8, 0x07, 0xc1, 0xfe, 0x0c, 0x1f, 0x9f, 0x0f, 0x87, 0xf0, 0x00, 0x7f, 0x00, 
  0x01, 0xfc, 0x00, 0x00, 0x07, 0xe0, 0x7f, 0x80, 0x1f, 0x9f, 0x0f, 0x83, 0xfc, 0x01, 0xfe, 0x00, 
  0x01, 0xfc, 0x7f, 0xff, 0xc7, 0xe0, 0x1f, 0xe0, 0x1f, 0x9f, 0x0f, 0x81, 0xff, 0x8f, 0xfc, 0x00, 
  0x01, 0xfc, 0x7f, 0xff, 0xc7, 0xe0, 0x07, 0xf8, 0x1f, 0x9f, 0x0f, 0x80, 0x7f, 0x8f, 0xf8, 0x00, 
  0x01, 0xf8, 0x7f, 0xff, 0xc7, 0xe0, 0x01, 0xfc, 0x1f, 0x9f, 0x0f, 0x80, 0x1f, 0x8f, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xc0, 0x30, 0x30, 0x07, 0xc0, 0x1c, 0x08, 0x1f, 0x02, 0x06, 0x03, 0x83, 0x80, 0x00, 
  0x00, 0x70, 0x60, 0x70, 0x30, 0x1c, 0x78, 0x1f, 0x0c, 0x1f, 0x0e, 0x06, 0x03, 0xef, 0x80, 0x00, 
  0x00, 0x60, 0x30, 0x70, 0x30, 0x38, 0x18, 0x1d, 0xcc, 0x1f, 0x0e, 0x06, 0x03, 0x3d, 0x80, 0x00, 
  0x00, 0x38, 0xe0, 0x38, 0x70, 0x3f, 0xf8, 0x18, 0x7c, 0x1f, 0x07, 0x0e, 0x03, 0x01, 0x80, 0x00, 
  0x00, 0x1f, 0xc0, 0x1f, 0xc0, 0x30, 0x10, 0x08, 0x1c, 0x1f, 0x03, 0xf8, 0x01, 0x01, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xc0, 0xf8, 0x18, 0x70, 0xff, 0x83, 0xfc, 0x1f, 0x02, 0x3f, 0xe1, 0xff, 0x00, 0x00, 
  0x00, 0x60, 0x03, 0x0c, 0x1f, 0xf0, 0xc0, 0x06, 0x00, 0x1f, 0x00, 0x00, 0x60, 0x03, 0x00, 0x00, 
  0x00, 0x67, 0xc3, 0xfe, 0x1b, 0x30, 0xfe, 0x03, 0xf8, 0x1f, 0x00, 0x1f, 0xc0, 0xfe, 0x00, 0x00, 
  0x00, 0x3f, 0xc3, 0x04, 0x18, 0x30, 0xff, 0x87, 0xf8, 0x0f, 0x00, 0x1f, 0xf1, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

void StartUpDisplay(){
  display.clearDisplay();
  display.setTextSize(1); // Draw SIZE

  display.setTextColor(SSD1306_WHITE); 

  display.drawBitmap(0, 4, enitioLogo, 128, 44, WHITE); 

  display.setCursor(0, 56);
  display.println("   Please wait ...   ");
  display.display();
};

#define EEPROM_SIZE 20

#define hp_pxl_bar_width 2 // [pxl/HP]

#define ENABLE_add 0 // 0 means Treasure has not been initialized, 1 means already initialized
#define ID_add 1
#define HP_add 2
#define collectedOG_add 3
#define collectedID_add 4
#define uploadSuccess_add 5


#define DOUBLE_CLICK_LENGTH 1000 // [ms]

int TREASURE_LEVEL2_INITIAL_HP;
int NUM_L2TREASURES;
int TREASURE_VIRUS_THRESHOLD;
int TREASURE_LEVEL2_ACTION_RECV_WAIT;
int TREASURE_LEVEL2_RECOVER_PERIOD;
int TREASURE_LEVEL2_VIRUS_INFECTION_TIME;

class TreasureLevel2
{
  private:
    int HP;
    bool _isVirus;

    unsigned long lastRecoveredTime = 0;
    unsigned long lastActionReceived = 0;
    unsigned long timeleftToRecover = 0;

  public:
    
    int OG_, ID_, En_, MANA_, action_; 

    void setup_initial_state(){
      if (id > TREASURE_VIRUS_THRESHOLD) _isVirus = true;
      else _isVirus = false;

      if (EEPROM.read(ENABLE_add) == 0){
      EEPROM.write(ENABLE_add, 1);
      EEPROM.write(HP_add, TREASURE_LEVEL2_INITIAL_HP);
      EEPROM.commit(); 
      HP = TREASURE_LEVEL2_INITIAL_HP;}
      else{
        HP = EEPROM.read(HP_add);
        if (HP == 0) {
          OG_ = EEPROM.read(collectedOG_add);
        }
      }

      TreasureLevel2_Bluetooth.initialise(id);
      
      if (HP == 0) {
        TreasureLevel2_NeoPixel.off_FRONT();
        TreasureLevel2_NeoPixel.off_TOP();
      }
      else {
        TreasureLevel2_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
        TreasureLevel2_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
      }
      
    }

    void feedback_collectL2(int OG_, int ID_){
      bool killed = (HP == 0);
      TreasureLevel2_EspNOW.send_data(3, OG_, ID_, id, killed);
    } ;

    void receiveAction() {
      if (HP > 0) {
        unsigned long currTime = millis();
        if (TreasureLevel2_IR.available()) {
           ir_signal IRsignal_ = TreasureLevel2_IR.read();

           if (currTime - lastActionReceived > TREASURE_LEVEL2_ACTION_RECV_WAIT) {
             OG_ = IRsignal_.address.digit2;
             ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);

             MANA_ = IRsignal_.command.digit1;
             action_ = IRsignal_.command.digit0;
        
             Serial.printf("%d %d %d %d %d\n", action_, MANA_, ID_, OG_, HP);

             lastActionReceived = currTime;
      
             if (action_ == collect) {
               HP = max(HP-MANA_, 0);
               feedback_collectL2(OG_, ID_);
               EEPROM.write(HP_add, HP);
               EEPROM.commit(); 
               Serial.printf("Current HP: %d\n", HP);
               unsigned long currTime = millis();
               lastRecoveredTime = currTime;
               if (HP == 0) handle_Collected();
             }
           }
        }
      }
    }

    void recover(){
      if (HP == TREASURE_LEVEL2_INITIAL_HP) {
        lastRecoveredTime = millis();
      }
      else if (HP > 0){
        unsigned long currTime = millis();
        timeleftToRecover = max(int(TREASURE_LEVEL2_RECOVER_PERIOD - (currTime - lastRecoveredTime)), 0);
        if (timeleftToRecover == 0){
          HP++;
          lastRecoveredTime = currTime;
          EEPROM.write(HP_add, HP);
          EEPROM.commit(); 
        }
      }
      else if (HP == 0){
         // do nothing
      }    
    }

    void handle_Collected() {
      interim_collected_display();
      TreasureLevel2_NeoPixel.off_FRONT();
      TreasureLevel2_NeoPixel.off_TOP();
      // no need to feedback everytime the player collecting the Treasure. Only feedback to the server and to the player when the treasure is fully collected, ie. HP == 0
      TreasureLevel2_Bluetooth.stopAdvertisingService(pAvailableService);
      delay(2000);
      Serial.print("TREASURE NAME:"); Serial.println(TreasureLevel2_Bluetooth.getTreasureName());
      Serial.printf("OG: %d ID: %d\n", OG_, ID_);
      // this code to save the info of the player collected the treasure to resend if required
      EEPROM.write(collectedOG_add, OG_);
      EEPROM.write(collectedID_add, ID_);
      EEPROM.commit(); 
      // broadcast virus here ...
      if (_isVirus) {
        Serial.println("Broadcasting Virus...");
        TreasureLevel2_Bluetooth.startAdvertisingService(pVirusService);
        delay(TREASURE_LEVEL2_VIRUS_INFECTION_TIME);
        TreasureLevel2_Bluetooth.stopAdvertisingService(pVirusService);
        Serial.println("Box Shutting down...");
      }

      // upload to server
      int curr_upload_fail_counter = wifi_timeout_or_refused_counter;
      String player_mac_address = dbc.setTreasureAsOpened(TreasureLevel2_Bluetooth.getTreasureName(), OG_, ID_);
      int new_upload_fail_counter = wifi_timeout_or_refused_counter;
      if (new_upload_fail_counter == curr_upload_fail_counter) {
        EEPROM.write(uploadSuccess_add, 1);
        EEPROM.commit();
      }
    }

    void resend_treasure_collection() {
        int upload_status = EEPROM.read(uploadSuccess_add);
        if (HP == 0 && upload_status == 0) {
            int curr_upload_fail_counter = wifi_timeout_or_refused_counter;
            OG_ = EEPROM.read(collectedOG_add);
            ID_ = EEPROM.read(collectedID_add);
            String player_mac_address = dbc.setTreasureAsOpened(TreasureLevel2_Bluetooth.getTreasureName(), OG_, ID_);
            int new_upload_fail_counter = wifi_timeout_or_refused_counter;
            if (new_upload_fail_counter == curr_upload_fail_counter) {
                EEPROM.write(uploadSuccess_add, 1);
                EEPROM.commit();
            }
        }
    }

    void display_not_playing_yet(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Level 2 Treasure  ")); 
          
      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("Hold on!!");
      display.setCursor(0, 30);
      display.println("The game has not");
      display.println("started yet.");
      display.display();
    };

    void interim_collected_display(){
      display.clearDisplay();\
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Level 2 Treasure  ")); 

      display.setCursor(0, 16);
      display.setTextSize(2);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.print("HP ");

      display.setCursor(110, 32);
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.println(HP);

      display.setCursor(0, 48);
      display.print("Please wait ...");
      display.display();
    }
    
    void display_in_game(){
      if(HP != 0) {
        display.clearDisplay();\
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Level 2 Treasure  ")); 

        display.setCursor(0, 16);
        display.setTextSize(2);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.print("HP ");
        display.fillRect(0, 34, HP*hp_pxl_bar_width, 4, SSD1306_WHITE);

        display.setCursor(110, 32);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.println(HP);

        display.setCursor(0, 48);
        if (timeleftToRecover > 0) {
          display.print("Recover in: ");
          display.println(int(timeleftToRecover/1000.0)+1);
        }
        display.display();
      }
      else {
        display.clearDisplay();\
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Level 2 Treasure  ")); 

        display.setCursor(0, 16);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("  Congratulations!!  ");

        display.setCursor(0, 30);
        if (!_isVirus) {
          display.println("   It's a Treasure!  ");
        }
        else{
          display.println("    It's a Virus!   ");
        }   

        display.setCursor(0, 46);
        

        if (OG_ == ALATAR)
          display.println("OG collected: ALATAR");
        else if (OG_ == DRACHEN)
          display.println("OG collected: DRACHEN");
        else if (OG_ == EVA)
          display.println("OG collected: EVA");
        else if (OG_ == INVICTA)
          display.println("OG collected: INVICTA");

        display.display();
      };
    }
      
    void display_game_finished(){};
};

TreasureLevel2 Treasure;
bool gameStarted = 0;
bool initiateTreasureDone = 0;

int get_game_state(){
      // retrieve game state from server
      // 0 mean game did not start
      // 1 mean in game
      // once the game has started then we dunnid to check anymore
   if (!gameStarted) {
     gameStarted = dbc.hasGameStarted();
     if (gameStarted) {
        // start BLE functions
        Treasure.setup_initial_state();
        TreasureLevel2_Bluetooth.startAdvertisingService(pAvailableService);
        initiateTreasureDone = 1;
     }
     return gameStarted;
   } else 
   {
    return 1;
   }
}

bool AdminFunction = false;
bool clicked_once = 0;
unsigned long last_clicked = 0;

#include "Admin.h" 

void handleJoystick(){
  joystick_pos joystick_pos = TreasureLevel2_joystick.read_Joystick();
  if (TreasureLevel2_joystick.get_state() == 0) {
    switch (joystick_pos){
    case button:
      if (clicked_once){
        AdminFunction = true;
        clicked_once = 0;
      }
      else{
        clicked_once = 1;
        last_clicked = millis();
      }
      TreasureLevel2_joystick.set_state();
      break;
      
    case idle:
      if (clicked_once){
          unsigned long currTime = millis();
          if(currTime - last_clicked > DOUBLE_CLICK_LENGTH) clicked_once = 0;
        }
      break;
    
    default:
      if (clicked_once){
        unsigned long currTime = millis();
        if(currTime - last_clicked > DOUBLE_CLICK_LENGTH) clicked_once = 0;
      }
      TreasureLevel2_joystick.set_state();
      break;
    }
  }
}

TaskHandle_t backgroundTask;

void backgroundTaskCode(void * pvParameters){
  for ( ; ; ) {
      get_game_state();
      delay(10000);
      Serial.print("This ID: "); Serial.println(id);
  }
};

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
  }
  StartUpDisplay();
  Serial.begin(115200);
  TreasureLevel2_IR.enable();
  TreasureLevel2_NeoPixel.initialize();

  EEPROM.begin(EEPROM_SIZE);
  id = EEPROM.read(ID_add);

  TreasureLevel2_EspNOW.enable();

  bool isWiFiConnected = dbc.connectToWiFi();
  while (!isWiFiConnected) {
    Serial.println("Reconnecting..");
    isWiFiConnected = dbc.connectToWiFi();
  }

  GAME_CONSTANTS game_consts = dbc.getGameConstants();
  HTTP_TIMEOUT = game_consts.HTTP_TIMEOUT;
  TREASURE_LEVEL2_INITIAL_HP = game_consts.TREASURE_LEVEL2_INITIAL_HP;
  NUM_L2TREASURES = game_consts.NUM_L2TREASURES;
  TREASURE_LEVEL2_ACTION_RECV_WAIT = game_consts.TREASURE_LEVEL2_ACTION_RECV_WAIT;
  TREASURE_LEVEL2_RECOVER_PERIOD = game_consts.TREASURE_LEVEL2_RECOVER_PERIOD;
  TREASURE_LEVEL2_VIRUS_INFECTION_TIME = game_consts.TREASURE_LEVEL2_VIRUS_INFECTION_TIME;
  TREASURE_VIRUS_THRESHOLD = game_consts.TREASURE_VIRUS_THRESHOLD;
  
  xTaskCreatePinnedToCore(
                      backgroundTaskCode,   /* Task function. */
                      "backgroundTask",     /* name of task. */
                      10000,       /* Stack size of task */
                      NULL,        /* parameter of the task */
                      2,           /* priority of the task */
                      &backgroundTask,      /* Task handle to keep track of created task */
                      0);
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
  if (wifi_timeout_or_refused_counter >= 3) {
    ESP.restart();
  }
  switch (initiateTreasureDone){
    case 0:
      if(!AdminFunction){
        handleJoystick();
        Treasure.display_not_playing_yet();
      }
      else{
        TreasureLevel2_Admin.AdminLoop();
      }
      break;
    case 1:
      if (!AdminFunction){
        handleJoystick();
        Treasure.display_in_game();
      }
      else{
        TreasureLevel2_Admin.AdminLoop();
      }
      Treasure.receiveAction();
      Treasure.recover();
      Treasure.resend_treasure_collection();
      break;
  }
}
