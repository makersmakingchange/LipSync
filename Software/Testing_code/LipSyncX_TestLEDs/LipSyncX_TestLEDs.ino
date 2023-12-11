/*******************
  This code is used to test the LEDs on the LipSyncX Hub.
  Last edited: November 16, 2023
 *******************/

#include "Adafruit_TinyUSB.h"

#define PIN_LED_MOUSE   8
#define PIN_LED_GAMEPAD 7
#define PIN_LED_MOUSE_WIRELESS  6

#define PIN_BUTTON_SEL    3
#define PIN_BUTTON_NEXT   9

#define MODE_GAMEPAD    0
#define MODE_MOUSE      1
#define MODE_MOUSE_WIRELESS   2

int mode = MODE_GAMEPAD;

bool buttonSelPressed = false;
bool buttonNextPressed = false;
bool buttonSelPrevPressed = false;
bool buttonNextPrevPressed = false;


void setup() {
  pinMode(PIN_BUTTON_SEL, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NEXT, INPUT_PULLUP);

  pinMode(PIN_LED_MOUSE, OUTPUT);
  pinMode(PIN_LED_MOUSE_WIRELESS, OUTPUT);
  pinMode(PIN_LED_GAMEPAD, OUTPUT);
  
  digitalWrite(PIN_LED_MOUSE, LOW);
  digitalWrite(PIN_LED_MOUSE_WIRELESS, LOW);
  digitalWrite(PIN_LED_GAMEPAD, LOW);

  delay(500);


  digitalWrite(PIN_LED_GAMEPAD, HIGH);

  delay(1000);

  digitalWrite(PIN_LED_GAMEPAD, LOW);

  delay(100);

  digitalWrite(PIN_LED_MOUSE, HIGH);

  delay(1000);

  digitalWrite(PIN_LED_MOUSE, LOW);

  delay(100);

  digitalWrite(PIN_LED_MOUSE_WIRELESS, HIGH);

  delay(1000);

  digitalWrite(PIN_LED_MOUSE_WIRELESS, LOW);

  delay(100);

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
    if (mode>2){
      mode=0;
    }
    Serial.println("Next pressed");
  }
}

void ledActions(void){

  switch (mode){
    case MODE_GAMEPAD:
      digitalWrite(PIN_LED_MOUSE, LOW);
      digitalWrite(PIN_LED_GAMEPAD, HIGH);
      digitalWrite(PIN_LED_MOUSE_WIRELESS, LOW);
      Serial.println("Gamepad LED on");
      break;
    case MODE_MOUSE:
      digitalWrite(PIN_LED_GAMEPAD, LOW);
      digitalWrite(PIN_LED_MOUSE, HIGH);
      digitalWrite(PIN_LED_MOUSE_WIRELESS, LOW);
      Serial.println("Mouse LED on");
      break;
    case MODE_MOUSE_WIRELESS:
      digitalWrite(PIN_LED_MOUSE, LOW);
      digitalWrite(PIN_LED_GAMEPAD, LOW);
      digitalWrite(PIN_LED_MOUSE_WIRELESS, HIGH);
      Serial.println("Mouse Wireless LED on");
      break;
  }
}
