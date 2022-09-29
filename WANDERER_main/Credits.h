class Credits{
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
        
        void CreditsLoop(){
            CreditsDisplay();
            handle_joystick();
        }
};

Credits My_Credits;
