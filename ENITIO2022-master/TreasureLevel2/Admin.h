void clearEEPROM(){
  int i;
  for (i=0; i<EEPROM_SIZE; i++){
    EEPROM.write(i,0);
  }
  EEPROM.commit();
};

const int FactoryResetFunction = 1;
const int SetIDFunction = 0;
const int UncollectTreasureFunction = 2;
const int ExitFunction = 3;
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

    void ConfirmSettingIDDisplay(int currentIDnum){
      display.clearDisplay();
      display.setTextSize(1); // Draw SIZE

      display.setTextColor(SSD1306_WHITE); 
      display.setCursor(0, 22);
                      
      display.print("Treasure ID set as ");
      display.println(currentIDnum);

      display.setCursor(0, 34);
      
      if (currentIDnum <= TREASURE_VIRUS_THRESHOLD) display.println("    Type: Treasure   ");         
      else display.println("     Type: Virus     ");
      
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
      display.println("Register ID");
      display.setCursor(10, 22);
      display.println("Factory Reset");
      display.setCursor(10, 32);
      display.println("Uncollect Treasure");
      display.setCursor(10, 42);
      display.println("Back to Main Menu");

      switch (FunctionNav)
      {
      case SetIDFunction:
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

      case UncollectTreasureFunction:
        display.setCursor(2, 32);
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

    void display_SettingID(int previousID, int currentIDnum){
      display.clearDisplay();
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.setCursor(0, 0);
      display.println(F(" Setting Treasure ID ")); 
      if (!gameStarted){
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 12);
        display.print("Old ID: ");
        display.println(previousID);
        display.setCursor(0, 22);
        display.print("Old Type: ");  
        if (previousID <= TREASURE_VIRUS_THRESHOLD) display.print("Treasure");
        else display.print("Virus");

        display.setCursor(0, 40);
        display.print("New ID: "); 
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println(currentIDnum); 

        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 50);
        display.print("New Type: "); 
        if (currentIDnum <= TREASURE_VIRUS_THRESHOLD) display.print("Treasure");
        else display.print("Virus");       
      }
      else{
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 12);
        display.print("ID: ");
        display.println(previousID);
        display.setCursor(0, 22);
        display.print("Type: ");  
        if (previousID <= TREASURE_VIRUS_THRESHOLD) display.print("Treasure");
        else display.print("Virus");

        display.setCursor(0, 40);
        display.print("Game Started!!"); 

        display.setCursor(10, 56);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println("(Press to go back)"); 
      }
      display.display();
    }
};

Admin_OLED Admin_OLED;

class Admin {
    private:
        int admin_pwd[6] = {1, 0, 0, 0, 0, 9};
        int user_key_in_pwd[6] = {};
        int currentDigitIndex = 0;
        int currentDigit = 0;
        int enteringPwdNav = 1 ;
        bool verified = false ; 
        int FunctionNav = 0;
        bool isConfirmingReset = false;
        int ConfirmingResetNav = 1;
        bool isSettingID = false;
        bool isUnSettingTreasure = false;
        int currentIDnum = 0 ;

    public:
        void handleJoystickMain(){
            joystick_pos joystick_pos = TreasureLevel2_joystick.read_Joystick();
            if (TreasureLevel2_joystick.get_state() == 0) {
                switch (joystick_pos){
                     case up:
                        FunctionNav = max(FunctionNav - 1, 0);
                        TreasureLevel2_joystick.set_state();
                        break;

                    case down:
                        FunctionNav = min(FunctionNav + 1, 3);
                        TreasureLevel2_joystick.set_state();
                        break;

                    case button:
                        switch (FunctionNav)
                        {
                        case FactoryResetFunction:
                            isConfirmingReset = true;
                            break;

                        case SetIDFunction:
                            isSettingID = true;
                            break;

                        case ExitFunction:
                            AdminFunction = false;
                            verified = false ; 
                            FunctionNav = 0;
                            break;

                        case UncollectTreasureFunction:
                            EEPROM.write(ENABLE_add, 1);
                            EEPROM.write(HP_add, TREASURE_LEVEL2_INITIAL_HP);
                            EEPROM.commit();
                            ESP.restart();
                            break;
                        
                        default:
                            break;
                        }
                        TreasureLevel2_joystick.set_state();
                        break;

                    case idle:
                        break;

                    default:
                        TreasureLevel2_joystick.set_state();
                        break;
                }
            }
        }

