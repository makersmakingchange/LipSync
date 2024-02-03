/*******************
  This code is used to test the LEDs on the LipSyncX Hub.
  Last edited: 2024-January-19
 *******************/

#define PIN_LED_LEFT    7
#define PIN_LED_MIDDLE  8
#define PIN_LED_RIGHT   6

#define CONF_LED_MICRO_RED LED_RED          // Xiao NRF52840 User LED Red
#define CONF_LED_MICRO_GREEN LED_GREEN      // Xiao NRF52840 User LED Green
#define CONF_LED_MICRO_BLUE LED_BLUE        // Xiao NRF52840 User LED Blue


#include "Adafruit_TinyUSB.h"

int r = 0;
int g = 0;
int b = 0;

void setup() {
  
  Serial.begin(115200);
  while (!Serial) { delay(1); }  // Wait until serial port is opened
  
  pinMode(PIN_LED_LEFT, OUTPUT);
  pinMode(PIN_LED_MIDDLE, OUTPUT);
  pinMode(PIN_LED_RIGHT, OUTPUT);

  digitalWrite(PIN_LED_LEFT, LOW);
  digitalWrite(PIN_LED_MIDDLE, LOW);
  digitalWrite(PIN_LED_RIGHT, LOW);


  // Microcontroller LED
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);


}

void loop() {

/*

  Serial.println("All on");

  digitalWrite(PIN_LED_LEFT, HIGH);
  digitalWrite(PIN_LED_MIDDLE, HIGH);
  digitalWrite(PIN_LED_RIGHT, HIGH);
  delay(3000);

  Serial.println("All off");
  digitalWrite(PIN_LED_LEFT, LOW);
  digitalWrite(PIN_LED_MIDDLE, LOW);
  digitalWrite(PIN_LED_RIGHT, LOW);
  delay(1000);

  Serial.println("Left on");

  digitalWrite(PIN_LED_LEFT, HIGH);
  digitalWrite(PIN_LED_MIDDLE, LOW);
  digitalWrite(PIN_LED_RIGHT, LOW);
  delay(1000);

  Serial.println("Middle on");
  digitalWrite(PIN_LED_LEFT, LOW);
  digitalWrite(PIN_LED_MIDDLE, HIGH);
  digitalWrite(PIN_LED_RIGHT, LOW);
  delay(1000);

  Serial.println("Right on");
  digitalWrite(PIN_LED_LEFT, LOW);
  digitalWrite(PIN_LED_MIDDLE, LOW);
  digitalWrite(PIN_LED_RIGHT, HIGH);
  delay(1000);
  
  Serial.println("All off");

  digitalWrite(PIN_LED_LEFT, LOW);
  digitalWrite(PIN_LED_MIDDLE, LOW);
  digitalWrite(PIN_LED_RIGHT, LOW);
  delay(2000);

  */

  Serial.println("Red");

  // int r = colorProperty[ledColorNumber].colorCode.r;
  // int g = colorProperty[ledColorNumber].colorCode.g;
  // int b = colorProperty[ledColorNumber].colorCode.b;


r = 220;
g = 38;
b = 127;

  analogWrite(LED_RED, 255 - r);
  analogWrite(LED_GREEN, 255 - g);
  analogWrite(LED_BLUE, 255 - b);

  delay(1000);

  
Serial.println("Blue");
 r = 100;
g = 143;
b = 255;

  analogWrite(LED_RED, 255 - r);
  analogWrite(LED_GREEN, 255 - g);
  analogWrite(LED_BLUE, 255 - b); 
 
 delay(1000);

 Serial.println("White");

 r = 255;
g = 255;
b = 255;

  analogWrite(LED_RED, 255 - r);
  analogWrite(LED_GREEN, 255 - g);
  analogWrite(LED_BLUE, 255 - b); 
  
  delay(1000);
}

