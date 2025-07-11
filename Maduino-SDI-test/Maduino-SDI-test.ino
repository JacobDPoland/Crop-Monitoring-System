/**
 * @example{lineno} a_wild_card.ino
 * @copyright Stroud Water Research Center
 * @license This example is published under the BSD-3 license.
 * @author Kevin M.Smith <SDI12@ethosengineering.org>
 * @date August 2013
 *
 * @brief Example A: Using the Wildcard - Getting Single Sensor Information
 *
 * This is a simple demonstration of the SDI-12 library for Arduino.
 *
 * It requests information about the attached sensor, including its address and
 * manufacturer info.
 */

#include <SDI12.h>

#ifndef SDI12_DATA_PIN
#define SDI12_DATA_PIN 2
#endif
#ifndef SDI12_POWER_PIN
#define SDI12_POWER_PIN -1
#endif

uint32_t SerialUSBBaud = 115200; /*!< The baud rate for the output SerialUSB port */
int8_t   dataPin    = SDI12_DATA_PIN;  /*!< The pin of the SDI-12 data bus */
int8_t   powerPin   = SDI12_POWER_PIN; /*!< The sensor power pin (or -1) */

/** Define the SDI-12 bus */
SDI12 mySDI12(dataPin);

/**
  '?' is a wildcard character which asks any and all sensors to respond
  'I' indicates that the command wants information about the sensor
  '!' finishes the command
*/
String myCommand = "CI!";

void setup() {
  SerialUSB.begin(SerialUSBBaud);
  while (!SerialUSB && millis() < 10000L);

  SerialUSB.println("Opening SDI-12 bus...");
  mySDI12.begin();
  delay(500);  // allow things to settle


}

void loop() {
  
  mySDI12.forceHold();
  mySDI12.sendCommand(myCommand);
  mySDI12.forceListen();
  delay(300);                    // wait a while for a response
  while (mySDI12.available()) {  // write the response to the screen
    int result = mySDI12.read();
    if (result == 127){
      SerialUSB.println("DEL");
    }
    else {
      SerialUSB.println((char) result);
    }
  }
  SerialUSB.println("-");
  delay(1000);  // print again in three seconds
}