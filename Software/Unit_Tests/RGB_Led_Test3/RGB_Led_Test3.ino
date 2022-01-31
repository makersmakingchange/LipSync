
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

int ledBlinkTimerId[8];

LSTimer ledStateTimer;


void setup() {
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  
  delay(3000);

  led.begin();       
  
  initLedFeedback2();


  
} //end setup


void loop() {
ledStateTimer.run();
}




void initLedFeedback(){
    setLedState(1,3,4,3,500);
    performLedOff();
    performLedBlink();
    setLedState(1,3,4,3,9000);
    //performLedOnce();


}


void initLedFeedback2(){
      performLedOff();
      setLedState(1,1,4,3,2000);
      ledBlinkTimerId[0] = ledStateTimer.setTimer(ledCurrentState.ledBlinkTime,0,ledCurrentState.ledBlinkNumber*2,performLedOn); 
      setLedState(1,3,4,3,2000);
      ledBlinkTimerId[1] = ledStateTimer.setTimer(ledCurrentState.ledBlinkTime*2,0,ledCurrentState.ledBlinkNumber+1,performLedOff);  
      //setLedState(1,4,4,1,2000);
      //ledBlinkTimerId[2] = ledStateTimer.setTimer(ledCurrentState.ledBlinkTime,0,1,performLedOn); 
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

void performLedOn(){
  led.setLedColor(ledCurrentState.ledNumber, ledCurrentState.ledColorNumber, ledCurrentState.ledBrightness);
  Serial.println("LED ON");
}

void performLedOff(){
    //led.clearLed(ledCurrentState.ledNumber);
    led.setLedColor(ledCurrentState.ledNumber, 0, ledCurrentState.ledBrightness);
    Serial.println("LED OFF");
}


void performLedOnce(){
  ledBlinkTimerId[2] = ledStateTimer.setTimeout(ledCurrentState.ledBlinkTime,performLedOn);
  Serial.println("LED ON ONCE");
}


void performLedBlink() {

  ledBlinkTimerId[0] = ledStateTimer.setTimer(ledCurrentState.ledBlinkTime, 0,ledCurrentState.ledBlinkNumber*2,performLedOn);  
  ledBlinkTimerId[1] = ledStateTimer.setTimer(ledCurrentState.ledBlinkTime*2, 0,ledCurrentState.ledBlinkNumber+1,performLedOff);   
 
  
}
