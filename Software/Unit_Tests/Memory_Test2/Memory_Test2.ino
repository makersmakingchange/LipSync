#include <Adafruit_TinyUSB.h> 
#include "LSMemory.h"
#include <StopWatch.h>

#define SETTINGS_FILE    "/settings.txt"
#define SETTINGS_JSON    "{\"configured\":0,\"mode\":1,\"reaction-level\":5,\"sip-thr\":1.0,\"puff-thr\":1.5,\"point\":[1.0,2.9],\"text\":\"MakersMakingChange\"}"

//Initialize 
LSMemory mem;

int deviceConfigured;
float sipThreshold;
pointFloatType testPoint;
String testText;

unsigned long myTime; 

pointFloatType writePoint = {5.0,3.7};
float writeSipThreshold = 5.9;
String writeText = "NeilSquire";
StopWatch myTimer[1];

void setup() {

  myTime = 0;
  Serial.begin(115200);                                       //Start Serial
  while( !TinyUSBDevice.mounted() ) delay(1);
  while (!Serial) { delay(1); }                               //Start Serial
  mem.begin();
  
  delay(2000);
  resetTimer();
  mem.format();
  Serial.print("format total time: ");
  Serial.println(getTime());
  delay(2000);
  
  resetTimer();
  mem.initialize(SETTINGS_FILE,SETTINGS_JSON);   
  //Setup FileSystem
  Serial.print("init total time: ");
  Serial.println(getTime());
  delay(5);

  
  resetTimer();
  testReadMemory();
  Serial.print("test (read) total time: ");
  Serial.println(getTime());
  printMemory();
  delay(500);
  
  resetTimer();
  testWriteMemory();
  Serial.print("test (write) total time: ");
  Serial.println(getTime());
  delay(500);
  testReadMemory();
  printMemory();
};

void loop() {
  
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

//***TEST MEMORY FUNCTION***//

void testReadMemory() {

  //Check if it's first time running the code
    deviceConfigured = mem.readInt(SETTINGS_FILE,"configured");
    delay(1);
    sipThreshold = mem.readFloat(SETTINGS_FILE,"sip-thr");
    delay(1);
    testPoint=mem.readPoint(SETTINGS_FILE,"point");
    delay(1);
    testText=mem.readString(SETTINGS_FILE,"text");
    delay(1);

}

//***TEST WRITE MEMORY FUNCTION***//

void testWriteMemory() {
    mem.writeInt(SETTINGS_FILE,"configured",5);
    delay(5);

    mem.writeFloat(SETTINGS_FILE,"sip-thr",writeSipThreshold);
    delay(5);
    
    mem.writePoint(SETTINGS_FILE,"point",writePoint);
    delay(1);

    mem.writeString(SETTINGS_FILE,"text",writeText);
    delay(1);
}

void printMemory() {

    Serial.println(mem.readAll(SETTINGS_FILE));

    //Serial print settings 
    Serial.print("x: ");
    Serial.println(testPoint.x);

    Serial.print("y: ");
    Serial.println(testPoint.y);

    Serial.print("Configured: ");
    Serial.println(deviceConfigured);

    Serial.print("sipThreshold: ");
    Serial.println(sipThreshold);

    Serial.print("text: ");
    Serial.println(testText);

}
