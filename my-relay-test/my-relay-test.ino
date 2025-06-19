void setup() {
  // Initialize digital pin 3 as an output
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  Serial.println("\n================== start ================");
  digitalWrite(3, LOW);
  Serial.println("Relay on");
}

void loop() {
  // // Turn pin 3 HIGH for 2 seconds
  // digitalWrite(3, HIGH);
  // Serial.println("Relay off");
  // delay(5000);
  
  // // Turn pin 3 LOW for 2 seconds
  // digitalWrite(3, LOW);
  // Serial.println("Relay on");
  // delay(5000);
}