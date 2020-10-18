/* 
https://shiftautomation.com/control-linear-actuator-with-arduino-and-relays

*/


//We need to specify which Arduino pins are going to trigger the relays, and then set them as OUTPUT pins.
//Use constants for the relay pins in case you need to change these later
const int relay1 = 26;   //Arduino pin that triggers relay #1
const int relay2 = 27;   //Arduino pin that triggers relay #2
const int enable = 32; // GPIO 11 for linear actuator enable

void setup() {
  //Set pinMode to OUTPUT for the two relay pins
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(enable, OUTPUT); // define enable pin as OUTPUT
  digitalWrite(enable, HIGH); // turn enable HIGH for linear actuator
  Serial.begin(115200);
}

//Helper functions
void extendActuator() {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, LOW);
}

void retractActuator() {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
}

void stopActuator() {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
}

//Test (does the linear actuator work)
//This will make the actuator extend out for 1 second, then stop for 1 second, then retract for 1 second, and then pause for 5 seconds.
void loop() {
    Serial.println("Extending...");
    extendActuator();
    delay(5000);

    stopActuator();
    delay(250);

    Serial.println("Retracting...");
    retractActuator();
    delay(5000);

    stopActuator();
    delay(250);
}

/* Trouble shooting:
If the actuator retracts when you want it to extend, swap pins
//Use constants for the relay pins in case you need to change these later
const int relay1 = 7;   //Arduino pin that triggers relay #1
const int relay2 = 6;   //Arduino pin that triggers relay #2
*/
