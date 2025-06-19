/*
 * Simplified Soil Sensor Reader
 * Turns on sensor, waits, reads once, displays output
 * Runs only once in setup() - no looping
 */

#include <SDI12.h>

#define DATA_PIN 2        // Blue wire to digital pin 2
#define POWER_PIN 3       // Controls relay power to sensor

SDI12 mySDI12(DATA_PIN);

void setup() {
  Serial.begin(9600);
  
  // Turn on sensor power
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);  // Turn on relay (inverted logic)
  Serial.println("Sensor powered on");
  
  // Wait for sensor to stabilize
  delay(5000);
  
  // Initialize SDI-12 communication
  mySDI12.begin();
  
  // Read soil moisture
  for (int i = 0; i < 5; i++){
    readSoilMoisture();
  }
}

void loop() {
  // Do nothing - runs only once
}

void readSoilMoisture() {
  // Start measurement
  String response = sendCommand("CC0!");
  Serial.print("Measurement started: ");
  Serial.println(response);
  
  // Wait for measurement to complete
  delay(3000);
  
  // Get the data
  String data = sendCommand("CD0!");
  Serial.print("Soil data: ");
  Serial.println(data);
}

String sendCommand(String command) {
  mySDI12.sendCommand(command);
  delay(1000);
  
  String response = "";
  while (mySDI12.available()) {
    response += (char)mySDI12.read();
  }
  
  return response;
}