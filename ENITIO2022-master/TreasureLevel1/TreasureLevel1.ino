// upload this code whenever you want to start testing
// comment out the clearEEPROM() function when used on real game

// int failed_kill_feedback = 0;
// int failed_kill_OG[50] = {};
// int failed_kill_ID[50] = {};
// int current_failed_save_pointer = 0;
// int current_failed_read_pointer = 0;

// bool can_upload_fail = 1;

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

#define EEPROM_SIZE 20

#define ID_BOUND 50

#define hp_pxl_bar_width 100 // [pxl/HP]

#define ENABLE_add 0  // 0 means Treasure has not been initialized, 1 means already initialized
#define ID_add 1
#define HP_add 2
#define ALATAR_add 3
#define DRACHEN_add 4
#define EVA_add 5
#define INVICTA_add 6

#define R_ON 0
#define G_ON 0
#define B_ON 255

#define DOUBLE_CLICK_LENGTH 1000 // [ms]

bool AdminFunction = false;
int ID;

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

// Constants
int TREASURE_LEVEL1_INITIAL_HP;
int TREASURE_LEVEL1_ACTION_RECV_WAIT;
int TREASURE_LEVEL1_RECOVER_DURATION;

void StartUpDisplay(){
  display.clearDisplay();
  display.setTextSize(1); // Draw SIZE

  display.setTextColor(SSD1306_WHITE); 

  display.drawBitmap(0, 4, enitioLogo, 128, 44, WHITE); 

  display.setCursor(0, 56);
  display.println("   Please wait ...   ");
  display.display();
};

bool clicked_once = 0;
unsigned long last_clicked = 0;

void handleJoystick(){
  joystick_pos joystick_pos = TreasureLevel1_joystick.read_Joystick();
  if (TreasureLevel1_joystick.get_state() == 0) {
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
      TreasureLevel1_joystick.set_state();
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
      TreasureLevel1_joystick.set_state();
      break;
    }
  }
}

class TreasureLevel1
{
  private:
    int HP;

    int this_recover_duration = TREASURE_LEVEL1_RECOVER_DURATION;
    unsigned long lastOpenedTime = 0;
     unsigned long lastActionReceived = 0;

  public:
    
    int OG_, ID_, En_, MANA_, action_; 

    void init_treasure(){
      EEPROM.write(ENABLE_add, 1);
      EEPROM.commit();
      HP = TREASURE_LEVEL1_INITIAL_HP;
    };
    
    void receiveAction() {
      int currStatus = EEPROM.read(ENABLE_add);
      if (TreasureLevel1_IR.available()) {
         ir_signal IRsignal_ = TreasureLevel1_IR.read();
         unsigned long currTime = millis();

         if ((currStatus == 1) && (currTime - lastActionReceived > TREASURE_LEVEL1_ACTION_RECV_WAIT)){
          OG_ = IRsignal_.address.digit2;
         ID_ = IRsignal_.address.digit0 + (IRsignal_.address.digit1 << 4);

  
           MANA_ = IRsignal_.command.digit1;
           action_ = IRsignal_.command.digit0;
           lastActionReceived = currTime;
      
           Serial.printf("%d %d %d %d \n", action_, MANA_, ID_, OG_);
    
           if (action_ == collect) {
            lastOpenedTime = millis();
            handle_Collected();
           }
          }
         }
      };

    void feedback_collectL1(int OG_, int ID_){
      unsigned long currTime = millis();
      // randomSeed(currTime);
      int powerup_ID = random(1,6);

       TreasureLevel1_EspNOW.send_data(2, OG_, ID_, ID, powerup_ID);

//       while(!last_send_status){
//         delay(200);
//         TreasureLevel1_EspNOW.send_data(2, OG_, ID_, ID, powerup_ID);
//       }

    } ;

    void handle_Collected() {
      interim_collected_display();
      
      // inform the server here ...
      TreasureLevel1_NeoPixel.off_FRONT();
      TreasureLevel1_NeoPixel.off_TOP();


      feedback_collectL1(OG_, ID_);
      delay(2000);
      // can_upload_fail = 0;
      int player_identifier = OG_ * pow(16, 2) + ID_;
      Serial.printf("TREASURE%d opened by OG %d ID %d\n", ID, OG_, ID_);
      String player_mac_address = dbc.setTreasureAsOpened("TREASURE" + String(ID), OG_, ID_);
      // this code to save the info of the OG collected the treasure

      this_recover_duration = TREASURE_LEVEL1_RECOVER_DURATION*random(1,9);

      EEPROM.write(ENABLE_add, 2);
      switch (OG_)
      {
      case ALATAR:
        EEPROM.write(ALATAR_add, EEPROM.read(ALATAR_add) + 1);
        break;

      case DRACHEN:
        EEPROM.write(DRACHEN_add, EEPROM.read(DRACHEN_add) + 1);
        break;

      case EVA:
        EEPROM.write(EVA_add, EEPROM.read(EVA_add) + 1);
        break;

      case INVICTA:
        EEPROM.write(INVICTA_add, EEPROM.read(INVICTA_add) + 1);
        break;
      
      default:
        break;
      }
      EEPROM.commit();

      // can_upload_fail = 1;
    };

