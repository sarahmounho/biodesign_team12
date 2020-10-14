/*
Main program for BME370 - Group 12

Author: Sarah Mounho
Date Created: 10/11/2020
Last Modified: 10/11/2020

Description:

*/

// Library uploads
#include <stdio.h>
#include <stdlib.h>

// I/O definitions
// 14 pins available

const int on_led = A2; // A2 for on/off LED (red)
const int exam_button = 2; // GPIO 2 for exam start/stop button
const int exam_led_red = A3; // A3 for exam start/in progress/stop LED
const int exam_led_blue = A4; // A4 for exam start/in progress/stop LED
const int exam_led_green = A5; // A5 for exam start/in progress/stop LED
const int potentiometer = A1; // A1 for potentiometer


int exam_flag = 0; // start/in progress/stop LED

void setup(){
    // pinMode definitions
    pinMode(on_led, OUTPUT); // define on/off LED as OUTPUT
    pinMode(exam_button, INPUT); // define exam button as INPUT
    pinMode(exam_led_red, OUTPUT); // define exam LED as OUTPUT
    pinMode(exam_led_blue, OUTPUT); // define exam LED as OUTPUT
    pinMode(exam_led_green, OUTPUT); // define exam LED as OUTPUT

    // initial writes
    digitalWrite(on_led, LOW); // turn LED off to start
    digitalWrite(exam_led_red, HIGH); // turn LED off to start
    digitalWrite(exam_led_blue, LOW); // turn LED off to start
    digitalWrite(exam_led_green, LOW); // turn LED off to start

    // potentiometer
    Serial.begin(9600);

}

void loop() {

    // Device on, waiting for exam to start
    digitalWrite(on_led, HIGH);

    if (digitalRead(exam_button)==HIGH){ // exam button pressed
        if(exam_flag == 0){
            RGB_color(LOW, LOW, HIGH); // Blue exam LED
            exam_flag = 1; // 1 indicates exam is occuring 
            int pain_idx = 0; //start pain array
        }
        else{
            RGB_color(HIGH, LOW, LOW); //Red exam LED
            exam_flag = 0; // 0 indicates exam is not occuring
        }
    }
    
    // Device on, exam occuring
    //Perform exam
    //Need to add some sort of 'time element'
    
    if (exam_flag==1){
        int pain = analogRead(potentiometer); // read pain level
        Serial.println(pain); // print pain level on screen
        delay(30000);
    }
    delay(10); // .1 second wait to stabalize
}

//Helper function to write to RGC exam status LED
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  digitalWrite(exam_led_red, red_light_value);
  digitalWrite(exam_led_blue, green_light_value);
  digitalWrite(exam_led_green, blue_light_value);
}
