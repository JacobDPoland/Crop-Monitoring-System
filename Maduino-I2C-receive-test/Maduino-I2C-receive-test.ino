#include <Wire.h>

#define SLAVE_ADDRESS 0x08  // This device's I2C address

void setup() {
  Wire.begin(SLAVE_ADDRESS);     // Initialize I2C as slave with address 0x08
  Wire.onReceive(receiveEvent);  // Register function to handle incoming data
  SerialUSB.begin(115200);            // Initialize SerialUSB communication
  SerialUSB.println("Maduino Zero I2C Slave Ready");
  SerialUSB.println("Waiting for data from Arduino Uno...");
}

void loop() {
  // Main loop can do other tasks
  // I2C communication is handled by interrupt
  delay(500);
}

// Function called when I2C data is received
void receiveEvent(int numBytes) {
  String receivedString = "";
  
  // Read all incoming bytes and build the string
  while (Wire.available()) {
    char c = Wire.read();
    receivedString += c;
  }
  
  // Print the received string
  SerialUSB.print("Received: ");
  SerialUSB.println(receivedString);
  SerialUSB.print("Bytes received: ");
  SerialUSB.println(numBytes);
  SerialUSB.println("---");
}