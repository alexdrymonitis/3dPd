// This sketch tests whether it's possible to write the value 13 from the Teesny to the Arduino
// using the Serial.write() function. It is possible that instead of 13, Pd receives a 10.
// Upload this sketch to the Teensy and open the test13.pd patch.
// If you get a 13 printed into Pd's console, you're good to go.
// Upload the modular_synthesizer.ino sketch to the Teensy and open the modular_synthesizer.pd patch
// If instead of 13 you get a 10, try the print13.ino sketch with the same patch.

void setup() {
  Serial.begin(57600);
}

void loop() {
  while(true) {
    Serial.write(13);
    delay(500);
  }
}
