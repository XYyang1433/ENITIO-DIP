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

const int SetGLFunction = 0;
const int FactoryResetFunction = 1;
const int ExitFunction = 2;

void StartUpDisplay(){
  display.clearDisplay();
  display.setTextSize(1); // Draw SIZE

  display.setTextColor(SSD1306_WHITE); 

  display.drawBitmap(0, 4, enitioLogo, 128, 44, WHITE); 

  display.setCursor(0, 56);
  display.println("   Please wait ...   ");
  display.display();
}

void CreditsDisplay() {
    display.clearDisplay();
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
    display.setCursor(0, 0);
    display.println(F("  ENITIO Tech Team  ")); 

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1); // Draw SIZE
    display.setCursor(0, 12);
    display.println("Quan Tran");
    
    display.setCursor(0, 22);
    display.println("Kah Leong");
    display.setCursor(0, 32);
    display.println("Siddhangana");
    display.setCursor(0, 42);
    display.println("Edmund");

    display.setCursor(10, 56);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.println("(Press to go back)"); 
    display.display();
}

class Admin_OLED{
  private:

  public:
    void WrongPwdDisplay(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
  
      display.setTextColor(SSD1306_WHITE); 
      display.setCursor(0, 28);

      display.println("    Wrong Password   ");
      display.display();
    }

    void RejectRegisterGLDisplay() {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE); 
      display.setCursor(0, 22);
                      
      display.println("Unable to Register GL");

      display.setCursor(0, 34);
      display.println("  after game started ");
      display.display();
    }

    void ConfirmSettingGLDisplay(int isGL){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE); 
      display.setCursor(0, 22);
                      
      display.println("    Registered as    ");

      display.setCursor(0, 34);
      if (isGL) display.println("     Group Leader    ");
                                
      else display.println("       Freshman      ");
      display.display();
    }

    void display_EnteringPwd(int currentDigitIndex, int currentDigit, int enteringPwdNav, int *user_key_in_pwd){
      display.clearDisplay();

      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("Please Enter Password")); 

      int i;
      for(i = 0; i < 6; i++){
        display.setCursor(5+15*(i+1), 28);
        if (currentDigitIndex < i){
          display.setTextColor(SSD1306_WHITE);
          display.print("*");
        }
        else if (currentDigitIndex == i){
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(currentDigit);
        }
        else {
          display.setTextColor(SSD1306_WHITE);
          display.print(user_key_in_pwd[i]);
        }
      }

      display.setCursor(0, 56);
      switch (enteringPwdNav)
      {
      case 0:
        display.setTextColor(SSD1306_WHITE);
        display.print(" "); 
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.print("Back to Main Menu"); 
        display.setTextColor(SSD1306_WHITE);
        display.print(" >"); 
        break;
      
      case 1:
        display.setTextColor(SSD1306_WHITE);
        display.print(" <      Enter      > "); 
        break;

      case 2: 
        display.setTextColor(SSD1306_WHITE);
        display.print(" <          "); 
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.print("Backspace"); 
        break;

      default:
        break;
      }

      display.display();
    }

    void display_MainAdmin(int FunctionNav){
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("        Admin        ")); 

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 12);
      display.println("Register Role");
      display.setCursor(10, 22);
      display.println("Factory Reset");

      display.setCursor(10, 42);
      display.println("Back to Main Menu");

      switch (FunctionNav)
      {
      case SetGLFunction:
        display.setCursor(2, 12);
        display.println(">");
        break;
      
      case FactoryResetFunction:
        display.setCursor(2, 22);
        display.println(">");
        break;

      case ExitFunction:
        display.setCursor(2, 42);
        display.println(">");
        break;

      default:
        break;
      }

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)"); 

      display.display();
    }

    void display_ConfirmingReset(int ConfirmingResetNav){
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F(" Confirm to Reset?   ")); 
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 12);
      display.println("NO");
      display.setCursor(10, 22);
      display.println("YES");

      switch (ConfirmingResetNav)
      {
      case 0:
        display.setCursor(2, 12);
        display.println(">");
        break;
      
      case 1:
        display.setCursor(2, 22);
        display.println(">");
        break;

      default:
        break;
      }

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)"); 

      display.display();
    }

    void display_SettingGL(int isGLNav){
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F(" Choose your Role... ")); 
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 12);
      display.println("Freshman (default)");
      display.setCursor(10, 22);
      display.println("Group Leader");     

      switch (isGLNav)
      {
      case 0:
        display.setCursor(2, 12);
        display.println(">");
        break;
      
      case 1:
        display.setCursor(2, 22);
        display.println(">");
        break;

      default:
        break;
      }

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)"); 

      display.display();
    }
};

