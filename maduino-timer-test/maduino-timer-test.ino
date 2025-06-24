void setup() {
  SerialUSB.begin(115200);
  while (!SerialUSB) {}  // wat for SerialUSB
  SerialUSB.println(millis());
}

void loop() {
  delay(2000);
  SerialUSB.println(millis());
}
