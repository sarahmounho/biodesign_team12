/* 
https://shiftautomation.com/control-linear-actuator-with-arduino-and-relays

*/


//We need to specify which Arduino pins are going to trigger the relays, and then set them as OUTPUT pins.
//Use constants for the relay pins in case you need to change these later
const int relay1 = 12;   //Arduino pin that triggers relay #1
const int relay2 = 13;   //Arduino pin that triggers relay #2
const int enable = 11; //Enable pin
const int exam_button = 2; // GPIO 2 for exam start/stop button

int exam_flag = 0; // 
void setup() {
  //Set pinMode to OUTPUT for the two relay pins
  pinMode(enable, OUTPUT); 
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(exam_button, INPUT);

  digitalWrite(enable, HIGH); 
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  
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

    if (digitalRead(exam_button)==HIGH){
        if(exam_flag == 0){
            exam_flag = 1; // 1 indicates exam is occuring 
        }
        else{
            exam_flag = 0; // 0 indicates exam is not occuring
        }
        delay(500); // delay to let wait for button press
    }

    if (exam_flag == 1) {
        extendActuator();
        delay(10000);

        // stopActuator();
        // delay(1000);

        // retractActuator();
        // delay(1000);

        // stopActuator();
        // delay(1000);
    }
}

/* Trouble shooting:
If the actuator retracts when you want it to extend, swap pins
//Use constants for the relay pins in case you need to change these later
const int relay1 = 7;   //Arduino pin that triggers relay #1
const int relay2 = 6;   //Arduino pin that triggers relay #2
*/