//https://www.arduino.cc/en/Tutorial/LibraryExamples/TFTGraph

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_CS   10
#define TFT_DC   7
#define TFT_RST  9  

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int xPos = 0;

void setup(){
  Serial.begin(9600); 
  pinMode(A1, INPUT); // define potentiometer pin as INPUT
  Serial.print(F("LCD TEST"));

  tft.initR(INITR_BLACKTAB); // Init ST7735S chip, black tab

  Serial.println(F("Initialized"));

  // large block of text
  tft.fillScreen(ST77XX_BLACK);
  testdrawtext("Pain level during duration of exam", ST77XX_WHITE);
  delay(1000);

  // draw a single pixel
 
  
  delay(500);
  //TFTscreen.begin();

  //TFTscreen.background(250,16,200); 
}

void loop(){

  int sensor = analogRead(A1);
  Serial.println(sensor);

  int graphHeight = map(sensor,0,1023,0,tft.height());
  
  tft.drawPixel(xPos, tft.height() - graphHeight, ST77XX_WHITE);
  //tft.drawLine(xPos, tft.height() - graphHeight, xPos, tft.height(), ST77XX_WHITE);


  if (xPos >= 160) {

    xPos = 0;

    tft.fillScreen(ST77XX_BLACK);

  } 

  else {

    xPos++;

  }

  delay(1000);
}


// Helper functions
void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
    delay(0);
  }
}