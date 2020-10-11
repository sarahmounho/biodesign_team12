/* 
https://www.makerguides.com/fsr-arduino-tutorial/
Voltage is inversely proportional to the FSR resistance 
Vout = (Vcc * R) / (R + Rfsr) Voltage drop across the pull-down resistor  
0 V ~ no pressure applied, 5 V maximum pressure applied 

How many bits is the analog to digital converter? 
If 10-bit map 0-5V between 0 - 1023
*/


// Define FSR pin:
#define fsrpin A0
//Define variable to store sensor readings:
int fsrreading; //Variable to store FSR value
void setup() {
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
}
void loop() {
  // Read the FSR pin and store the output as fsrreading:
  fsrreading = analogRead(fsrpin);
  // Print the fsrreading in the serial monitor:
  // Print the string "Analog reading = ".
  Serial.print("Analog reading = ");
  // Print the fsrreading:
  Serial.print(fsrreading);
  // We can set some threshholds to display how much pressure is roughly applied:
  //This is stuff we will want to manipulate (need to set thresholds)
  if (fsrreading < 10) {
    Serial.println(" - No pressure");
  } else if (fsrreading < 200) {
    Serial.println(" - Light touch");
  } else if (fsrreading < 500) {
    Serial.println(" - Light squeeze");
  } else if (fsrreading < 800) {
    Serial.println(" - Medium squeeze");
  } else {
    Serial.println(" - Big squeeze");
  }
  delay(500); //Delay 500 ms.
}