Admin_OLED Admin_OLED;

class MainMenu_OLED {
  private:

  public:
    void display_menu(int ProcessNav){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE); 
  
      display.drawBitmap(0, 4, enitioLogo, 128, 44, WHITE); 

      switch (ProcessNav) {
        case MainMenuProcess:
          display.setCursor(26, 56);
          display.println(F("< Main Menu >"));
          break;

        case ProfileProcess:
          display.setCursor(34, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Profile"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case TreasureHuntProcess:
          display.setCursor(15, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Treasure Hunt"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case SnakeGameProcess:
          display.setCursor(40, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Snake"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case MusicProcess:
          display.setCursor(40, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Music"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case AdminProcess:
          display.setCursor(40, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Admin"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

        case CreditProcess:
          display.setCursor(37, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Credit"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;
      }
      display.display();
    }
};

MainMenu_OLED MainMenu_OLED;

class Profile_OLED {
  private:

  public:
    void display_CompleteProfilePage(int OG, int isGL){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("     Your Profile    ")); 

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 12);
      display.println("ENITIO 2022");

      display.setCursor(0, 24);

      if (isGL == 0){
        display.println("Freshman"); 
      }
      else {
        display.println("Group Leader");
      }

      display.setCursor(0, 36);
      switch (OG){
          case ALATAR:
            display.println("OG: Alatar ");
            break;
          case DRACHEN:
            display.println("OG: Drachen ");
            break;
          case EVA:
            display.println("OG: Eva ");
            break;
          case INVICTA:
            display.println("OG: Invicta ");
            break;
        }

      display.setCursor(10, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to go back)"); 
      display.display();
    }

    void display_OGregisteringPage(int OG_pointer){
      display.clearDisplay();
      // change text display here
      display.setTextSize(1);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);

      display.setCursor(0,0);
      // menu title
      display.println(" Choose your OG...   ");
      //---------------------------------

      display.setTextColor(SSD1306_WHITE);

      display.setCursor(10, 12);
      
      display.println("ALATAR");

      display.setCursor(10, 22);
      
      display.println("DRACHEN");

      display.setCursor(10, 32);
      
      display.println("EVA");

      display.setCursor(10, 42);
      
      display.println("INVICTA");
      
      // prints the cursor to highlight menu items
      display.setCursor(2, (OG_pointer * 10) + 12);
      display.println(">");

      display.setCursor(14, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to choose)"); 

      display.display();
    }

    void interim_registering_display(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("     Your Profile    ")); 

      display.setCursor(0, 28);
      display.setTextColor(SSD1306_WHITE);
      display.println(F("   Please wait ...   ")); 
      display.display();
    }

    // void display_isGLregisteringPage(int isGL_pointer){
    //   display.clearDisplay();
    //   // change text display here
    //   display.setTextSize(1);
    //   display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);

    //   display.setCursor(0,0);
    //   // menu title
    //   display.println(" Are you a GL?       ");
    //   //---------------------------------

    //   display.setTextColor(SSD1306_WHITE);

    //   display.setCursor(10, 12);
      
    //   display.println("NO");

    //   display.setCursor(10, 22);
      
    //   display.println("YES");
      
    //   // prints the cursor to highlight menu items
    //   display.setCursor(2, (isGL_pointer * 10) + 12);
    //   display.println(">");

    //   display.setCursor(14, 56);
    //   display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    //   display.println("(Press to choose)"); 

    //   display.display();
    // }
};

Profile_OLED Profile_OLED;

const int mainPage = 0;
const int powerupPage = 1;
const int infoPage = 2;
const int achievementPage = 3;
const int exitPage = 4;

class TreasureHunt_OLED {
  private:
    String lastNoti;
    
