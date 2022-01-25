
#include <Adafruit_TinyUSB.h> 
#include "LSTimer.h"
int pollTimerId[3];
int stateTimerId;

LSTimer mainPollTimer;
LSTimer mainStateTimer;

void setup() {
  Serial.begin(115200);                                       //Start Serial
  while( !TinyUSBDevice.mounted() ) delay(1);
  while (!Serial) { delay(1); }                               //Start Serial
  Serial.println("Start");
  stateTimerId =  mainStateTimer.startTimer();
}

void loop() {
  // put your main code here, to run repeatedly:
  mainPollTimer.run();
  mainStateTimer.run();
  if(mainStateTimer.elapsedTime(stateTimerId)>=2000){
    Serial.println("hello");
    mainStateTimer.restartTimer(stateTimerId);
  }
}
