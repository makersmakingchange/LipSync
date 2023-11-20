/*******************
  This code is used to test the LEDs on the LipSyncX Hub.
  Last edited: November 16, 2023
 *******************/

#include "Adafruit_TinyUSB.h"

#define PIN_LED_MOUSE   7
#define PIN_LED_GAMEPAD 9

#define PIN_BUTTON_SEL    3
#define PIN_BUTTON_NEXT   10

#define MODE_MOUSE    0
#define MODE_GAMEPAD  1

int mode = MODE_MOUSE;

bool buttonSelPressed = false;
bool buttonNextPressed = false;
bool buttonSelPrevPressed = false;
bool buttonNextPrevPressed = false;


void setup() {
  pinMode(PIN_BUTTON_SEL, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NEXT, INPUT_PULLUP);

  pinMode(PIN_LED_MOUSE, OUTPUT);
  pinMode(PIN_LED_GAMEPAD, OUTPUT);
  digitalWrite(PIN_LED_MOUSE, LOW);
  digitalWrite(PIN_LED_GAMEPAD, LOW);

  delay(500);

  digitalWrite(PIN_LED_MOUSE, HIGH);

  delay(1000);

  digitalWrite(PIN_LED_MOUSE, LOW);

  delay(100);

  digitalWrite(PIN_LED_GAMEPAD, HIGH);

  delay(1000);

  digitalWrite(PIN_LED_GAMEPAD, LOW);

  delay(10);

  Serial.begin(115200);

  delay(100);

  Serial.println("Setup");
}

void loop() {
  readButtons();
  buttonActions();
  ledActions();
  
}

void readButtons(void){
  buttonSelPrevPressed = buttonSelPressed;
  buttonNextPrevPressed = buttonNextPressed;

  buttonSelPressed = !digitalRead(PIN_BUTTON_SEL);
  buttonNextPressed = !digitalRead(PIN_BUTTON_NEXT);
}

void buttonActions(void){
  if (!buttonNextPressed && buttonNextPrevPressed){
    mode++;
    if (mode>1){
      mode=0;
    }
    Serial.println("Next pressed");
  }
}

void ledActions(void){

  switch (mode){
    case MODE_MOUSE:
      digitalWrite(PIN_LED_GAMEPAD, LOW);
      digitalWrite(PIN_LED_MOUSE, HIGH);
      Serial.println("Mouse LED on");
      break;
    case MODE_GAMEPAD:
      digitalWrite(PIN_LED_MOUSE, LOW);
      digitalWrite(PIN_LED_GAMEPAD, HIGH);
      Serial.println("Gamepad LED on");
      break;
  }
}
