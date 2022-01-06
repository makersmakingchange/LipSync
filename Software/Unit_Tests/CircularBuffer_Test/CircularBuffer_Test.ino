
#include "Adafruit_TinyUSB.h"
#include <Wire.h>  //For I2C communication
#include <StopWatch.h>

#define CBUFFER_SIZE 16
#include "LSCircularBuffer.h"
LSCircularBuffer <int> testBuffer(CBUFFER_SIZE);

void setup() {
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  while (!Serial) { delay(1); }
  
  delay(5000);
  for (int i=0; i < CBUFFER_SIZE+10; i++) {
    testBuffer.pushElement(i);
  }
  testBuffer.updateLastElement(21);

  for (int i=0; i < CBUFFER_SIZE; i++) {
    Serial.println(testBuffer.getElement(i));
  }
  
  
} //end setup


void loop() {


}
