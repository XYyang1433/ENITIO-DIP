#include "stdio.h"
#include "ENITIO_BLE_UART.h"
std::string txValueString="this is a test to sent ";
u_int8_t txValueint=0;

void setup() {
  Serial.begin(115200);  
  Player_UART.initialise();
  Player_UART.SentValueToPhone(txValueString);

}

void loop() {
  Player_UART.PlayerUARTloop();
  
  
}
