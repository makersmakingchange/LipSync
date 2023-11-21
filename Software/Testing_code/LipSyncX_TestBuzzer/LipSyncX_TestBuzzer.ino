/*******************
  This code is used to test the LEDs on the LipSyncX Hub.
  Last edited: November 16, 2023
 *******************/

#include "Adafruit_TinyUSB.h"

#define PIN_BUZZER    6

#define PIN_BUTTON_SEL    3
#define PIN_BUTTON_NEXT   10

bool buttonSelPressed = false;
bool buttonNextPressed = false;
bool buttonSelPrevPressed = false;
bool buttonNextPrevPressed = false;


void setup() {
  pinMode(PIN_BUTTON_SEL, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NEXT, INPUT_PULLUP);

  pinMode(PIN_BUZZER, OUTPUT);

  delay(500);

  tone(PIN_BUZZER, 1000);

  delay(1000);

  noTone(PIN_BUZZER);

  delay(10);

  Serial.begin(9600);

  Serial.println("Setup");
}

void loop() {
  readButtons();
  buttonActions();
  
}

void readButtons(void){
  buttonSelPrevPressed = buttonSelPressed;
  buttonNextPrevPressed = buttonNextPressed;

  buttonSelPressed = !digitalRead(PIN_BUTTON_SEL);
  buttonNextPressed = !digitalRead(PIN_BUTTON_NEXT);
}

void buttonActions(void){
  if (buttonNextPressed){
    tone(PIN_BUZZER, 1000);
    Serial.println("Next pressed");
    delay(1000);
  } else{
    noTone(PIN_BUZZER);
  }
}