    void recover() {
        // Level1Treasures can recover after a fixed amt of time
        int currStatus = EEPROM.read(ENABLE_add);
        unsigned int currTime = millis();

        if (currStatus == 2 && currTime - lastOpenedTime > this_recover_duration) {
            Serial.println("Reopening Treasure..");
            EEPROM.write(ENABLE_add, 1);
            HP = TREASURE_LEVEL1_INITIAL_HP;
            TreasureLevel1_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
            TreasureLevel1_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
        }
    };

    void display_not_playing_yet(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Level 1 Treasure  ")); 
          
      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("Hold on!!");
      display.setCursor(0, 30);
      display.println("The game has not");
      display.println("started yet.");
      display.display();
    };

    void interim_collected_display(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("   Level 1 Treasure  ")); 
          
      display.setCursor(0, 16);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.println("    Please wait ...   ");
      display.display();
    }

    void display_in_game(){
      int currStatus = EEPROM.read(ENABLE_add);
      if(currStatus == 1) {
        display.clearDisplay();\
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Level 1 Treasure  ")); 

        display.setCursor(0, 24);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("   Collect this to   ");
        display.println("   get a power-up!   ");
      
        display.display();
      }
      else {
        display.clearDisplay();\
        display.setTextSize(1); // Draw SIZE
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("   Level 1 Treasure  ")); 

        display.setCursor(0, 16);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.println("  Treasure is Opened  ");

        display.setCursor(0, 30);
        display.println("  Resetting soon...  ");

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

};

TreasureLevel1 Treasure;  // use OLED to input ID 
bool gameStarted = 0;

void clearEEPROM(){
  int i;
  for (i=0; i<EEPROM_SIZE; i++){
    EEPROM.write(i,0);
  }
  EEPROM.commit();
}
bool setUpDone = 0;
int get_game_state(){
      // retrieve game state from server
      // 0 mean game did not start
      // 1 mean in game
      // once the game has started then we dunnid to check anymore
   
   if (!gameStarted) {
     gameStarted = dbc.hasGameStarted();
     if(gameStarted){
      TreasureLevel1_NeoPixel.displayRGB_FRONT(R_ON, G_ON, B_ON);
      TreasureLevel1_NeoPixel.displayRGB_TOP(R_ON, G_ON, B_ON);
      Treasure.init_treasure();
      setUpDone = 1;
     }
     return gameStarted;
   } else return 1;
}

#include "Admin.h"

TaskHandle_t backgroundTask;

void backgroundTaskCode(void * pvParameters){
  for ( ; ; ) {
      // if ((failed_kill_feedback > 0) && can_upload_fail){
      //   int i;
      //   for (i = 0; i < failed_kill_feedback; i++){
      //     int this_ID = failed_kill_ID[current_failed_read_pointer];
      //     int this_OG = failed_kill_OG[current_failed_read_pointer];
      //     Serial.println(this_ID);
      //     Serial.println(this_OG);
      //     dbc.uploadFailedFeedback("TREASURE" + String(ID), this_OG, this_ID);
      //     current_failed_read_pointer ++ ;
      //     if(current_failed_read_pointer > 50) current_failed_read_pointer -= 50 ;
      //     failed_kill_feedback --;
      //   }
      // }
      get_game_state();
      delay(10000);
  }
};

void setup() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }
  StartUpDisplay();
  Serial.begin(115200);
  TreasureLevel1_NeoPixel.initialize();
  TreasureLevel1_NeoPixel.off_FRONT();
  TreasureLevel1_NeoPixel.off_TOP();

  TreasureLevel1_IR.enable();
  EEPROM.begin(EEPROM_SIZE);
//  int i;
//  for (i=0; i<EEPROM_SIZE; i++){
//    Serial.println(EEPROM.read(i));
//  }
  // clearEEPROM();
  
  ID = EEPROM.read(ID_add);

  TreasureLevel1_EspNOW.enable();
  bool isWiFiConnected = dbc.connectToWiFi();
  while (!isWiFiConnected) {
    Serial.println("Reconnecting..");
    isWiFiConnected = dbc.connectToWiFi();
  }

  GAME_CONSTANTS game_consts = dbc.getGameConstants();
  HTTP_TIMEOUT = game_consts.HTTP_TIMEOUT;
  TREASURE_LEVEL1_INITIAL_HP = game_consts.TREASURE_LEVEL1_INITIAL_HP;
  TREASURE_LEVEL1_ACTION_RECV_WAIT = game_consts.TREASURE_LEVEL1_ACTION_RECV_WAIT;
  TREASURE_LEVEL1_RECOVER_DURATION = game_consts.TREASURE_LEVEL1_RECOVER_DURATION;

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
  switch (setUpDone){
    case 0:
      if(!AdminFunction){
        handleJoystick();
        Treasure.display_not_playing_yet();
        // delay(10000);
        break;
      }
      else{
        TreasureLevel1_Admin.AdminLoop();
      }
    case 1:
      if(!AdminFunction){
        handleJoystick();
        Treasure.display_in_game();
      }
      else{
        TreasureLevel1_Admin.AdminLoop();
      }
      Treasure.receiveAction();
      Treasure.recover();
      break;
  }
}