  public:
    
    void display_mainPage(int HP, int En, String noti, int pageNav) {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      if (noti.length() == 0)
      {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("  Treasure Hunt Game ")); 
      }
      else 
      {
        display.setTextColor(SSD1306_WHITE); 
        display.setCursor(0, 0);
        display.println(noti); 
      }
      
    
      display.setCursor(0, 12);
      display.setTextSize(2);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
    
      int i;
      if (HP <= 12){
        display.println("HP ");
        for (i = 0; i < HP; i ++) {
          display.fillRect(34+i*8, 12, 4, 16, SSD1306_WHITE);
        }
      }
      else {
        display.print("HP ");
        display.println(HP);
      }
            
      display.setCursor(0, 32);
    
      if (En <= 12) {
        display.println("EN ");
        for (i = 0; i < En; i ++) {
          display.fillRect(34+i*8, 32, 4, 16, SSD1306_WHITE);
        }
      }
      else {
        display.print("EN ");
        display.println(En);
      }
    
      display.setTextSize(1);

      switch (pageNav){
        case mainPage:
          display.setCursor(44, 56);
          display.println(F("< Main >"));
          break;

         case powerupPage:
          display.setCursor(30, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Power-Up"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

         case infoPage:
          display.setCursor(44, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Info"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

         case achievementPage:
          display.setCursor(20, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Achievement"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;

         case exitPage:
          display.setCursor(44, 56);
          display.print(F("< "));
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.print(F("Exit"));
          display.setTextColor(SSD1306_WHITE);
          display.print(F(" >"));
          break;
      }
      display.display();
    }

    void display_powerupPage(int num_bonus6HP, 
                            int num_bonus1MaxEn,
                            int num_bonus1MANA,
                            int num_fiveminx2EnRegen,
                            int num_bomb,
                            String noti,
                            int PowerUpNav){

      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      if (noti.length() == 0)
      {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("  Treasure Hunt Game ")); 
      }
      else 
      {
        display.setTextColor(SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(noti); 
      }

      display.setCursor(0, 12);
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text      

      switch (PowerUpNav)
      {
      case bonus6HP:
        display.println((" Give 6 HP instantly ")); 
        display.setCursor(0, 36);
        display.print(("You have "));    
        display.print(num_bonus6HP);
        display.println((" of this!"));   
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.setCursor(0, 56);
        display.println("    Press to use!    ");
        break;

      case bonus1MaxEn:
        display.println(("  Bonus 1 Max Energy ")); 
        display.setCursor(0, 36);
        display.print(("You have "));    
        display.print(num_bonus1MaxEn);
        display.println((" of this!"));   
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.setCursor(0, 56);
        display.println("    Press to use!    ");
        break;

      case bonus1MANA:
        display.println(("     Bonus 1 MANA    ")); 
        display.setCursor(0, 36);
        display.print(("You have "));    
        display.print(num_bonus1MANA);
        display.println((" of this!"));   
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.setCursor(0, 56);
        display.println("    Press to use!    ");
        break;

      case fiveminx2EnRegen:
        display.println((" Double Energy Regen ")); 
        display.setCursor(0, 22);
        display.println(("       in 5 min      ")); 
        display.setCursor(0, 36);
        display.print(("You have "));    
        display.print(num_fiveminx2EnRegen);
        display.println((" of this!"));   
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.setCursor(0, 56);
        display.println("    Press to use!    ");
        break;

      case bomb:
        display.println(("        Bomb!!       ")); 
        display.setCursor(0, 22);
        display.println(("  Damage surrounding ")); 
        display.setCursor(0, 36);
        display.print(("You have "));    
        display.print(num_bomb);
        display.println((" of this!"));   
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.setCursor(0, 56);
        display.println("    Press to use!    ");
        break;

      case 0:
        display.println(("  Use Left and Right ")); 
        display.setCursor(0, 22);
        display.println(("      to Navigate    ")); 
        display.setCursor(0, 32);
        display.println(("through each power-up")); 
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.setCursor(0, 56);
        display.println("  Press to go back!  ");
      
      default:
        break;
      }
      display.display();
    }

    void display_infoPage(int OG, int ID, int MANA, int MaxEn, String noti, int pageNav)
    {
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      if (noti.length() == 0)
      {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("  Treasure Hunt Game ")); 
      }
      else 
      {
        display.setTextColor(SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(noti); 
      }

        display.setCursor(0, 10);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text      

        switch (OG){
          case ALATAR:
            display.println("OG: Alatar ");
            break;
          case DRACHEN:
            display.println("OG: Drachen ");
            break;
          case EVA:
            display.println("OG: Eva ");
            break;
          case INVICTA:
            display.println("OG: Invicta ");
            break;
        }

        display.setCursor(0, 20);
        display.print("ID: ");
        display.println(ID);

        display.setCursor(0, 30);
        display.print("MANA: ");
        display.println(MANA);

        display.setCursor(0, 40);
        display.print("Max En: ");
        display.println(MaxEn);

        switch (pageNav){
          case mainPage:
            display.setCursor(44, 56);
            display.print(F("< "));
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.print(F("Main"));
            display.setTextColor(SSD1306_WHITE);
            display.print(F(" >"));
            break;

          case powerupPage:
            display.setCursor(30, 56);
            display.print(F("< "));
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.print(F("Power-Up"));
            display.setTextColor(SSD1306_WHITE);
            display.print(F(" >"));
            break;
          
          case infoPage:
            display.setCursor(44, 56);
            display.println(F("< Info >"));
            break;

          case achievementPage:
            display.setCursor(20, 56);
            display.print(F("< "));
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.print(F("Achievement"));
            display.setTextColor(SSD1306_WHITE);
            display.print(F(" >"));
            break;

          case exitPage:
            display.setCursor(44, 56);
            display.print(F("< "));
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.print(F("Exit"));
            display.setTextColor(SSD1306_WHITE);
            display.print(F(" >"));
            break;
      }
      display.display();
    }

    void display_achievementPage(int numKilled, int numL1Treasure, int numL2Treasure, String noti, int pageNav){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE
      if (noti.length() == 0)
      {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(F("  Treasure Hunt Game ")); 
      }
      else 
      {
        display.setTextColor(SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(0, 0);
        display.println(noti); 
      }

      display.setCursor(0, 12);
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text      

      display.print("Killed: ");
      display.println(numKilled);

      display.setCursor(0, 24);
      display.print("L1 Treasure: ");
      display.println(numL1Treasure);

      display.setCursor(0, 36);
      display.print("L2 Treasure: ");
      display.println(numL2Treasure);

      switch (pageNav){
          case mainPage:
            display.setCursor(44, 56);
            display.print(F("< "));
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.print(F("Main"));
            display.setTextColor(SSD1306_WHITE);
            display.print(F(" >"));
            break;

          case powerupPage:
            display.setCursor(30, 56);
            display.print(F("< "));
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.print(F("Power-Up"));
            display.setTextColor(SSD1306_WHITE);
            display.print(F(" >"));
            break;
          
          case infoPage:
            display.setCursor(44, 56);
            display.print(F("< "));
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.print(F("Info"));
            display.setTextColor(SSD1306_WHITE);
            display.print(F(" >"));
            break;

          case achievementPage:
            display.setCursor(20, 56);
            display.println(F("< Achievement >"));
            break;

          case exitPage:
            display.setCursor(44, 56);
            display.print(F("< "));
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.print(F("Exit"));
            display.setTextColor(SSD1306_WHITE);
            display.print(F(" >"));
            break;
      }

      display.display();
    };

    void display_WaitingPage(){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F("  Treasure Hunt Game ")); 

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 24);
      display.println("   The game has not  ");
      display.println("     started yet.    "); 

      display.setCursor(10, 56);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println("(Press to go back)"); 
      display.display();
    }
};

TreasureHunt_OLED TreasureHunt_OLED;
