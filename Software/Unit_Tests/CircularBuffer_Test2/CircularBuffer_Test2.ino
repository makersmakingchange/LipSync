
#include "Adafruit_TinyUSB.h"
#include "LSCircularBuffer.h"
#include <StopWatch.h>

#define CBUFFER_SIZE 8
LSCircularBuffer <char> testBuffer(CBUFFER_SIZE);

// declare a string message.
const String msg = "Happy-Hacking!";

StopWatch myTimer[1];

// startup point entry (runs once).
void setup () {
  // start serial communication.
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  while (!Serial) { delay(1); }
  delay(2000);

  // push all the message's characters to the buffer.
  resetTimer();
  for (int i = 0; i < msg.length(); i++)
    testBuffer.pushElement(msg.charAt(i));
  testBuffer.updateLastElement('#');
  Serial.print("total time: ");
  Serial.println(getTime());    

  Serial.println (testBuffer.getLastElement());
  
  resetTimer();
  for (int i=0; i < CBUFFER_SIZE; i++) {
    Serial.println(testBuffer.getElement(i));
  }
  // print end of line character.
  Serial.println ();
  Serial.print("total time: ");
  Serial.println(getTime());  
}

// loop the main sketch.
void loop () {
  // nothing here.
}

//***RESET TIMER FUNCTION***//

void resetTimer() {
  myTimer[0].stop();                                //Reset and start the timer         
  myTimer[0].reset();                                                                        
  myTimer[0].start(); 
}

//***GET TIME FUNCTION***//

unsigned long getTime() {
  unsigned long finalTime = myTimer[0].elapsed(); 
  myTimer[0].stop();                                //Reset and start the timer         
  myTimer[0].reset(); 
  return finalTime;
}
