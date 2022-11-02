#define SW_PIN 27
#define x_pin  34 // analog pin connected to X output
#define y_pin  35 // analog pin connected to Y output

#define MOVE_BOUNDARY 50
#define RELEASE_BOUNDARY 25

#include <Arduino.h>
#include <cmath>
#include "ENITIO_enums.h"

class Joystick{
    private:
        int state = 0 ;

    public:
        void initialize() {
            pinMode(SW_PIN, INPUT);
            digitalWrite(SW_PIN, LOW);
            pinMode(x_pin, INPUT);
            pinMode(y_pin, INPUT);
        }
        
        joystick_pos read_Joystick(){
            int xValue = -map(analogRead(x_pin), 0, 4095, -100, 100) ;
            int yValue = map(analogRead(y_pin), 0, 4095, -100, 100);
            int SW_state = digitalRead(SW_PIN);
            
            float r = sqrt(pow(xValue, 2) + pow(yValue,2));
            float theta = atan2(yValue,xValue) * 180 / PI;

            if (SW_state){
                return button;
            }
            else if (r <= MOVE_BOUNDARY){
                if (r <= RELEASE_BOUNDARY){
                    state = 0;
                }
                return idle;
            }
            else if ((theta > 45) && (theta <= 135)){
                return up;
            }
            else if ((theta > 135) || (theta <= -135)){
                return left;
            }
            else if ((theta > -135) && (theta <= -45)){
                return down;
            }
            else if ((theta > -45) && (theta <= 45)){
                return right;
            }
        }

        void set_state(){
            state = 1;
        }

        int get_state(){
            return state;
        }
};

Joystick TreasureLevel2_joystick;
