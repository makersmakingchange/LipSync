#include <Adafruit_TinyUSB.h> 
#include "LSMemory.h"
#include <StopWatch.h>

#define SETTINGS_FILE    "/settings.txt"
#define SETTINGS_JSON    "{\"configured\":0,\"mode\":1,\"reaction-level\":5,\"sip-thr\":1,\"puff-thr\":1,\"int1\":100,\"int2\":200,\"int3\":300,\"int4\":400,\"int5\":500,\"int6\":600,\"int7\":700,\"int8\":800,\"int9\":900,\"int10\":100,\"int11\":110,\"int12\":220,\"int13\":330,\"int14\":440,\"int15\":550,\"int16\":660,\"int17\":770,\"int18\":880,\"int19\":990,\"int20\":201,\"float1\":3.14,\"float2\":2.71,\"float3\":-3.00,\"float4\":4.00,\"float5\":5.00,\"float6\":6.00,\"string1\":\"Testing 123\",\"string2\":\"MakersMakingChange\"}"

//Initialize 
LSMemory mem;

int deviceConfigured;
int deviceReactionTime;
int deviceReactionLevel;
int deviceMode;
int deviceSipThreshold;
int devicePuffThreshold;

int varInt1;
int varInt2;
int varInt3;
int varInt4;
int varInt5;
int varInt6;
int varInt7;
int varInt8;
int varInt9;
int varInt10;
int varInt11;
int varInt12;
int varInt13;
int varInt14;
int varInt15;
int varInt16;
int varInt17;
int varInt18;
int varInt19;
int varInt20;
float varFloat1;
float varFloat2;
float varFloat3;
float varFloat4;
float varFloat5;
float varFloat6;
String varString1;
String varString2;


unsigned long myTime; 

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
  testMemory();
  Serial.print("test (read and write back) total time: ");
  Serial.println(getTime());
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

void testMemory() {
  //Check if it's first time running the code
    deviceConfigured = mem.readInt(SETTINGS_FILE,"configured");
    delay(1);
    deviceReactionLevel=mem.readInt(SETTINGS_FILE,"reaction_level");
    delay(1);
    deviceMode=mem.readInt(SETTINGS_FILE,"mode");
    delay(1);
    deviceSipThreshold=mem.readInt(SETTINGS_FILE,"sip-thr");
    delay(1);
    devicePuffThreshold=mem.readInt(SETTINGS_FILE,"puff-thr");
    delay(1);
    varInt1=mem.readInt(SETTINGS_FILE,"int1");
    delay(1);
    varInt2=mem.readInt(SETTINGS_FILE,"int2");
    delay(1);
    varInt3=mem.readInt(SETTINGS_FILE,"int3");
    delay(1);
    varInt4=mem.readInt(SETTINGS_FILE,"int4");
    delay(1);
    varInt5=mem.readInt(SETTINGS_FILE,"int5");
    delay(1);
    varInt6=mem.readInt(SETTINGS_FILE,"int6");
    delay(1);
    varInt7=mem.readInt(SETTINGS_FILE,"int7");
    delay(1);
    varInt8=mem.readInt(SETTINGS_FILE,"int8");
    delay(1);
    varInt9=mem.readInt(SETTINGS_FILE,"int9");
    delay(1);
    varInt10=mem.readInt(SETTINGS_FILE,"int10");
    delay(1);
    varInt11=mem.readInt(SETTINGS_FILE,"int11");
    delay(1);
    varInt12=mem.readInt(SETTINGS_FILE,"int12");
    delay(1);
    varInt13=mem.readInt(SETTINGS_FILE,"int13");
    delay(1);
    varInt14=mem.readInt(SETTINGS_FILE,"int14");
    delay(1);
    varInt15=mem.readInt(SETTINGS_FILE,"int15");
    delay(1);
    varInt16=mem.readInt(SETTINGS_FILE,"int16");
    delay(1);
    varInt17=mem.readInt(SETTINGS_FILE,"int17");
    delay(1);
    varInt18=mem.readInt(SETTINGS_FILE,"int18");
    delay(1);
    varInt19=mem.readInt(SETTINGS_FILE,"int19");
    delay(1);
    varInt20=mem.readInt(SETTINGS_FILE,"int20");
    delay(1);
    varFloat1=mem.readFloat(SETTINGS_FILE,"float1");
    delay(1);
    varFloat2=mem.readFloat(SETTINGS_FILE,"float2");
    delay(1);
    varFloat3=mem.readFloat(SETTINGS_FILE,"float3");
    delay(1);
    varFloat4=mem.readFloat(SETTINGS_FILE,"float4");
    delay(1);
    varFloat5=mem.readFloat(SETTINGS_FILE,"float5");
    delay(1);
    varFloat6=mem.readFloat(SETTINGS_FILE,"float6");
    delay(1);
    varString1=mem.readString(SETTINGS_FILE,"string1");
    delay(1);
    varString2=mem.readString(SETTINGS_FILE,"string2");
    delay(1);
  
  Serial.print("Configured: ");
  Serial.println(deviceConfigured); 
  if (deviceConfigured==0) {
    //Define default settings if it's first time running the code
    deviceConfigured=1;
    /*
    deviceReactionLevel=4;
    deviceMode=1;
    deviceSipThreshold=1;
    devicePuffThreshold=5;
    varInt1=10;
    varInt2=20;
    varInt3=30;
    varInt4=40;
    varInt5=50;
    varInt6=60;
    varInt7=70;
    varInt8=80;
    varInt9=90;
    varInt10=100;
    varInt11=10;
    varInt12=120;
    varInt13=130;
    varInt14=140;
    varInt15=150;
    varInt16=160;
    varInt17=170;
    varInt18=180;
    varInt19=190;
    varInt20=200;
    varFloat1=2.54;
    varFloat2=1.27;
    varFloat3=30.0;
    varFloat4=-4.11;
    varFloat5=50.0;
    varFloat6=60.7;
    varString1="Testing 456";
    */
    //varString2="\"Neil Squire Society\"";
    

    //Write default settings to flash storage 
    //mem.writeInt(SETTINGS_FILE,"reaction_level",deviceReactionLevel);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"mode",deviceMode);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"configured",deviceConfigured);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"sip-thr",deviceSipThreshold);    
    //delay(1);
    //writeInt(SETTINGS_FILE,"puff-thr",devicePuffThreshold);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int1",varInt1);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int2",varInt2);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int3",varInt3);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int4",varInt4);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int5",varInt5);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int6",varInt6);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int7",varInt7);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int8",varInt8);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int9",varInt9);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int10",varInt10);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int11",varInt11);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int12",varInt12);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int13",varInt13);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int14",varInt14);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int15",varInt15);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int16",varInt16);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int17",varInt17);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int18",varInt18);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int19",varInt19);
    //delay(1);
    //mem.writeInt(SETTINGS_FILE,"int20",varInt20);
    //delay(1);
    //mem.writeFloat(SETTINGS_FILE,"float1",varFloat1);
    //delay(1);
    //mem.writeFloat(SETTINGS_FILE,"float2",varFloat2);
    //delay(1); 
    //mem.writeFloat(SETTINGS_FILE,"float3",varFloat3);
    //delay(1);
    //mem.writeFloat(SETTINGS_FILE,"float4",varFloat4);
    //delay(1); 
    //mem.writeFloat(SETTINGS_FILE,"float5",varFloat5);
    //delay(1);
    //mem.writeFloat(SETTINGS_FILE,"float6",varFloat6);
    //delay(1); 
    //mem.writeString(SETTINGS_FILE,"string1",varString1);
    //delay(1);
    //mem.writeString(SETTINGS_FILE,"string2",varString2);
    delay(1);               
  } else {
  }  

}


