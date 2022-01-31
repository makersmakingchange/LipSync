
#include "Adafruit_TinyUSB.h"
#include <Wire.h>  //For I2C communication
#include "LSTimer.h"
#include "LSOutput.h"

#define LED_BRIGHTNESS 50   

typedef struct { 
  uint8_t ledAction;            //off = 0, on = 1, blink = 2
  uint8_t ledColorNumber;
  uint8_t ledNumber;
  uint8_t ledBlinkNumber;
  unsigned long ledBlinkTime;
  uint8_t ledBrightness;
} ledStateStruct; 

ledStateStruct ledCurrentState; 



LSOutput led;

int ledTimerId[9];

LSTimer ledStateTimer;


void setup() {
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  
  delay(3000);

  led.begin();       
  
  initLedFeedback();


  
} //end setup


void loop() {
  ledStateTimer.run();
}




void initLedFeedback(){
    setLedState(1,LED_CLR_GREEN,4,3,500);
    turnLedOff();
    blinkLed();
    setLedState(1,LED_CLR_PINK,4,3,9000);
    turnLedOnce();


}


void initLedFeedback2(){
    turnLedOff();
    setLedState(1,1,4,2,1000);

   for (int i = 0; i <= 8; i++) {
    int color = i;
    if(color==8){color = 0; }
    ledTimerId[i] = ledStateTimer.setTimeout(ledCurrentState.ledBlinkTime*(i+1),turnLedOnWithColor,(void *)color); 
   }
}



void setLedState(int ledAction, int ledColorNumber, int ledNumber,  int ledBlinkNumber, unsigned long ledBlinkTime){ //Set led state after output action is performed 
  if(ledNumber<=OUTPUT_RGB_LED_NUM+1){
    ledCurrentState.ledAction=ledAction;
    ledCurrentState.ledColorNumber=ledColorNumber;
    ledCurrentState.ledNumber=ledNumber;
    ledCurrentState.ledBlinkNumber=ledBlinkNumber;
    ledCurrentState.ledBlinkTime=ledBlinkTime;
    ledCurrentState.ledBrightness=LED_BRIGHTNESS;
  }

}

void turnLedOn(){
  led.setLedColor(ledCurrentState.ledNumber, ledCurrentState.ledColorNumber, ledCurrentState.ledBrightness);
  Serial.println("LED ON");
} 

void turnLedOnWithColor(void * args){
  int color = (int)args;
  led.setLedColor(ledCurrentState.ledNumber, color, ledCurrentState.ledBrightness);
  Serial.println("LED ON WITH COLOR");
}

void turnLedOff(){
    //led.clearLed(ledCurrentState.ledNumber);
    led.setLedColor(ledCurrentState.ledNumber, 0, ledCurrentState.ledBrightness);
    Serial.println("LED OFF");
}


void turnLedOnce(){
  ledTimerId[0] = ledStateTimer.setTimeout(ledCurrentState.ledBlinkTime,turnLedOn);
  Serial.println("LED ON ONCE");
}


void blinkLed() {

  ledTimerId[0] = ledStateTimer.setTimer(ledCurrentState.ledBlinkTime, 0,ledCurrentState.ledBlinkNumber*2,turnLedOn);  
  ledTimerId[1] = ledStateTimer.setTimer(ledCurrentState.ledBlinkTime*2, 0,ledCurrentState.ledBlinkNumber+1,turnLedOff);   
 
}
