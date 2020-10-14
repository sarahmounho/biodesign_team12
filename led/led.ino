/* https://create.arduino.cc/projecthub/muhammad-aqib/arduino-rgb-led-tutorial-fc003e */

int red_light_pin= A3;
int green_light_pin = A4;
int blue_light_pin = A5;
void setup() {
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
}

void loop() {
  //red
  digitalWrite(red_light_pin, HIGH);
  digitalWrite(green_light_pin, LOW);
  digitalWrite(blue_light_pin, LOW);
  delay(1000);
  //green
  digitalWrite(red_light_pin, LOW);
  digitalWrite(green_light_pin, HIGH);
  digitalWrite(blue_light_pin, LOW);
  delay(1000);
  //blue
  digitalWrite(red_light_pin, LOW);
  digitalWrite(green_light_pin, LOW);
  digitalWrite(blue_light_pin, HIGH);
  delay(1000);
}