void printMemory() {

  Serial.println(mem.readAll(SETTINGS_FILE));

    //Serial print settings 
    Serial.print("Mode: ");
    Serial.println(deviceMode);

    Serial.print("Reaction Level: ");
    Serial.println(deviceReactionLevel);

    Serial.print("Configured: ");
    Serial.println(deviceConfigured);


    Serial.print("Sip Threshold: ");
    Serial.println(deviceSipThreshold);


    Serial.print("Puff Threshold: ");
    Serial.println(devicePuffThreshold);

    Serial.print("int1: ");
    Serial.println(varInt1);


    Serial.print("int2: ");
    Serial.println(varInt2);


    Serial.print("int3: ");
    Serial.println(varInt3);


    Serial.print("int4: ");
    Serial.println(varInt4);


    Serial.print("int5: ");
    Serial.println(varInt5);


    Serial.print("int6: ");
    Serial.println(varInt6);


    Serial.print("int7: ");
    Serial.println(varInt7);


    Serial.print("int8: ");
    Serial.println(varInt8);


    Serial.print("int9: ");
    Serial.println(varInt9);


    Serial.print("int10: ");
    Serial.println(varInt10);

    Serial.print("int11: ");
    Serial.println(varInt11);


    Serial.print("int12: ");
    Serial.println(varInt12);


    Serial.print("int13: ");
    Serial.println(varInt13);


    Serial.print("int14: ");
    Serial.println(varInt14);


    Serial.print("int15: ");
    Serial.println(varInt15);


    Serial.print("int16: ");
    Serial.println(varInt16);


    Serial.print("int17: ");
    Serial.println(varInt17);


    Serial.print("int18: ");
    Serial.println(varInt18);


    Serial.print("int19: ");
    Serial.println(varInt19);


    Serial.print("int20: ");
    Serial.println(varInt20);

    Serial.print("float1: ");
    Serial.println(varFloat1);

    Serial.print("float2: ");
    Serial.println(varFloat2);

    Serial.print("float3: ");
    Serial.println(varFloat3);

    Serial.print("float4: ");
    Serial.println(varFloat4);

    Serial.print("float5: ");
    Serial.println(varFloat5);

    Serial.print("float6: ");
    Serial.println(varFloat6);


    Serial.print("string1: ");
    Serial.println(varString1);
    
    Serial.print("string2: ");
    Serial.println(varString2);
}
