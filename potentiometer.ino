/*
  FGFS Slide Potentiometer Input
  Reads an analog input on pin A0, prints the result
  to the serial port
 
  This example code is in the public domain.

  https://playground.arduino.cc/Main/FlightGearInputPot/
 */
 
void setup_p() {
  Serial.begin(9600);
}
 
void loop_p() {
  int sensorValue = analogRead(A0);
  Serial.println(sensorValue);
}