#include <Adafruit_GFX.h>  //Core graphics library
#include <Adafruit_ST7735.h> //Hardware-specific library
#include <SPI.h> 
/* 
LCD Screen: 
https://www.electronics-lab.com/project/using-st7735-1-8-color-tft-display-arduino/

pin connection
LED - 3.3v
SCK - D13
SDA - D11
DC - D9
Reset - D8
CS - D10
GND - GND
VCC - 5v

graphing package: http://www.instesre.org/TFTDataGraphing.pdf
*/

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! ST7735 TFT Test");
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // Use this initializer (uncomment) if you're using a 1.44" TFT
  //tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
  Serial.println("Initialized");
  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);
 // large block of text
  tft.fillScreen(ST7735_BLACK);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST7735_WHITE);
  delay(1000);
  // tft print function!
  tftPrintTest();
  delay(4000);
  // a single pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, ST7735_GREEN);
  delay(500);
  // line draw test
  testlines(ST7735_YELLOW);
  delay(500);
  // optimized lines
  testfastlines(ST7735_RED, ST7735_BLUE);
  delay(500);
  testdrawrects(ST7735_GREEN);
  delay(500);
  testfillrects(ST7735_YELLOW, ST7735_MAGENTA);
  delay(500);
  tft.fillScreen(ST7735_BLACK);
  testfillcircles(10, ST7735_BLUE);
  testdrawcircles(10, ST7735_WHITE);
  delay(500);
  testroundrects();
  delay(500);
  testtriangles();
  delay(500);
  mediabuttons();
  delay(500);
  Serial.println("done");
  delay(1000);
}

void loop() {
tft.invertDisplay(true);
delay(500);
tft.invertDisplay(false);
delay(500);
}