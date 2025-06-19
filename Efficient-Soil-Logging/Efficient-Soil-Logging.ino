#include <SDI12.h>
#include <ArduinoLowPower.h>

#define PROBE_DATA_PIN 2        // Blue wire connected to digital pin 2

SDI12 probeSDI12(PROBE_DATA_PIN);
String probeResponse = "";
String probeCommand = "";

// Sleep configuration - easily adjustable
const int READING_INTERVAL_MINUTES = 1;  // Change this value to adjust reading interval
const unsigned long READING_INTERVAL_MS = READING_INTERVAL_MINUTES * 10UL * 1000UL; // Convert to milliseconds

// Timing variables 
unsigned long lastReadingTime = 0;

// Function declarations
void takeReading();
void getMoistureReading();
void getTemperatureReading();
void sendProbeCommand(String command);
void processMoistureData(String data);
void processTemperatureData(String data);
void enterLightSleep();
void disablePeripherals();
void enablePeripherals();

void setup() {
  SerialUSB.begin(115200);
  while (!SerialUSB) {
    ; // wait for SerialUSB port to connect. Needed for native USB
  }
  
  delay(100); // Small delay to ensure connection is stable

  SerialUSB.println("========================================");
  SerialUSB.println("Efficient Soil Logging System - Maduino Zero");
  SerialUSB.print("Reading every ");
  SerialUSB.print(READING_INTERVAL_MINUTES);
  SerialUSB.println(" minutes");
  SerialUSB.println("Format: timestamp_seconds,M,moisture_values or timestamp_seconds,T,temperature_values");
  SerialUSB.println("========================================");
  SerialUSB.println();
  
  // Take initial reading and record the time
  lastReadingTime = millis();
  takeReading();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Check if it's time for the next reading
  if (currentTime - lastReadingTime >= READING_INTERVAL_MS) {
    lastReadingTime = currentTime;
    takeReading();
  } else {
    // Enter light sleep to save power while maintaining timers
    enterLightSleep();
  }
}

void takeReading() {
  probeSDI12.begin();
  delay(500);
  
  // Take moisture reading
  getMoistureReading();
  delay(1000);
  
  // Take temperature reading
  getTemperatureReading();
  delay(1000);
  
  probeSDI12.end();
}

void getMoistureReading() {
  sendProbeCommand("CC0!");
  delay(3000); // Wait for measurement to complete
  sendProbeCommand("CD0!");
  delay(1000);
  
  if (probeResponse.length() > 0) {
    processMoistureData(probeResponse);
    probeResponse = "";
  }
}

void getTemperatureReading() {
  sendProbeCommand("CC2!");
  delay(3000);
  sendProbeCommand("CD0!");
  delay(1000);
  
  if (probeResponse.length() > 0) {
    processTemperatureData(probeResponse);
    probeResponse = "";
  }
}

void sendProbeCommand(String command) {
  probeSDI12.sendCommand(command);
  probeResponse = "";
  unsigned long startTime = millis();
  
  while (millis() - startTime < 2000) {
    if (probeSDI12.available()) {
      char c = probeSDI12.read();
      if (c == '\n' || c == '\r') {
        if (probeResponse.length() > 0) break;
      } else {
        probeResponse += c;
      }
    }
    delay(10);
  }
}

void processMoistureData(String data) {
  if (data.length() > 0 && (data.charAt(0) == 'C' || data.charAt(0) == 'c')) {
    data = data.substring(1);
  }

  SerialUSB.print(millis() / 1000);
  SerialUSB.print(",M,");

  int startIndex = 0;
  int plusIndex = data.indexOf('+', startIndex);
  bool firstValue = true;

  while (plusIndex != -1) {
    String value = data.substring(startIndex, plusIndex);
    value.trim();
    if (value.length() > 0) {
      if (!firstValue) SerialUSB.print(",");
      SerialUSB.print(value);
      firstValue = false;
    }
    startIndex = plusIndex + 1;
    plusIndex = data.indexOf('+', startIndex);
  }

  if (startIndex < data.length()) {
    String value = data.substring(startIndex);
    value.trim();
    if (value.length() > 0) {
      if (!firstValue) SerialUSB.print(",");
      SerialUSB.print(value);
    }
  }

  SerialUSB.println();
}

void processTemperatureData(String data) {
  if (data.length() > 0 && (data.charAt(0) == 'C' || data.charAt(0) == 'c')) {
    data = data.substring(1);
  }

  SerialUSB.print(millis() / 1000);
  SerialUSB.print(",T,");

  int startIndex = 0;
  int plusIndex = data.indexOf('+', startIndex);
  bool firstValue = true;

  while (plusIndex != -1) {
    String value = data.substring(startIndex, plusIndex);
    value.trim();
    if (value.length() > 0) {
      if (!firstValue) SerialUSB.print(",");
      SerialUSB.print(value);
      firstValue = false;
    }
    startIndex = plusIndex + 1;
    plusIndex = data.indexOf('+', startIndex);
  }

  if (startIndex < data.length()) {
    String value = data.substring(startIndex);
    value.trim();
    if (value.length() > 0) {
      if (!firstValue) SerialUSB.print(",");
      SerialUSB.print(value);
    }
  }

  SerialUSB.println();
}

void enterLightSleep() {
  LowPower.idle(100); // Sleep for 100ms at a time
}

void disablePeripherals() {
  ADC->CTRLA.bit.ENABLE = 0;
  while (ADC->STATUS.bit.SYNCBUSY);
}

void enablePeripherals() {
  ADC->CTRLA.bit.ENABLE = 1;
  while (ADC->STATUS.bit.SYNCBUSY);
}
