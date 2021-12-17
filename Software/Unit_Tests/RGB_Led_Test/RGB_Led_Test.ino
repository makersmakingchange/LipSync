
#include "Adafruit_TinyUSB.h"
#include <Wire.h>  //For I2C communication

#include "LSOutput.h"

#define LED_BRIGHTNESS 50          

LSOutput led;

void setup() {
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  
  delay(2000);

  led.begin();       
  
  initLedFeedback();


  
} //end setup


void loop() {

  for (int i = 1; i < 5; i++) {
    for (int j = 1; j < 9; j++) {
      led.setLedBlinkById(i,1,500,j,LED_BRIGHTNESS);
      delay(500);
    }
  }

  for (int i = 1; i < 5; i++) {
    for (int j = 1; j < 9; j++) {
      led.setLedColorById(i,j,LED_BRIGHTNESS);
      delay(500);
      led.clearLed(i);
      delay(1000);
    }
  }
  delay(500);
}




void initLedFeedback(){
  led.setLedBlinkById(4,3,500,LED_CLR_GREEN,LED_BRIGHTNESS);
  delay(5);
}
