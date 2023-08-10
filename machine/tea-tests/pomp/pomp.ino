
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(5, HIGH);  
  digitalWrite(6, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(3000);                       // wait for a second
  digitalWrite(5, LOW);  
  digitalWrite(6, LOW);    // turn the LED off by making the voltage LOW
  delay(3000);                       // wait for a second
}
