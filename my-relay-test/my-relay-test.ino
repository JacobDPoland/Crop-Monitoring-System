void setup() {
  // Initialize digital pin 3 as an output
  pinMode(3, OUTPUT);
}

void loop() {
  // Turn pin 3 HIGH for 2 seconds
  digitalWrite(3, HIGH);
  delay(5000);
  
  // Turn pin 3 LOW for 2 seconds
  digitalWrite(3, LOW);
  delay(5000);
}