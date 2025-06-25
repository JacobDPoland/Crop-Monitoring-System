/*
 * EnviroPro Soil Probe Test Code
 * Tests connection to EnviroPro EP100G series soil probe
 * 
 * Wiring:
 * Red wire: +7V to +16VDC (connect to VIN if using 7-12V power supply)
 * Black wire: GND
 * Blue wire: Digital Pin 2 (DATA_PIN)
 * Yellow wire: Not connected
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
  
  Serial.println("========================================");
  Serial.println("EnviroPro Soil Probe Connection Test");
  Serial.println("========================================");
  Serial.println();
  
  // Test sequence
  testProbeAddress();
  delay(2000);
  testProbeID();
  delay(2000);
  testMoistureReading();
  delay(2000);
  testTemperatureReading();
  delay(2000);
  
  Serial.println("========================================");
  Serial.println("Test complete. You can now send manual commands.");
  Serial.println("Commands to try:");
  Serial.println("  ?!     - Query probe address");
  Serial.println("  CI!    - Get probe ID (address C)");
  Serial.println("  CC0!   - Measure moisture with salinity compensation");
  Serial.println("  CC2!   - Measure temperature in Celsius");
  Serial.println("  CC1!   - Measure salinity");
  Serial.println("========================================");
}

void loop() {
  testMoistureReading();
  delay(2000);
  // Check for incoming commands from Serial Monitor
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.length() > 0) {
      Serial.println("Sending: " + command);
      sendCommand(command);
    }
  }
  
  // Check for responses from probe
  if (mySDI12.available()) {
    char c = mySDI12.read();
    if (c == '\n' || c == '\r') {
      if (sdiResponse.length() > 0) {
        Serial.println("Response: " + sdiResponse);
        sdiResponse = "";
      }
    } else {
      sdiResponse += c;
    }
  }
}

void testProbeAddress() {
  Serial.println("1. Testing probe address query...");
  sendCommand("?!");
  delay(2000);
}

void testProbeID() {
  Serial.println("2. Testing probe ID (address C)...");
  
  // Use probe address C
  sendCommand("CI!");
  delay(2000);
}

void testMoistureReading() {
  Serial.println("3. Testing moisture measurement...");
  
  // Use probe address C
  sendCommand("CC0!");  // Address C - measure moisture with salinity compensation
  delay(2000);         // Wait for measurement (probe says 0002 seconds)
  sendCommand("CD0!");  // Read the data
  delay(2000);
}

void testTemperatureReading() {
  Serial.println("4. Testing temperature measurement...");
  
  // Use probe address C
  sendCommand("CC2!");  // Address C - measure temperature in Celsius
  delay(2000);         // Wait for measurement
  sendCommand("CD0!");  // Read the data
  delay(2000);
}

void sendCommand(String command) {
  mySDI12.sendCommand(command);
  Serial.println("Sent: " + command);
  
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
          Serial.println("Response: " + response);
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
    Serial.println("No response received");
  }
}