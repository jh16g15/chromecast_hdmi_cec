#include <Arduino.h>
#include <IRremote.hpp>
#include "AK59-00149A_codes.h"

#define IR_RECEIVE_PIN 2 // To be compatible with interrupt example, pin 2 is chosen here.

/*
 * Helper macro for getting a macro definition as string
 */
#if !defined(STR_HELPER)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif

// Based on SimpleReceiver.ino Example from IRremote library 4.4.1

void setup() {
  Serial.begin(115200);
  // while (!Serial); // wait for serial to become available (delete when headless)
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));  
  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  
  Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
  // Serial.println(F("Using IR receiver on pin " STR(IR_RECEIVE_PIN)));
}

void loop() {
  if (IrReceiver.decode()) {
    IrReceiver.resume(); // Early enable receiving of the next IR frame
    Serial.println("Result Short:");
    IrReceiver.printIRResultShort(&Serial);
    // Serial.println("How to Send:");
    // IrReceiver.printIRSendUsage(&Serial);

    Serial.println();

    auto ir_code = IrReceiver.decodedIRData.decodedRawData; // uint32_t as 8-bit AVR
    switch(ir_code){
      case AK59_00149A_PLAY:
        Serial.println("Play");
        break;
      case AK59_00149A_PAUSE:
        Serial.println("Pause");
        break;
      case AK59_00149A_FORWARD:
        Serial.println("Forward");
        break;
      case AK59_00149A_BACKWARD:
        Serial.println("Backward");
        break;
        
    }


      /*
        * Finally, check the received data and perform actions according to the received command
        */
      if (IrReceiver.decodedIRData.command == 0x10) {
          // do something
      } else if (IrReceiver.decodedIRData.command == 0x11) {
          // do something else
      }
    }

}
