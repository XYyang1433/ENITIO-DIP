#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
    #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


#define FRONT_PIN 15
#define TOP_PIN 2
#define FRONT_NUMPIXELS 10
#define TOP_NUMPIXELS 5

Adafruit_NeoPixel pixelsFRONT(FRONT_NUMPIXELS, FRONT_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixelsTOP(TOP_NUMPIXELS, TOP_PIN, NEO_GRB + NEO_KHZ800);

class NeoPixel {
  public:
    void initialize(){
        // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
        // Any other board, you can remove this part (but no harm leaving it):
        #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
        #endif
        // END of Trinket-specific code.

        pixelsTOP.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
        pixelsFRONT.begin();

        off_FRONT();
        off_TOP();
    }

    void displayRGB_TOP(int R, int G, int B){
        pixelsFRONT.clear();
        for(int i=0; i<TOP_NUMPIXELS; i++) { // For each pixel...
            pixelsTOP.setPixelColor(i, pixelsTOP.Color(R, G, B));
        }
        pixelsTOP.show();   // Send the updated pixel colors to the hardware.
    }

    void off_TOP(){
        pixelsTOP.clear();
        pixelsTOP.show();
    }

    void displayRGB_FRONT(int R, int G, int B){
        pixelsFRONT.clear();
        for(int i=0; i<FRONT_NUMPIXELS; i++) { // For each pixel...
            pixelsFRONT.setPixelColor(i, pixelsFRONT.Color(R, G, B));
        }
        pixelsFRONT.show();   // Send the updated pixel colors to the hardware.
    }

    void off_FRONT(){
        pixelsFRONT.clear();
        pixelsFRONT.show();
    }
};

NeoPixel TreasureLevel2_NeoPixel;
