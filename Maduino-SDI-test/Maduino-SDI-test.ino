/*
 * EnviroPro Soil Probe Test Code - Maduino Zero Version
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
SDI12 mySDI12(DATA_PIN);

String sdiResponse = "";
String myCommand = "";

void setup() {
  SerialUSB.begin(115200);
  while (!SerialUSB) {}  // wait for serial
  
  // Initialize SDI-12 with a longer delay for Maduino Zero
  mySDI12.begin();
  delay(1000);  // Increased delay for board initialization
  
  SerialUSB.println("========================================");
  SerialUSB.println("Maduino EnviroPro Soil Probe Connection Test");
  SerialUSB.println("========================================");
  SerialUSB.println();
  
  // Test sequence with longer delays
  testProbeAddress();
  testProbeID();
  
  // testMoistureReading();
  // delay(3000);
  // testTemperatureReading();
  // delay(3000);
  
  SerialUSB.println("========================================");
  SerialUSB.println("Test complete. You can now send manual commands.");
  SerialUSB.println("Commands to try:");
  SerialUSB.println("  ?!     - Query probe address");
  SerialUSB.println("  CI!    - Get probe ID (address C)");
  SerialUSB.println("  CC0!   - Measure moisture with salinity compensation");
  SerialUSB.println("  CC2!   - Measure temperature in Celsius");
  SerialUSB.println("  CC1!   - Measure salinity");
  SerialUSB.println("========================================");
}

void loop() {
  // Check for incoming commands from SerialUSB Monitor
  if (SerialUSB.available()) {
    String command = SerialUSB.readStringUntil('\n');
    command.trim();
    if (command.length() > 0) {
      SerialUSB.println("Sending: " + command);
      sendCommand(command);
    }
  }
  
  // Check for responses from probe
  if (mySDI12.available()) {
    char c = mySDI12.read();
    if (c == '\n' || c == '\r') {
      if (sdiResponse.length() > 0) {
        SerialUSB.println("Response: " + sdiResponse);
        sdiResponse = "";
      }
    } else {
      sdiResponse += c;
    }
  }
}

void testProbeAddress() {
  SerialUSB.println("1. Testing probe address query...");
  sendCommand("?!");
  delay(2000);
}

void testProbeID() {
  SerialUSB.println("2. Testing probe ID (address C)...");
  sendCommand("CI!");
  delay(2000);  // Increased delay to ensure response is captured
}

void testMoistureReading() {
  SerialUSB.println("3. Testing moisture measurement...");
  
  // Start measurement
  sendCommand("CC0!");  // Address C - measure moisture with salinity compensation
  SerialUSB.println("Waiting for measurement to complete...");
  delay(2000);      
  
  // Request data
  SerialUSB.println("Requesting measurement data...");
  sendCommand("CD0!");  // Read the data
  delay(2000);       
}

void testTemperatureReading() {
  SerialUSB.println("4. Testing temperature measurement...");
  
  // Start measurement
  sendCommand("CC2!");  // Address C - measure temperature in Celsius
  SerialUSB.println("Waiting for measurement to complete...");
  delay(2000);       
  
  // Request data
  SerialUSB.println("Requesting measurement data...");
  sendCommand("CD0!"); 
  delay(2000);         
}

void sendCommand(String command) {
  mySDI12.sendCommand(command);
  SerialUSB.println("Sent: " + command);
  
  // Give more time for response on Maduino Zero
  delay(300);  // Increased from 100ms
  
  // Read immediate response if available
  String response = "";
  int timeout = 0;
  while (timeout < 300) {  // Increased timeout to 3 seconds
    if (mySDI12.available()) {
      char c = mySDI12.read();
      if (c == '\n' || c == '\r') {
        if (response.length() > 0) {
          SerialUSB.println("Response: " + response);
          return;  // Exit once we get a complete response
        }
      } else if (c != ' ' && c != 0) {  // Filter out spaces and null characters
        response += c;
      }
    }
    delay(10);
    timeout++;
  }
  
  if (response.length() > 0) {
    SerialUSB.println("Response: " + response);
  } else {
    SerialUSB.println("No response received");
  }
}