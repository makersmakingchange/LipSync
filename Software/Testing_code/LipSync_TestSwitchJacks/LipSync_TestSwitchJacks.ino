/*******************
  This code is used to test the LEDs on the LipSync Hub.
  Last edited: November 16, 2023
 *******************/

#include "Adafruit_TinyUSB.h"

#define PIN_BUZZER    6

#define PIN_LED_MOUSE   7
#define PIN_LED_GAMEPAD 9

#define PIN_SWITCH_S1   0
#define PIN_SWITCH_S2   1
#define PIN_SWITCH_S3   2


bool switchS1Pressed = false;
bool switchS2Pressed = false;
bool switchS3Pressed = false;

bool switchS1PrevPressed = false;
bool switchS2PrevPressed = false;
bool switchS3PrevPressed = false;


void setup() {
  pinMode(PIN_SWITCH_S1, INPUT_PULLUP);
  pinMode(PIN_SWITCH_S2, INPUT_PULLUP);
  pinMode(PIN_SWITCH_S3, INPUT_PULLUP);

  pinMode(PIN_LED_MOUSE, OUTPUT);
  pinMode(PIN_LED_GAMEPAD, OUTPUT);
  digitalWrite(PIN_LED_MOUSE, LOW);
  digitalWrite(PIN_LED_GAMEPAD, LOW);


  Serial.begin(115200);

  delay(100);

  Serial.println("Setup");
}

void loop() {
  readButtons();
  switchActions();
  
}

void readButtons(void){
  switchS1PrevPressed = switchS1Pressed;
  switchS2PrevPressed = switchS2Pressed;
  switchS3PrevPressed = switchS3Pressed;

  switchS1Pressed = !digitalRead(PIN_SWITCH_S1);
  switchS2Pressed = !digitalRead(PIN_SWITCH_S2);
  switchS3Pressed = !digitalRead(PIN_SWITCH_S3);
}

void switchActions(void){
  if (!switchS1Pressed && switchS1PrevPressed){
    Serial.println("Switch S1 pressed");
    digitalWrite(PIN_LED_GAMEPAD, LOW);
    digitalWrite(PIN_LED_MOUSE, HIGH);
    tone(PIN_BUZZER, 65);
    delay(1000);
    noTone(PIN_BUZZER);
  } else if (!switchS2Pressed && switchS2PrevPressed) {
    Serial.println("Switch S2 pressed");
    digitalWrite(PIN_LED_GAMEPAD, HIGH);
    digitalWrite(PIN_LED_MOUSE, LOW);
    tone(PIN_BUZZER, 262);
    delay(1000);
    noTone(PIN_BUZZER);
  } else if (!switchS3Pressed && switchS3PrevPressed) {
    Serial.println("Switch S3 pressed");
    digitalWrite(PIN_LED_GAMEPAD, HIGH);
    digitalWrite(PIN_LED_MOUSE, HIGH);
    tone(PIN_BUZZER, 1047);
    delay(1000);
    noTone(PIN_BUZZER);
  } else {
    digitalWrite(PIN_LED_GAMEPAD, LOW);
    digitalWrite(PIN_LED_MOUSE, LOW);
  }
}
