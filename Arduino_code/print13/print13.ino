// This sketch tests whether it's possible to write the value 13 from the Teesny to the Arduino
// using the Serial.print() function, in case Serial.write(13) doesn't work.
// It is possible that instead of 13, Pd receives a 10.
// Upload this sketch to the Teensy and open the test13.pd patch.
// If you get a 13 printed into Pd's console, you're good to go.
// Upload the modular_synthesizer_print.ino sketch to the Teensy and open the modular_synthesizer.pd patch
// If instead of 13 you get a 10, upload the modular_synthesizer13.ino sketch
// and in the modular_synthesizer.pd patch, change the [arduino_data] and [matrix_connections] abstractions
// with the [arduino_data13] and the [matrix_connections13] abstractions. The rest should work as expected

void setup() {
  Serial.begin(57600);
}

void loop() {
  Serial.print((char)13);
  delay(500);
}
