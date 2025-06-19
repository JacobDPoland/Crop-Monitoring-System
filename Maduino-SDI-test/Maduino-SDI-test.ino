#include <SDI12.h>

#define DATA_PIN 2        // SDI-12 Data (Blue wire)

SDI12 mySDI12(DATA_PIN);
String sdiResponse = "";
String myCommand = "";

void setup() {
  SerialUSB.begin(115200);  // Use SerialUSB on Maduino Zero
  while (!SerialUSB);       // Wait for USB to connect

  mySDI12.begin();
  delay(500);               // Give probe time to stabilize

  SerialUSB.println("========================================");
  SerialUSB.println("EnviroPro Soil Probe - Maduino Zero Test");
  SerialUSB.println("========================================");
  SerialUSB.println("Type commands like CI!, CC0!, CD0!, etc.");
  SerialUSB.println();
}

void loop() {
  // Read from USB Serial
  if (SerialUSB.available()) {
    String command = SerialUSB.readStringUntil('\n');
    command.trim();
    if (command.length() > 0) {
      SerialUSB.println("Sending: " + command);
      sendCommand(command);
    }
  }

  // Read probe response
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

void sendCommand(String command) {
  mySDI12.sendCommand(command);
  delay(100);  // SDI-12 spec requires ≥10ms pause before reading

  // Wait for probe to respond (max 1s)
  String response = "";
  int timeout = 0;
  while (timeout < 100) {  // 100 × 10ms = 1s
    if (mySDI12.available()) {
      char c = mySDI12.read();
      if (c == '\n' || c == '\r') {
        if (response.length() > 0) {
          SerialUSB.println("Response: " + response);
          break;
        }
      } else {
        response += c;
      }
    } else {
      delay(10);
      timeout++;
    }
  }

  if (response.length() == 0) {
    SerialUSB.println("No response received.");
  }
}
