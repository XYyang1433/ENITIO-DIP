void clearEEPROM(){
  int i;
  for (i=0; i<EEPROM_SIZE; i++){
    EEPROM.write(i,0);
  }
  EEPROM.commit();
};

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
        bool isSettingGL = false;
        int isGLNav = 1 ;

    public:
        void handleJoystickMain(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos){
                     case up:
                        FunctionNav = max(FunctionNav - 1, 0);
                        Player_joystick.set_state();
                        break;

                    case down:
                        FunctionNav = min(FunctionNav + 1, 2);
                        Player_joystick.set_state();
                        break;

                    case button:
                        switch (FunctionNav)
                        {
                        case FactoryResetFunction:
                            isConfirmingReset = true;
                            break;

                        case SetGLFunction:
                            if (PLAYER.gameStarted) {
                                Admin_OLED.RejectRegisterGLDisplay();
                                delay(1500);
                            }
                            else {
                                isSettingGL = true;
                            }
                            break;

                        case ExitFunction:
                            currentProcess = MainMenuProcess;
                            verified = false ; 
                            FunctionNav = 0;
                            break;
                        
                        default:
                            break;
                        }
                        Player_joystick.set_state();
                        break;

                    case idle:
                        break;

                    default:
                        Player_joystick.set_state();
                        break;
                }
            }
        }

        void handleJoystickPwd(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos){
                    case up:
                        currentDigit ++;
                        if (currentDigit > 9) currentDigit -= 10;
                        if (currentDigit < 0) currentDigit += 10;
                        Player_joystick.set_state();
                        break;

                    case down:
                        currentDigit --;
                        if (currentDigit > 9) currentDigit -= 10;
                        if (currentDigit < 0) currentDigit += 10;
                        Player_joystick.set_state();
                        break;

                    case left:
                        enteringPwdNav = max(enteringPwdNav - 1 , 0) ;
                        Player_joystick.set_state();
                        break;

                    case right:
                        enteringPwdNav = min(enteringPwdNav + 1 , 2) ;
                        Player_joystick.set_state();
                        break;

                    case button:
                        switch (enteringPwdNav)
                        {
                        case 0:
                            currentProcess = MainMenuProcess;
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
                                    Player_Buzzer.sound(NOTE_E4);
                                    delay(1500);
                                    Player_Buzzer.end_sound();
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
                        Player_joystick.set_state();
                        break;

                    case idle:
                        break;

                    default:
                        Player_joystick.set_state();
                        break;
                }
            }
        }

        void handleJoystickConfirmReset() {
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos){
                    case up:
                        ConfirmingResetNav = max(ConfirmingResetNav - 1, 0);
                        Player_joystick.set_state();
                        break;

                    case down:
                        ConfirmingResetNav = min(ConfirmingResetNav + 1, 1);
                        Player_joystick.set_state();
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
                        Player_joystick.set_state();
                        break;

                    case idle:
                        break;
                    
                    default:
                        Player_joystick.set_state();
                        break;
                }
            }
        }

        void handleJoystickSettingGL(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos){
                    case up:
                        isGLNav = max(isGLNav - 1, 0);
                        Player_joystick.set_state();
                        break;

                    case down:
                        isGLNav = min(isGLNav + 1, 1);
                        Player_joystick.set_state();
                        break;

                    case button:
                        EEPROM.write(isGL_add, isGLNav);
                        EEPROM.commit();
                        Admin_OLED.ConfirmSettingGLDisplay(isGLNav);
                        isGLNav = 1;
                        isSettingGL = false;
                        delay(1500);
                        Player_joystick.set_state();
                        break;

                    case idle:
                        break;
                    
                    default:
                        Player_joystick.set_state();
                        break;
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
                else if(isSettingGL){
                    handleJoystickSettingGL();
                    Admin_OLED.display_SettingGL(isGLNav);
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

Admin My_Admin;
