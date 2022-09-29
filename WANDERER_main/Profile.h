class Profile{
    private:
        int OG_pointer = 0;
        int OG;
        bool registerStatus = false;
        // int isGL_pointer = 0;
        // int isGL;
        // int currentRegisteringPage = 0;
    
    public:
        // void reset(){
        //   OG_pointer = 0;
        //   isGL_pointer = 0;
        //   currentRegisteringPage = 0;
        // }
    
        void handleJoystickRegistered(){
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
        }

        void handleJoystickRegisteringOG(){
            joystick_pos joystick_pos = Player_joystick.read_Joystick();
            if (Player_joystick.get_state() == 0) {
                switch (joystick_pos)
                { 
                case button:
                    Profile_OLED.interim_registering_display();
                    Player_joystick.set_state();
                    OG = OG_pointer;
                    registerStatus = dbc.registerWanderer(OG, my_MAC_address); 
                    while (!registerStatus){
                        delay(10000);
                        registerStatus = dbc.registerWanderer(OG, my_MAC_address); 
                    }
                    EEPROM.write(OG_add, OG);
                    EEPROM.write(PROFILE_enable_add, 1);
                    if (EEPROM.read(isGL_add) != 1) EEPROM.write(isGL_add, 0);
                    EEPROM.commit();
                    Player_joystick.set_state();
                    break;

                case up:
                    OG_pointer = max(OG_pointer - 1, 0);
                    Player_joystick.set_state();
                    break;

                case down:
                    OG_pointer = min(OG_pointer + 1, 3);
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

        // void handleJoystickRegisteringisGL(){
        //     joystick_pos joystick_pos = Player_joystick.read_Joystick();
        //     if (Player_joystick.get_state() == 0) {
        //         switch (joystick_pos)
        //         { 
        //         case up:
        //             isGL_pointer = max(isGL_pointer - 1, 0);
        //             Player_joystick.set_state();
        //             break;

        //         case down:
        //             isGL_pointer = min(isGL_pointer + 1, 1);
        //             Player_joystick.set_state();
        //             break;

        //         case button:
        //             isGL = isGL_pointer;
        //             EEPROM.write(isGL_add, isGL);
        //             EEPROM.write(OG_add, OG);
        //             dbc.registerWanderer(OG, my_MAC_address); 
        //             EEPROM.write(PROFILE_enable_add, 1);
        //             EEPROM.commit();
        //             Player_joystick.set_state();
        //             break;

        //         default:
        //             break;
        //         }
        //     }
        // }

        void ProfileLoop(){
            if (EEPROM.read(PROFILE_enable_add) == 0){
                handleJoystickRegisteringOG();
                Profile_OLED.display_OGregisteringPage(OG_pointer);
            }
            else {
                handleJoystickRegistered();
                int OG_ = EEPROM.read(OG_add);
                int isGL_ = EEPROM.read(isGL_add);
                Profile_OLED.display_CompleteProfilePage(OG_, isGL_);
            }
        }
};

Profile My_Profile;
