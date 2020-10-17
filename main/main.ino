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
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

// I/O definitions
// 14 pins available

const int on_led = A2; // A2 for on/off LED (red)
const int exam_button = 2; // GPIO 2 for exam start/stop button
const int exam_led_red = A3; // A3 for exam start/in progress/stop LED
const int exam_led_blue = A4; // A4 for exam start/in progress/stop LED
const int exam_led_green = A5; // A5 for exam start/in progress/stop LED
const int potentiometer = A1; // A1 for potentiometer
const int relay1 = 12; // GPIO 12 for linear actuator
const int relay2 = 13; // GPIO 13 for linear actuator
const int enable = 11; // GPIO 11 for linear actuator enable
const int FSR = 0; // GPIO 0 for FSR (pressure sensor)


#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         7

int exam_flag = 0; // start/in progress/stop LED

// Plotting set up
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
int xPos = 0; //starting graph position

// Linear actuator set up
int exam_len = 0; 

// Smoothing for slide potentiometer
const int numReadingsPain = 1000;
int readingsPain[numReadingsPain]; // array to store readings from the analog INPUT
int readIdxPain = 0; // idx of current reading
int totalPain = 0; // running total
int avgPain = 0; // the average

void setup(){
    // pinMode definitions
    pinMode(on_led, OUTPUT); // define on/off LED as OUTPUT
    pinMode(exam_button, INPUT); // define exam button as INPUT
    pinMode(exam_led_red, OUTPUT); // define exam LED as OUTPUT
    pinMode(exam_led_blue, OUTPUT); // define exam LED as OUTPUT
    pinMode(exam_led_green, OUTPUT); // define exam LED as OUTPUT
    pinMode(potentiometer, INPUT); // define potentiometer pin as INPUT
    pinMode(relay1, OUTPUT); // define relay1 as OUTPUT
    pinMode(relay2, OUTPUT); // define relay2 as OUTPUT
    pinMode(enable, OUTPUT); // define enable pin as OUTPUT

    // initial writes
    digitalWrite(on_led, LOW); // turn LED off to start
    digitalWrite(exam_led_red, HIGH); // turn LED off to start
    digitalWrite(exam_led_blue, LOW); // turn LED off to start
    digitalWrite(exam_led_green, LOW); // turn LED off to start
    digitalWrite(enable, HIGH); // turn enable HIGH for linear actuator
    digitalWrite(relay1, LOW); // turn off relay for linear actuator
    digitalWrite(relay2, LOW); // turn off relay for linear actuator

    // potentiometer
    Serial.begin(9600);

    // LCD
    tft.initR(INITR_BLACKTAB); // Init ST7735S chip, black tab
    // large block of text
    tft.fillScreen(ST77XX_BLACK);
    //testdrawtext("Pain level during duration of exam", ST77XX_WHITE);
    delay(1000);

    // Retract linear actuator to start
    retractActuator();
    delay(2500);

    // initialize all the readings to 0 for pain:
    for (int thisReadingPain = 0; thisReadingPain < numReadingsPain; thisReadingPain++) {
        readingsPain[thisReadingPain] = 0;
  }

}

void loop() {

    // Device on, waiting for exam to start
    digitalWrite(on_led, HIGH);

    if (digitalRead(exam_button)==HIGH){ // exam button pressed
        if(exam_flag == 0){
            RGB_color(LOW, LOW, HIGH); // Blue exam LED
            exam_flag = 1; // 1 indicates exam is occuring 
        }
        else{
            RGB_color(HIGH, LOW, LOW); //Red exam LED
            exam_flag = 0; // 0 indicates exam is not occuring
        }
        delay(500); // delay to let wait for button press
    }

    // Device on, exam occuring
    // Perform exam
    // Need to add some sort of 'time element'
    
    if (exam_flag==1){
        
        // potentiometer + graph
        pain_smooth(); // Helper function

        // FSR
        fsr_exam(); // Helper function

        // Extend for 5 seconds 
        // Pause for 10 seconds
        // Retract for 5 seconds
        // Extend/Retract actuator
        actuator_exam(); // Helper function
    }
    delay(10); // .1 second wait to stabalize
}

// Helper function to write to RGC exam status LED
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  digitalWrite(exam_led_red, red_light_value);
  digitalWrite(exam_led_blue, green_light_value);
  digitalWrite(exam_led_green, blue_light_value);
}


void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
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

void actuator_exam(){
    // print exam_len for dev
    Serial.print("exam_len = ");
    Serial.println(exam_len);

    if (exam_len <= 5){
        extendActuator();
        delay(500); // extend for 0.5 seconds
        exam_len++; // index the exam button 
    }
    else if (exam_len <= 15){
        stopActuator();
        delay(500); 
        exam_len++; // index the exam button
    }
    else if (exam_len <= 20){
        retractActuator();
        delay(500);
        exam_len++; // index the exam button
    }
    else if (exam_len > 20){
        exam_flag = 0; // exam over
        exam_len = 0; // reset exam timer
        RGB_color(LOW, HIGH, LOW); // Green exam LED

    }
}
    
void fsr_exam(){
    int fsrReading = analogRead(FSR); // read pressure level
    Serial.println("Analog reading = ");
    // analog voltage reading ranges from about 0 to 1023 which maps to 0V to 3.3V (= 3300 mV)
    int fsrVoltage = map(fsrReading, 0, 1023, 0, 3300); 
    unsigned long fsrResistance = 3300 - fsrVoltage; // fsrVoltage in mV
    fsrResistance *= 10000; // 10K resistor
    fsrResistance /= fsrVoltage; 
    unsigned long fsrConductance = 1000000; // measured in microhms
    fsrConductance /= fsrResistance; 

    // Use the two FSR guide graphs to approximate the force
    if (fsrConductance <= 1000) {
        int fsrForce = fsrConductance / 80;
        Serial.print("Force in Newtons: ");
        Serial.println(fsrForce);     
        tft.fillScreen(ST77XX_BLACK);
        tft.print(fsrForce, ST77XX_WHITE); 
    } else {
        int fsrForce = fsrConductance - 1000;
        fsrForce /= 30;
        Serial.print("Force in Newtons: ");
        Serial.println(fsrForce);
        tft.fillScreen(ST77XX_BLACK);
        tft.print(fsrForce, ST77XX_WHITE);
    }        
}

void pain_smooth(){
    // rolling average (currently)...may just change to pure average

    // subtract the last reading:
    totalPain = totalPain - readingsPain[readIdxPain];
    // read from the slide pot:
    readingsPain[readIdxPain] = analogRead(potentiometer);
    // add the reading to the total:
    totalPain = totalPain + readingsPain[readIdxPain]; 
    // add to idx 
    readIdxPain ++;

    // at end of array
    if (readIdxPain >= numReadingsPain) {
        // Restart
        readIdxPain = 0; 
    }

    // calc avgPain:
    avgPain = totalPain / numReadingsPain; 

    Serial.print("Pain:"); 
    Serial.println(avgPain); // print pain level on screen

    // Graph on LCD
    int graphHeight = map(avgPain,0,1023,0,tft.height());
    tft.drawPixel(xPos, tft.height() - graphHeight, ST77XX_WHITE);
    if (xPos >= 160) {
        // Restart, ran out of screen space
        xPos = 0;
        tft.fillScreen(ST77XX_BLACK);
        testdrawtext("Pain level during duration of exam", ST77XX_WHITE);
    } 
    else {
        xPos++; // move on to next position
    }
}