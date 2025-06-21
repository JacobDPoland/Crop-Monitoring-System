/* Code by Jacob Poland
 * EnviroPro Soil Probe Test Code - Maduino Zero Version
 * Tests connection to EnviroPro EP100G series soil probe
 * 
 * Wiring:
 * Red wire: +7V to +16VDC (connect to VIN if using 7-12V power supply)
 * Black wire: GND
 * Blue wire: Digital Pin 2 (DATA_PIN)
 * 
 * Make sure to install the SDI-12 library:
 * Library Manager > Search "SDI-12" > Install "SDI-12" by Kevin M. Smith
 */

#include <SDI12.h>

#define DATA_PIN 3  // Blue wire connected to digital pin 2
SDI12 mySDI12(DATA_PIN);

String sdiResponse = "";
String myCommand = "";

void setup() {
  SerialUSB.begin(115200);
  while (!SerialUSB && millis() < 10000L) {}  // wait for SerialUSB

  mySDI12.begin();
  delay(500);

  SerialUSB.println("========================================");
  SerialUSB.println("Maduino EnviroPro Soil Probe Connection Test");
  SerialUSB.println("========================================");
  SerialUSB.println();

  testProbeAddress();
  testProbeID();
  testProbeID();
  testProbeID();
  testProbeID();
  testProbeID();
  testProbeID();

  // testMoistureReading();
  // testTemperatureReading();

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
}

void testProbeAddress() {
  SerialUSB.println("1. Testing probe address query...");
  sendCommand("?!");
}

void testProbeID() {
  SerialUSB.println("2. Testing probe ID (address C)...");
  sendCommand("CI!");
}

void sendCommand(String command) {
  SerialUSB.println("Sending: " + command);
  mySDI12.sendCommand(command);

  delay(1000);

  SerialUSB.print("Response (in curly brackets): {");

  // Read with timeout and character counting
  unsigned long startTime = millis();
  int charCount = 0;
  bool dataReceived = false;

  while (millis() - startTime < 10000) {
    while (mySDI12.available()) {
      char c = mySDI12.read();
      SerialUSB.print(c);
      charCount++;
      dataReceived = true;
      startTime = millis();  // Reset timeout when data received
    }
    delay(10);
  }

  SerialUSB.println("}");
  SerialUSB.println("Characters received: " + String(charCount));

  if (!dataReceived) {
    SerialUSB.println("*** NO RESPONSE RECEIVED ***");
  }
}