/*
  FGFS Slide Potentiometer Input
  Reads an analog input on pin A0, prints the result
  to the serial port
 
  This example code is in the public domain.

  https://playground.arduino.cc/Main/FlightGearInputPot/
 */
 
void setup() {             
  Serial.begin(115200);
}
 
void loop() {
  int buttonValue = digitalRead(22);
  Serial.println(buttonValue);
}