        void handleJoystickPwd(){
            joystick_pos joystick_pos = TreasureLevel2_joystick.read_Joystick();
            if (TreasureLevel2_joystick.get_state() == 0) {
                switch (joystick_pos){
                    case up:
                        currentDigit ++;
                        if (currentDigit > 9) currentDigit -= 10;
                        if (currentDigit < 0) currentDigit += 10;
                        TreasureLevel2_joystick.set_state();
                        break;

                    case down:
                        currentDigit --;
                        if (currentDigit > 9) currentDigit -= 10;
                        if (currentDigit < 0) currentDigit += 10;
                        TreasureLevel2_joystick.set_state();
                        break;

                    case left:
                        enteringPwdNav = max(enteringPwdNav - 1 , 0) ;
                        TreasureLevel2_joystick.set_state();
                        break;

                    case right:
                        enteringPwdNav = min(enteringPwdNav + 1 , 2) ;
                        TreasureLevel2_joystick.set_state();
                        break;

                    case button:
                        switch (enteringPwdNav)
                        {
                        case 0:
                            AdminFunction = false;
                            enteringPwdNav = 1;
                            currentDigit = 0;
                            currentDigitIndex = 0;
                            break;
                        
                        case 1:
                            if (currentDigitIndex < 5){
                                user_key_in_pwd[currentDigitIndex] = currentDigit ; 
                                currentDigitIndex ++ ;
                                currentDigit = 0;
                            }
                            else {
                                user_key_in_pwd[currentDigitIndex] = currentDigit ; 
                                currentDigit = 0;
                                currentDigitIndex = 0;
                                int pwd_check = 1 ;
                                int i ;
                                for (i = 0; i < 6; i++){
                                    pwd_check *= (int) (user_key_in_pwd[i] == admin_pwd[i]);
                                }
                                if (pwd_check){
                                    verified = true;
                                }
                                else{
                                    Admin_OLED.WrongPwdDisplay();
                                    delay(1500);
                                }
                            }
                            break;

                        case 2:
                            currentDigitIndex = max(currentDigitIndex - 1, 0);
                            currentDigit = 0;
                            enteringPwdNav = 1;
                            break;

                        default:
                            break;
                        }
                        TreasureLevel2_joystick.set_state();
                        break;

                    case idle:
                      break;

                    default:
                        TreasureLevel2_joystick.set_state();
                        break;
                }
            }
        }

        void handleJoystickConfirmReset() {
            joystick_pos joystick_pos = TreasureLevel2_joystick.read_Joystick();
            if (TreasureLevel2_joystick.get_state() == 0) {
                switch (joystick_pos){
                    case up:
                        ConfirmingResetNav = max(ConfirmingResetNav - 1, 0);
                        TreasureLevel2_joystick.set_state();
                        break;

                    case down:
                        ConfirmingResetNav = min(ConfirmingResetNav + 1, 1);
                        TreasureLevel2_joystick.set_state();
                        break;

                    case button:
                        switch (ConfirmingResetNav)
                        {
                        case 0:
                            isConfirmingReset = false;
                            ConfirmingResetNav = 1;
                            break;

                        case 1:
                            StartUpDisplay();
                            clearEEPROM();
                            ESP.restart();
                            break;
                        
                        default:
                            break;
                        }
                        TreasureLevel2_joystick.set_state();
                        break;

                    case idle:
                      break;
                    
                    default:
                        TreasureLevel2_joystick.set_state();
                        break;
                }
            }
        }

        void handleJoystickSettingID(){
            joystick_pos joystick_pos = TreasureLevel2_joystick.read_Joystick();
            if (TreasureLevel2_joystick.get_state() == 0) {
                if (!gameStarted) {
                    switch (joystick_pos){
                        case down:
                            currentIDnum = max(currentIDnum - 1, 0);
                            TreasureLevel2_joystick.set_state();
                            break;

                        case up:
                            currentIDnum = min(currentIDnum + 1, NUM_L2TREASURES);
                            TreasureLevel2_joystick.set_state();
                            break;

                        case button:
                            id = currentIDnum;
                            EEPROM.write(ID_add, id);
                            EEPROM.commit();
                            Admin_OLED.ConfirmSettingIDDisplay(currentIDnum);
                            currentIDnum = 0;
                            isSettingID = false;
                            delay(1500);
                            TreasureLevel2_joystick.set_state();
                            break;

                        case idle:
                          break;

                        default:
                            TreasureLevel2_joystick.set_state();
                            break;
                    }
                }
                else {
                  switch (joystick_pos){
                    case button:
                        isSettingID = false;
                        TreasureLevel2_joystick.set_state();
                        break;

                    case idle:
                      break;

                    default:
                        TreasureLevel2_joystick.set_state();
                        break;
                  }
                }
            }
        }

        void AdminLoop(){
            switch (verified)
            {
            case 0:
                handleJoystickPwd();
                Admin_OLED.display_EnteringPwd(currentDigitIndex, currentDigit, enteringPwdNav, user_key_in_pwd);
                break;

            case 1:
                if (isConfirmingReset){
                    handleJoystickConfirmReset();
                    Admin_OLED.display_ConfirmingReset(ConfirmingResetNav);
                }
                else if(isSettingID){
                    handleJoystickSettingID();
                    int previousID = EEPROM.read(ID_add);
                    Admin_OLED.display_SettingID(previousID, currentIDnum);
                }
                else {
                    handleJoystickMain();
                    Admin_OLED.display_MainAdmin(FunctionNav);
                }
                break;
            
            default:
                break;
            }
        }
};

Admin TreasureLevel2_Admin;
