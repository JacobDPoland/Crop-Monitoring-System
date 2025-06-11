#include <Arduino.h>
#include <SDI12.h>
#include <avr/sleep.h>

#define PROBE_DATA_PIN 2        // Blue wire connected to digital pin 2
#define PROBE_POWER_PIN 3       // Control power to the probe for energy saving

SDI12 probeSDI12(PROBE_DATA_PIN);
String probeResponse = "";
String probeCommand = "";

// Sleep configuration - easily adjustable
const int READING_INTERVAL_MINUTES = 1;  // Change this value to adjust reading interval
const unsigned long READING_INTERVAL_MS = READING_INTERVAL_MINUTES * 60UL * 1000UL; // Convert to milliseconds

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
  Serial.begin(9600);
  
  // Configure probe power pin
  pinMode(PROBE_POWER_PIN, OUTPUT);
  digitalWrite(PROBE_POWER_PIN, LOW); // Start with probe powered off
  
  Serial.println("========================================");
  Serial.println("Efficient Soil Logging System");
  Serial.print("Reading every ");
  Serial.print(READING_INTERVAL_MINUTES);
  Serial.println(" minutes");
  Serial.println("Format: timestamp,M,moisture_values or timestamp,T,temperature_values");
  Serial.println("========================================");
  Serial.println();
  
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
  // Power up the probe
  digitalWrite(PROBE_POWER_PIN, HIGH);
  delay(2000); // Give probe time to stabilize
  
  probeSDI12.begin();
  delay(500);
  
  // Take moisture reading
  getMoistureReading();
  delay(1000);
  
  // Take temperature reading
  getTemperatureReading();
  delay(1000);
  
  // Power down the probe to save energy
  digitalWrite(PROBE_POWER_PIN, LOW);
  probeSDI12.end();
}

void getMoistureReading() {
  // Start moisture measurement (address C, command 0 for moisture with salinity compensation)
  sendProbeCommand("CC0!");
  delay(3000); // Wait for measurement to complete
  
  // Request the data
  sendProbeCommand("CD0!");
  delay(1000);
  
  // Process the response
  if (probeResponse.length() > 0) {
    processMoistureData(probeResponse);
    probeResponse = "";
  }
}

void getTemperatureReading() {
  // Start temperature measurement (address C, command 2 for temperature)
  sendProbeCommand("CC2!");
  delay(3000); // Wait for measurement to complete
  
  // Request the data
  sendProbeCommand("CD0!");
  delay(1000);
  
  // Process the response
  if (probeResponse.length() > 0) {
    processTemperatureData(probeResponse);
    probeResponse = "";
  }
}

void sendProbeCommand(String command) {
  probeSDI12.sendCommand(command);
  
  // Read response with timeout
  probeResponse = "";
  unsigned long startTime = millis();
  while (millis() - startTime < 2000) { // 2 second timeout
    if (probeSDI12.available()) {
      char c = probeSDI12.read();
      if (c == '\n' || c == '\r') {
        if (probeResponse.length() > 0) {
          break;
        }
      } else {
        probeResponse += c;
      }
    }
    delay(10);
  }
}

void processMoistureData(String data) {
  // Remove address character (usually 'C') from beginning if present
  if (data.length() > 0 && (data.charAt(0) == 'C' || data.charAt(0) == 'c')) {
    data = data.substring(1);
  }
  
  // Start the moisture line with timestamp
  Serial.print(millis());
  Serial.print(",M,");
  
  // Split by '+' signs and collect all values
  int startIndex = 0;
  int plusIndex = data.indexOf('+', startIndex);
  bool firstValue = true;
  
  while (plusIndex != -1) {
    String value = data.substring(startIndex, plusIndex);
    value.trim();
    if (value.length() > 0) {
      if (!firstValue) {
        Serial.print(",");
      }
      Serial.print(value);
      firstValue = false;
    }
    startIndex = plusIndex + 1;
    plusIndex = data.indexOf('+', startIndex);
  }
  
  // Handle the last value (after the last '+' or if no '+' found)
  if (startIndex < data.length()) {
    String value = data.substring(startIndex);
    value.trim();
    if (value.length() > 0) {
      if (!firstValue) {
        Serial.print(",");
      }
      Serial.print(value);
    }
  }
  
  Serial.println(); // End the line
}

void processTemperatureData(String data) {
  // Remove address character (usually 'C') from beginning if present
  if (data.length() > 0 && (data.charAt(0) == 'C' || data.charAt(0) == 'c')) {
    data = data.substring(1);
  }
  
  // Start the temperature line with timestamp
  Serial.print(millis());
  Serial.print(",T,");
  
  // Split by '+' signs and collect all values
  int startIndex = 0;
  int plusIndex = data.indexOf('+', startIndex);
  bool firstValue = true;
  
  while (plusIndex != -1) {
    String value = data.substring(startIndex, plusIndex);
    value.trim();
    if (value.length() > 0) {
      if (!firstValue) {
        Serial.print(",");
      }
      Serial.print(value);
      firstValue = false;
    }
    startIndex = plusIndex + 1;
    plusIndex = data.indexOf('+', startIndex);
  }
  
  // Handle the last value (after the last '+' or if no '+' found)
  if (startIndex < data.length()) {
    String value = data.substring(startIndex);
    value.trim();
    if (value.length() > 0) {
      if (!firstValue) {
        Serial.print(",");
      }
      Serial.print(value);
    }
  }
  
  Serial.println(); // End the line
}

void enterLightSleep() {
  // Disable unnecessary peripherals to save power
  disablePeripherals();
  
  // Use IDLE sleep mode - maintains timers and millis()
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  
  // Brief sleep period
  delay(100); // Sleep for 100ms at a time
  
  sleep_disable();
  
  // Re-enable peripherals
  enablePeripherals();
}

void disablePeripherals() {
  // Disable ADC to save power
  ADCSRA &= ~(1 << ADEN);
  
  // Disable SPI
  SPCR &= ~(1 << SPE);
  
  // Disable TWI (I2C)
  TWCR &= ~(1 << TWEN);
  
  // Note: We keep UART enabled for Serial communication
  // and Timer0 for millis() functionality
}

void enablePeripherals() {
  // Re-enable ADC
  ADCSRA |= (1 << ADEN);
  
  // Re-enable SPI if needed
  // SPCR |= (1 << SPE);
  
  // Re-enable TWI if needed
  // TWCR |= (1 << TWEN);
}