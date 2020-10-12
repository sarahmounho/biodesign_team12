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

const int on_button = 8; // GPIO 8 for on/off button
const int on_led = 7; // GPIO 7 for on/off LED
const int exam_button = 9; // GPIO 9 for exam start/stop button
const int exam_led_red = 10; // GPIO 10 for exam start/in progress/stop LED
const int exam_led_blue = 11; // GPIO 11 for exam start/in progress/stop LED
const int exam_led_green = 12; // GPIO 12 for exam start/in progress/stop LED
const int potentiometer = 0; // GPIO 0 for potentiometer

int on_ledflag = 0; // on/off LED status flag
int exam_flag = 0; // start/in progress/stop LED

// pain output storage
int pain_array[100] = {}; // array to store 100 pain values
int pain_idx = 0; // idx to keep track of how many pain values stored

void setup(){
    // pinMode definitions
    pinMode(on_button, INPUT); // define on/off button as INPUT
    pinMode(on_led, OUTPUT); // define on/off LED as OUTPUT
    pinMode(exam_button, INPUT); // define exam button as INPUT
    pinMode(exam_led_red, OUTPUT); // define exam LED as OUTPUT
    pinMode(exam_led_blue, OUTPUT); // define exam LED as OUTPUT
    pinMode(exam_led_green, OUTPUT); // define exam LED as OUTPUT

    // initial writes
    digitalWrite(on_led, LOW); // turn LED off to start
    analogWrite(exam_led_red, LOW); // turn LED off to start
    analogWrite(exam_led_blue, LOW); // turn LED off to start
    analogWrite(exam_led_green, LOW); // turn LED off to start

    // potentiometer
    Serial.begin(9600);
}

void loop() {
    // Check for on/off button....change to switch
    if (digitalRead(on_button)==HIGH){
        if (on_ledflag==0) { // if on button pressed
            on_ledflag=1; // make status flag HIGH
            digitalWrite(on_led, HIGH); // turn LED on
        }
        else{
            on_ledflag=0; // make status flag LOW
            digitalWrite(on_led, LOW); // turn off LED
        }
    }
    // Device on, waiting for exam to start
    if (on_ledflag==1){
        if (digitalRead(exam_button)==HIGH){ // exam button pressed
            RGB_color(255, 255, 0); // Yellow exam LED
            exam_flag = 1; // 1 indicates exam is occuring 
        }
        else{
            RGB_color(255, 0, 0); // Red exam LED
            exam_flag = 0; // 0 indicates exam not occuring
        }
    }
    // Device on, exam occuring
    // Perform exam
    // Need to add some sort of 'time element'
    if (on_ledflag==1 && exam_flag==1){
        int pain = analogRead(potentiometer); // read pain level
        Serial.println(pain); // print pain level on screen
        pain_array[pain_idx] = pain; //store pain level in array
        pain_idx ++; // add 1 to index
    // DEVLOPING PRINT ARRAY
    if (pain_idx==99){
        Serial.println(); //blank line
        for (int i = 0; i < 99; i++) {
            Serial.print(pain_array[i]);
        }
    }
    

    }
    delay(1000); // 1 second wait to stabalize
}

//Helper function to write to RGC exam status LED
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(exam_led_red, red_light_value);
  analogWrite(exam_led_blue, green_light_value);
  analogWrite(exam_led_green, blue_light_value);
}