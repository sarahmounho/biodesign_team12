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

#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         7

int exam_flag = 0; // start/in progress/stop LED

// Plotting set up
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
int xPos = 0; //starting graph position


void setup(){
    // pinMode definitions
    pinMode(on_led, OUTPUT); // define on/off LED as OUTPUT
    pinMode(exam_button, INPUT); // define exam button as INPUT
    pinMode(exam_led_red, OUTPUT); // define exam LED as OUTPUT
    pinMode(exam_led_blue, OUTPUT); // define exam LED as OUTPUT
    pinMode(exam_led_green, OUTPUT); // define exam LED as OUTPUT
    pinMode(potentiometer, INPUT); // define potentiometer pin as INPUT

    // initial writes
    digitalWrite(on_led, LOW); // turn LED off to start
    digitalWrite(exam_led_red, HIGH); // turn LED off to start
    digitalWrite(exam_led_blue, LOW); // turn LED off to start
    digitalWrite(exam_led_green, LOW); // turn LED off to start

    // potentiometer
    Serial.begin(9600);

    // LCD
    tft.initR(INITR_BLACKTAB); // Init ST7735S chip, black tab
    // large block of text
    tft.fillScreen(ST77XX_BLACK);
    testdrawtext("Pain level during duration of exam", ST77XX_WHITE);
    delay(1000);

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
        int pain = analogRead(potentiometer); // read pain level
        Serial.println(pain); // print pain level on screen

        // Graph on LCD
        int graphHeight = map(pain,0,1023,0,tft.height());
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
        delay(1000); // delay so a reasonable amount of values viewed
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

