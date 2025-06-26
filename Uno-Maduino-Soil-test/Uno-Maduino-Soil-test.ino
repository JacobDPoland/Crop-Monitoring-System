/*
 * This code is used to test the connection
 * between an EnviroPro sensor, an Arduino Uno,
 * and a Maduino Zero. 
 * 
 * Make sure to install the SDI-12 library:
 * Library Manager > Search "SDI-12" > Install "SDI-12" by Kevin M. Smith
 */

#include <SDI12.h>

#define DATA_PIN 2        // Blue wire connected to digital pin 2
#define POWER_PIN -1      // Set to a pin number if you want to control power, -1 if always powered

SDI12 mySDI12(DATA_PIN);

String sdiResponse = "";
String myCommand = "";

void setup() {
  Serial.begin(9600);
  mySDI12.begin();
  
  // Small delay to let everything initialize
  delay(500);
}

void loop() {
  takeMoistureReading();
  // delay(2000);
  takeTemperatureReading();
  // delay(2000);
}

void testProbeAddress() {
  Serial.println("1. Testing probe address query...");
  sendCommand("?!");
  delay(2000);
}

void testProbeID() {
  Serial.println("2. Testing probe ID (address C)...");
  
  sendCommand("CI!");
  delay(2000);
}

void takeMoistureReading() {  
  sendCommand("CC0!");  // Address C - measure moisture with salinity compensation
  delay(2000);         // Wait for measurement (probe says 0002 seconds)
  String response = sendCommand("CD0!");
  response.replace("+", ",+");
  response.replace("-", ",-");
  response.replace("C", "");
  Serial.println("Moist" + response);  // Read the data
  delay(2000);
}

void takeTemperatureReading() {
  sendCommand("CC2!");  // Address C - measure temperature in Celsius
  delay(2000);         // Wait for measurement
  String response = sendCommand("CD0!");
  response.replace("+", ",+");
  response.replace("-", ",-");
  response.replace("C", "");
  Serial.println("Temp" + response);  // Read the data
  delay(2000);
}

String sendCommand(String command) {
  mySDI12.sendCommand(command);
  
  // Give some time for response
  delay(100);
  
  // Read immediate response if available
  String response = "";
  int timeout = 0;
  while (timeout < 100) {  // 1 second timeout
    if (mySDI12.available()) {
      char c = mySDI12.read();
      if (c == '\n' || c == '\r') {
        if (response.length() > 0) {
          break;
        }
      } else {
        response += c;
      }
    }
    delay(10);
    timeout++;
  }
  
  if (response.length() == 0) {
    return "No response received";
  }
  return response;
}