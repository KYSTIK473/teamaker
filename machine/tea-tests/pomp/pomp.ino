
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(A4, HIGH);  
  digitalWrite(A5, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(3000);                       // wait for a second
  digitalWrite(A4, LOW);  
  digitalWrite(A5, LOW);    // turn the LED off by making the voltage LOW
  delay(3000);                       // wait for a second
}
