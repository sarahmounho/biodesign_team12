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

const int on_led = 23; // A2 for on/off LED (red); new GPIO 23
const int exam_button = 22; // GPIO 2 for exam start/stop button; new 22
const int exam_led_red = 18; // A3 for exam start/in progress/stop LED; new 18
const int exam_led_blue = 21; // A4 for exam start/in progress/stop LED; new 21
const int exam_led_green = 19; // A5 for exam start/in progress/stop LED; new 19
const int potentiometer = 12; // A1 for potentiometer; new 12
const int relay1 = 26; // GPIO 12 for linear actuator; new 26
const int relay2 = 27; // GPIO 13 for linear actuator; new 27
const int enable = 32; // GPIO 11 for linear actuator enable; new 32
const int FSR = 25; // GPIO 0 for FSR (pressure sensor); new 25


#define TFT_CS        15 // new
#define TFT_RST       16 // new 
#define TFT_DC        17 // new
#define TFT_MOSI 13 // Data out; new
#define TFT_SCLK 14 // Clock out; new

int exam_flag = 0; // start/in progress/stop LED

// Plotting set up
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
int adcRange = 4095; // ADC range on board
int xPos = 20; //starting graph position
int prevXPain = 20; 
int prevYPain = 140;
int prevXPressure = 20;
int prevYPressure = 140;

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
    Serial.begin(115200);

    // LCD
    tft.initR(INITR_BLACKTAB); // Init ST7735S chip, black tab
    LCD_reset(); // fill screen with graphics

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
        
        // FSR
        fsr_exam(); // Helper function

        // potentiometer + graph
        pain_smooth(); // Helper function

        // Extend for 2.5 seconds 
        // Pause for 5 seconds
        // Retract for 2.5 seconds
        // Extend/Retract actuator
        actuator_exam(); // Helper function
    }
    delay(10); // .01 second wait to stabalize
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

    if (exam_len < 25){
        extendActuator();
        delay(100); // extend for 0.1 seconds
        exam_len++; // index the exam button 
    }
    else if (exam_len < 75){
        stopActuator();
        delay(100); 
        exam_len++; // index the exam button
    }
    else if (exam_len < 100){
        retractActuator();
        delay(100);
        exam_len++; // index the exam button
    }
    else if (exam_len >= 100){
        exam_flag = 0; // exam over
        exam_len = 0; // reset exam timer
        RGB_color(LOW, HIGH, LOW); // Green exam LED

    }
}
    
void fsr_exam(){
    int fsrReading = analogRead(FSR); // read pressure level
    Serial.println("Analog reading = ");
    // analog voltage reading ranges from about 0 to 1023 which maps to 0V to 3.3V (= 3300 mV)
    int fsrVoltage = map(fsrReading, 0, adcRange, 0, 3300);
    // Graph on LCD
    int graphHeightFSR = map(fsrVoltage, 0, adcRange, 40, tft.height()-20);
    tft.drawPixel(xPos, tft.height() - graphHeightFSR, ST7735_CYAN);
    if (xPos > 20){
        tft.drawLine(prevXPressure, prevYPressure, xPos, tft.height() - graphHeightFSR, ST7735_CYAN);
    }
    prevXPressure = xPos;
    prevYPressure = (tft.height() - graphHeightFSR);

    // Probably don't include in test, but should be included in validation
//    unsigned long fsrResistance = 3300 - fsrVoltage; // fsrVoltage in mV
//    fsrResistance *= 10000; // 10K resistor
//    if (fsrVoltage == 0){
//        fsrVoltage = 0.00001; // avoid divide by zero errors
//    }
//    fsrResistance /= fsrVoltage; 
//    unsigned long fsrConductance = 1000000; // measured in microhms
//    fsrConductance /= fsrResistance; 
//    
//    // Use the two FSR guide graphs to approximate the force
//    if (fsrConductance <= 1000) {
//        long fsrForce = fsrConductance / 80;
//        Serial.print("Force in Newtons: ");
//        Serial.println(fsrForce);     
//    } else {
//        long fsrForce = fsrConductance - 1000;
//        fsrForce /= 30;
//        Serial.print("Force in Newtons: ");
//        Serial.println(fsrForce);
//        tft.fillScreen(ST77XX_BLACK);
//        tft.print(fsrForce, ST77XX_WHITE);
//    }        
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
    int graphHeight = map(avgPain, 0, adcRange, 40, tft.height()-20);
    tft.drawPixel(xPos, tft.height() - graphHeight, ST7735_MAGENTA);
    if (xPos > 20) {
        tft.drawLine(prevXPain, prevYPain, xPos, tft.height() - graphHeight, ST7735_MAGENTA);
    }
    prevXPain = xPos;
    prevYPain = (tft.height() - graphHeight);
    if (xPos >= 120) {
        // Restart, ran out of screen space
        LCD_reset(); 
    } 
    else {
        xPos++; // move on to next position
    }
}
void LCD_reset(){
    xPos = 20; // for analog readings 
    tft.fillScreen(ST77XX_BLACK); // black screen
    testdrawtext("Rebound Tenderness\nExamination", ST77XX_WHITE); // title
    tft.setCursor(80, 15); // move cursor for legend
    tft.setTextColor(ST7735_MAGENTA);
    tft.println("Pain"); 
    tft.setCursor(80, 30); // shift down for legend part 2
    tft.setTextColor(ST7735_CYAN);
    tft.println("Pressure");
    tft.setCursor(20, 140); // move cursor for x-axis
    tft.drawLine(20, 140, 120, 140, ST77XX_WHITE); // x-axis
    tft.setCursor(20, 140); // move cursor for y-axis
    tft.drawLine(20, 140, 20, 40, ST77XX_WHITE); // y-axis
    tft.setCursor(50, 150); // move for x-axis label
    tft.setTextColor(ST77XX_WHITE);
    tft.println("Time"); // x-axis label
}
