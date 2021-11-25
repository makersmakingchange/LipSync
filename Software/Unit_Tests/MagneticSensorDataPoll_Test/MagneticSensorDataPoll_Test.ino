
#include "Adafruit_TinyUSB.h"
#include <Wire.h>  //For I2C communication
#include "LSJoystick.h"
//#include <Tlv493d.h>//Infinion TLV493 magnetic sensor

//#define JS_STACK_SIZE 10
#define neoPIN A3 //output pin for neopixel

//Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();
LSJoystick js;
float xRaw;
float yRaw;
float xComp=10;
float yComp=-100;
float zComp=-100;
float xLeveled;
float yLeveled;
int xFinal;
int yFinal;


//int testStack[JS_STACK_SIZE] = {1,2,3,4,5,6,7,8,9,10}; 

void setup() {
  
   Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  while (!Serial) { delay(1); }
  delay(3000);
  joystickBegin(); //TLV493D Magnetic Sensor (external module)

  joystickCalibrate(); //call subroutine to "Zero" joystick


  pinMode(neoPIN, OUTPUT);
  /*
  memmove( testStack, &testStack[1], (JS_STACK_SIZE-1)*sizeof(testStack[0]));
  testStack[JS_STACK_SIZE-1]=13;
  
  for (int i=0; i < JS_STACK_SIZE; i++) {
    delay(2000);
    Serial.println(testStack[i]);
    
  }
  */

} //end setup


void loop() {
  
  //=====  fetch TLV493D Magnetic Sensor data (external sensor) =====
  joystickUpdate();
  delay(50); 
  joystickRead();


  xLeveled=xRaw-xComp;
  yLeveled=yRaw-yComp;
  xFinal = map(xLeveled, -40, 40, 12, -12); //map(value, fromLow, fromHigh, toLow, toHigh);
  yFinal = map(yLeveled, -40, 40, -12, 12); //map(value, fromLow, fromHigh, toLow, toHigh);
  
  //lipSyncMouse.move(xFinal, yFinal);
  
  delay(10);
}

void joystickBegin() {
  
  js.begin();
}

void plotData() {
  //report gathered data:
  Serial.print(" xRaw: "); Serial.print(xRaw);Serial.print(", ");
  Serial.print(" yRaw: "); Serial.print(yRaw);Serial.print(", ");
  Serial.println();
 
}//end plot

void joystickRead() {
  xRaw = js.getAll().xVal;
  yRaw = js.getAll().yVal;
  plotData();
}

//=====  fetch TLV493D Magnetic Sensor data (external sensor) ===== 
void joystickUpdate() {

  js.update();
 
}

void joystickClear() {
  js.clear();
}


//=====  Calibrate Joystick ===== 
void joystickCalibrate() {
  //stashes current raw x and y sensor readings, to be subtracted from normal readings. This will "Zero" the joystick, reducing drift.
  joystickUpdate();
  joystickRead();
  xComp = xRaw; 
  yComp = yRaw;
  xLeveled=xRaw-xComp;
  yLeveled=yRaw-yComp;
  xFinal = map(xLeveled, -40, 40, 12, -12); //map(value, fromLow, fromHigh, toLow, toHigh);
  yFinal = map(yLeveled, -40, 40, -12, 12); //map(value, fromLow, fromHigh, toLow, toHigh);

}
