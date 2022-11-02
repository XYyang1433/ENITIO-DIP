#include <Arduino.h>
#include <IRremote.h>

#define IR_RECEIVE_PIN     32
#define IR_SEND_PIN        33

#define IR_out_freq  38 // kHz
#define DELAY_AFTER_SEND 20
//#define DELAY_AFTER_RECEIVE 60

struct uint16_hex_digits
{
    unsigned int digit0 = 0;
    unsigned int digit1 = 0;
    unsigned int digit2 = 0;
    unsigned int digit3 = 0;
};

struct ir_signal
{
    uint16_hex_digits address;
    uint16_hex_digits command;
};

class IR{
    public:
        IR(){}

        void enable(void)
        {
            IrReceiver.begin(IR_RECEIVE_PIN);
            IrSender.begin(IR_SEND_PIN);;  // Enable the sensor.
            IrSender.enableIROut(IR_out_freq); 
            IrReceiver.start();
        }


        void send(ir_signal signal, uint8_t sRepeats)
        {
            uint16_t sAddress = encode_uint16(signal.address);
            uint16_t sCommand = encode_uint16(signal.command);
            IrSender.sendNEC(sAddress, sCommand, sRepeats);
            delay(DELAY_AFTER_SEND);
            IrReceiver.restartAfterSend();
        }


        int available(void)
        {   
            return IrReceiver.decode();
        }


        ir_signal read(void)
        {
            uint16_hex_digits address = decode_uint16(IrReceiver.decodedIRData.address);
            uint16_hex_digits command = decode_uint16(IrReceiver.decodedIRData.command);

            ir_signal received_signal_decoded;
            received_signal_decoded.address = address;
            received_signal_decoded.command = command;
            IrReceiver.resume();

            return received_signal_decoded;
        }

        uint16_hex_digits decode_uint16(uint16_t number) {
            uint16_hex_digits decoded_digits;
            decoded_digits.digit0 = number & 0x000F;
            decoded_digits.digit1 = (number >> 4) & 0x000F;
            decoded_digits.digit2 = (number >> 8) & 0x000F;
            decoded_digits.digit3 = (number >> 12) & 0x000F;
            return decoded_digits;
        }

        uint16_t encode_uint16(uint16_hex_digits digits){
            return digits.digit0 + (digits.digit1 << 4) + (digits.digit2 << 8) + (digits.digit3 << 12);
        }
};

IR HealingStation_IR;
