#include <Wire.h>

#define SLAVE_ADDRESS 0x08  // I2C address of the Maduino Zero

void setup() {
  Wire.begin();        // Initialize I2C as master
  Serial.begin(9600);  // Initialize serial communication
  Serial.println("Arduino Uno I2C Master Ready");
  Serial.println("Sending data every 2 seconds...");
}

void loop() {
  String message = "Hello from Arduino Uno!";
  
  // Begin transmission to slave device
  Wire.beginTransmission(SLAVE_ADDRESS);
  
  delayMicroseconds(10);
  // Send the string data
  Wire.write(message.c_str(), message.length());
  // End transmission
  byte error = Wire.endTransmission();
  
  // Check for transmission errors
  if (error == 0) {
    Serial.println("Data sent successfully: " + message);
  } else {
    Serial.print("Transmission error: ");
    Serial.println(error);
  }
  
  delay(2000);  // Wait 2 seconds before sending next message
}
