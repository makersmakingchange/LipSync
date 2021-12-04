
#include "Adafruit_TinyUSB.h"
#include <Wire.h>  //For I2C communication

#include "LSPressure.h"
#include "LSOutput.h"

#define OUTPUT_NOTHING 0                              // No action
#define OUTPUT_LEFT_CLICK 1                           // Generates a short left click
#define OUTPUT_RIGHT_CLICK 2                          // Generates a short right click
#define OUTPUT_DRAG 3                                 // Initiates drag mode, holding down left click until cancelled
#define OUTPUT_SCROLL 4                               // Initiates scroll mode. Vertical motion generates mouse scroll wheel movement.
#define OUTPUT_MIDDLE_CLICK 5                         // Generates a short middle click
#define OUTPUT_CURSOR_HOME_RESET 6                    // Initiates the cursor home reset routine to reset center position. 
#define OUTPUT_CURSOR_CALIBRATION 7                   // Initiates the cursor calibration to calibrate joystick range and reset center position.

//#define OUTPUT_RGB_LED_PIN A3 //output pin for neopixel
//#define OUTPUT_RGB_LED_NUM 3

#define SIP_THRESHOLD -1.0                    //hPa
#define PUFF_THRESHOLD 1.0

#define SP_STATE_WAITING 0
#define SP_STATE_STARTED 1
#define SP_STATE_DETECTED 2

#define SP_ACTION_NONE 0
#define SP_ACTION_SIP 1
#define SP_ACTION_PUFF 2


#define LED_BRIGHTNESS 50          

boolean puffState; //if true, we're currently in a puff state 
boolean sipState;

float myLPSpres; //read sensor
float myBMPpres;
float myRawPres;

uint32_t led1StateColor;
uint32_t led2StateColor;
uint32_t led3StateColor;

float sipThreshold;
float puffThreshold;


typedef struct { 
  uint8_t pressureActionNumber;
  String pressureActionName;
  uint8_t pressureActionLedNumber;
  uint8_t pressureActionColorNumber;
} pressureActionStruct;

const pressureActionStruct pressureActionProperty[] {
    {1,"Left Click",1,LED_CLR_RED},
    {2,"Right Click",3,LED_CLR_BLUE},
    {3,"Drag Mode",2,LED_CLR_YELLOW},
    {4,"Scroll Mode",2,LED_CLR_GREEN},
    {5,"Middle Click",2,LED_CLR_NONE},
    {6,"Center Reset",2,LED_CLR_NONE},
    {7,"Calibration",2,LED_CLR_NONE}
};

LSPressure ps;

LSOutput led;

void setup() {
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  while (!Serial) { delay(1); }
  
  delay(2000);
  
  beginPressure();

  led.begin();       
  
  initLedFeedback();

  setPressureThreshold();

  
} //end setup


void loop() {

  //clearLed(4);

  
  updatePressure();

  readPressure();

  //plotPressureData(); 
  
  puffState = false;
  sipState = false;
  //check for sip and puff conditions
  if (myRawPres > puffThreshold)  { 
    puffState = true;
    led.setLedBlinkById(1,1,500,pressureActionProperty[0].pressureActionColorNumber,LED_BRIGHTNESS);
  }
  if (myRawPres < sipThreshold)  { 
    sipState = true;
    led.setLedBlinkById(3,1,500,pressureActionProperty[1].pressureActionColorNumber,LED_BRIGHTNESS);
  }

  
  delay(10);
}

void beginPressure() {

  ps.begin();
}

void plotPressureData() {
  //report gathered data:
  Serial.print(" myBMPpres: "); Serial.print(myBMPpres);Serial.print(", ");
  Serial.print(" myLPSpres: "); Serial.print(myLPSpres);Serial.print(", ");
  Serial.print(" myRawPres: "); Serial.print(myRawPres);Serial.print(", ");
  
  Serial.println();
 
}//end plot

void setPressureThreshold(){
  sipThreshold = SIP_THRESHOLD;
  puffThreshold = PUFF_THRESHOLD;
}


void readPressure() {

  pressureType pressureValues = ps.getAllPressure();
  myBMPpres = pressureValues.refPressure;
  myLPSpres = pressureValues.mainPressure;
  myRawPres = pressureValues.rawPressure;
  
 
}

//=====  fetch TLV493D Magnetic Sensor data (external sensor) ===== 
void updatePressure() {

  ps.update();
}

void performButtonAction(int outputAction) {

    switch (outputAction) {
      case OUTPUT_NOTHING: {
        //do nothing
        break;
      }
      case OUTPUT_LEFT_CLICK: {
        cursorLeftClick();
        delay(5);
        break;
      }
      case OUTPUT_RIGHT_CLICK: {
        cursorRightClick();
        delay(5);
        break;
      }
      case OUTPUT_DRAG: {
        cursorDrag();
        delay(5);
        break;
      }
      case OUTPUT_SCROLL: {
        cursorScroll(); //Enter Scroll mode
        delay(5);
        break;
      }
      case OUTPUT_MIDDLE_CLICK: {
        //Perform cursor middle click
        cursorMiddleClick();
        delay(5);
        break;
      }
      case OUTPUT_CURSOR_HOME_RESET: {
        centerReset();
        delay(5);
        break;
      }
      case OUTPUT_CURSOR_CALIBRATION: {
        calibration();
        delay(5);
        break;
      }
   }
}

void cursorLeftClick(void) {
  
}

void cursorRightClick(void) {
  
}

void cursorMiddleClick(void) {
  
}

void cursorDrag(void) {
  
}

void cursorScroll(void) {
  
}

void centerReset(void) {
  
}

void calibration(void) {
  
}




void initLedFeedback(){
  led.setLedBlinkById(4,3,500,LED_CLR_GREEN,LED_BRIGHTNESS);
  delay(5);
}
