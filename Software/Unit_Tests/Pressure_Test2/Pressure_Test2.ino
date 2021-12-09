#include <Arduino.h>
#include "Adafruit_TinyUSB.h"
#include <Wire.h>  
#include <StopWatch.h>

#include "LSQueue.h"
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



#define SOFT_SIP_THRESHOLD -2.0                    //hPa
#define SOFT_PUFF_THRESHOLD 2.0

#define HARD_SIP_THRESHOLD -10.0                    //hPa
#define HARD_PUFF_THRESHOLD 10.0

#define SAP_MAIN_STATE_NONE 0
#define SAP_MAIN_STATE_SIP 1
#define SAP_MAIN_STATE_PUFF 2

#define SAP_SEC_STATE_WAITING 0
#define SAP_SEC_STATE_STARTED 1
#define SAP_SEC_STATE_RELEASED 2

#define LED_BRIGHTNESS 50 

#define LED_ACTION_OFF 0
#define LED_ACTION_ON 1
#define LED_ACTION_BLINK 2

int sapState;
int outputAction;

float myLPSpres; //read sensor
float myBMPpres;
float myRawPres;

uint8_t ledStateArray[3] = {LED_ACTION_OFF,LED_ACTION_OFF,LED_ACTION_OFF};

float sipThreshold;
float puffThreshold;

typedef struct {
  int mainState;            //none = 0, sip = 1, puff = 2
  int secondaryState;       //waiting = 0, started = 1, detected = 2
  unsigned long elapsedTime;     //in ms
} sapStruct;

typedef struct { 
  uint8_t sapLedStartAction;
  uint8_t sapLedStartNumber;
  uint8_t sapLedStartColor;
  uint8_t sapLedEndAction;
  uint8_t sapLedEndNumber;
  uint8_t sapLedEndColor;
} sapLedStruct;


typedef struct { 
  uint8_t sapActionNumber;
  uint8_t sapActionState;
  sapLedStruct sapActionLedState;
  unsigned long sapActionStartTime;
  unsigned long sapActionEndTime;
} sapActionStruct;

int sapActionSize;
int ledStateArraySize;

const sapActionStruct sapActionProperty[] {
    {OUTPUT_NOTHING,            SAP_MAIN_STATE_NONE, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_OFF,0,LED_CLR_NONE},   0,0},
    {OUTPUT_LEFT_CLICK,         SAP_MAIN_STATE_SIP , {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,1,LED_CLR_RED},    0,1000},
    {OUTPUT_RIGHT_CLICK,        SAP_MAIN_STATE_PUFF, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,3,LED_CLR_BLUE},   0,1000},
    {OUTPUT_DRAG,               SAP_MAIN_STATE_SIP , {LED_ACTION_ON,1,LED_CLR_ORANGE,LED_ACTION_ON,1,LED_CLR_YELLOW}, 1000,3000},
    {OUTPUT_SCROLL,             SAP_MAIN_STATE_PUFF, {LED_ACTION_ON,3,LED_CLR_ORANGE,LED_ACTION_ON,3,LED_CLR_GREEN},  1000,3000},
    {OUTPUT_MIDDLE_CLICK,       SAP_MAIN_STATE_SIP , {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,2,LED_CLR_ORANGE},  3000,5000},
    {OUTPUT_CURSOR_HOME_RESET,  SAP_MAIN_STATE_PUFF, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,2,LED_CLR_PURPLE},  3000,5000}

};




sapStruct sapCurrState, sapPrevState, sapActionState;

StopWatch sapStateTimer[1];

LSQueue <sapStruct> sapQueue;   //Create a Queue of type sapStruct

LSPressure ps;                  //Starts an instance of the LSPressure object

LSOutput led;                   //Starts an instance of the LSOutput led object



void setup() {
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  while (!Serial) { delay(1); }
  
  delay(2000);

  initSipAndPuff();

  led.begin();    
  
  initLedFeedback();
  
  Scheduler.startLoop(pressureDataloop);
  
} //end setup


//The loop handling pressure polling, sip and puff state evaluation 
void pressureDataloop() {
  
  updatePressure();               //Request new pressure difference from sensor and push it to array

  readPressure();                 //Read the pressure object (can be last value from array, average or other algorithms)

  //printPressureData(); 

  sapPrevState = sapQueue.end();  //Get the previous state
  
  //check for sip and puff conditions
  if (myRawPres > puffThreshold)  { 
    sapState = SAP_MAIN_STATE_PUFF;
  } else if (myRawPres < sipThreshold)  { 
    sapState = SAP_MAIN_STATE_SIP;
  } else {
    sapState = SAP_MAIN_STATE_NONE;
  }

  //None:None, Sip:Sip, Puff:Puff
  //Update time
  if(sapPrevState.mainState == sapState){
    sapCurrState = {sapState, sapPrevState.secondaryState, sapStateTimer[0].elapsed()};
    //Serial.println("a");
    sapQueue.update(sapCurrState);
  } else {  //None:Sip , None:Puff , Sip:None, Puff:None
      //State: None
      //Previous state: {Sip or puff, released, time}
      //New state: {none, waiting, 0}
      if(sapPrevState.secondaryState==SAP_SEC_STATE_RELEASED && sapState==SAP_MAIN_STATE_NONE){
        sapCurrState = {sapState, SAP_SEC_STATE_WAITING, 0};
        //Serial.println("b");
      }
      //State: Sip or puff
      //Previous state: {none, released, time}
      //New state: {Sip or puff, started, 0}
      else if(sapPrevState.secondaryState==SAP_SEC_STATE_RELEASED && sapState!=SAP_MAIN_STATE_NONE){
        sapCurrState = {sapState, SAP_SEC_STATE_STARTED, 0};
        //Serial.println("c");
      }      
      //State: Sip or puff
      //Previous state: {none, waiting, time} Note: There can't be sip or puff and waiting 
      //New state: {Sip or puff, started, 0}
      else if(sapPrevState.secondaryState==SAP_SEC_STATE_WAITING){
        sapCurrState = {sapState, SAP_SEC_STATE_STARTED, 0};
        //Serial.println("d");
      } 
      //State: none
      //Previous state: {Sip or puff, started, time} Note: There can't be none and started 
      //New state: {Sip or puff, released, time}      
      else if(sapPrevState.secondaryState==SAP_SEC_STATE_STARTED){
        sapCurrState = {sapPrevState.mainState, SAP_SEC_STATE_RELEASED, sapPrevState.elapsedTime};
        //Serial.println("e");
      }
      //Push the new state   
      sapQueue.push(sapCurrState);
      if(sapQueue.count()==12){sapQueue.pop(); }  //Keep last 12 objects 
      //Reset and start the timer
      sapStateTimer[0].stop();      
      sapStateTimer[0].reset();                                                                        
      sapStateTimer[0].start(); 
  }

  //No action in 1 minute : reset timer
  if(sapPrevState.secondaryState==SAP_SEC_STATE_WAITING && sapStateTimer[0].elapsed()>60000){
      sapStateTimer[0].stop();                                //Reset and start the timer         
      sapStateTimer[0].reset();                                                                        
      sapStateTimer[0].start();     
  }

  delay(20);
}


void loop() {
  for (int i=0; i < ledStateArraySize; i++) {
    if(ledStateArray[i]==LED_ACTION_OFF) {
     led.clearLed(i+1);
     
    }
  }
  //Get the last state change 
  sapActionState = sapQueue.end(); 

  //printSipAndPuffData();


  //Output action logic

  int canPerformAction = true;

  //Skip Sip and puff action if it's in drag or scroll mode

  if((sapActionState.secondaryState == SAP_SEC_STATE_STARTED ||
      sapActionState.secondaryState == SAP_SEC_STATE_RELEASED) &&
      (outputAction == OUTPUT_SCROLL ||
      outputAction == OUTPUT_DRAG)){
      releaseHoldAction();
      outputAction=OUTPUT_NOTHING;
      canPerformAction=false;
      
  }

  int sapActionIndex = 0;

  //Perform output action and led action on sip and puff release 
  //Perform led action on sip and puff start
  while (sapActionIndex < sapActionSize && canPerformAction){
    //Perform output action and led action on sip and puff release 
    if(sapActionState.mainState==sapActionProperty[sapActionIndex].sapActionState && 
      sapActionState.secondaryState == SAP_SEC_STATE_RELEASED &&
      sapActionState.elapsedTime >= sapActionProperty[sapActionIndex].sapActionStartTime &&
      sapActionState.elapsedTime < sapActionProperty[sapActionIndex].sapActionEndTime){
      
      performSapAction(sapActionIndex,
      sapActionProperty[sapActionIndex].sapActionLedState.sapLedEndAction,
      sapActionProperty[sapActionIndex].sapActionLedState.sapLedEndNumber,
      sapActionProperty[sapActionIndex].sapActionLedState.sapLedEndColor);
      
      outputAction=sapActionIndex;      //used for releasing drag or scroll
      
      break;
    } //Perform led action on sip and puff start
    else if(sapActionState.mainState==sapActionProperty[sapActionIndex].sapActionState && 
      sapActionState.secondaryState == SAP_SEC_STATE_STARTED &&
      sapActionState.elapsedTime >= sapActionProperty[sapActionIndex].sapActionStartTime &&
      sapActionState.elapsedTime < sapActionProperty[sapActionIndex].sapActionEndTime){
        
      led.setLedColorById(sapActionProperty[sapActionIndex].sapActionLedState.sapLedStartNumber, 
      sapActionProperty[sapActionIndex].sapActionLedState.sapLedStartColor, 
      LED_BRIGHTNESS); 
      break;
    }
    sapActionIndex++;
    
  }
  
  delay(20);
}


void updatePressure() {
  ps.update();
}


void readPressure() {

  pressureStruct pressureValues = ps.getAllPressure();
  myBMPpres = pressureValues.refPressure;
  myLPSpres = pressureValues.mainPressure;
  myRawPres = pressureValues.rawPressure;
  
 
}

void printPressureData() {

  Serial.print(" myBMPpres: "); Serial.print(myBMPpres);Serial.print(", ");
  Serial.print(" myLPSpres: "); Serial.print(myLPSpres);Serial.print(", ");
  Serial.print(" myRawPres: "); Serial.print(myRawPres);Serial.print(", ");
  
  Serial.println();

}

void initSipAndPuff() {

  ps.begin(PRESS_TYPE_DIFF);
  setSipAndPuffThreshold();
  initSipAndPuffArray();
  sapActionSize=sizeof(sapActionProperty)/sizeof(sapActionStruct);
}

void setSipAndPuffThreshold(){
  sipThreshold = SOFT_SIP_THRESHOLD;
  puffThreshold = SOFT_PUFF_THRESHOLD;
}

void initSipAndPuffArray(){

  //Push initial state to state Queue
  
  //sapStateArray[0] = {SAP_MAIN_STATE_NONE, SAP_SEC_STATE_WAITING, 0};
  sapCurrState = sapPrevState = {SAP_MAIN_STATE_NONE, SAP_SEC_STATE_WAITING, 0};
  sapQueue.push(sapCurrState);

  //Reset and start the timer   
  sapStateTimer[0].stop();                                      
  sapStateTimer[0].reset();                                                                        
  sapStateTimer[0].start(); 
}

void printSipAndPuffData() {

  Serial.print(" main: "); Serial.print(sapActionState.mainState);Serial.print(", ");
  Serial.print(" secondary: "); Serial.print(sapActionState.secondaryState);Serial.print(", ");
  Serial.print(" time: "); Serial.print(sapActionState.elapsedTime);Serial.print(", ");
  
  Serial.println();
 
}


void releaseHoldAction(){
  for (int i=0; i < ledStateArraySize; i++) {
     led.clearLed(i+1);
     ledStateArray[i]=LED_ACTION_OFF;
  }
  delay(200);
}

void performSapAction(int action, int ledState, int ledNumber, int ledColor) {
  
    switch (action) {
      case OUTPUT_NOTHING: {
        //do nothing
        break;
      }
      case OUTPUT_LEFT_CLICK: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        cursorLeftClick();
        setLedState(ledState,ledNumber);
        delay(5);
        break;
      }
      case OUTPUT_RIGHT_CLICK: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        cursorRightClick();
        setLedState(ledState,ledNumber);
        delay(5);
        break;
      }
      case OUTPUT_DRAG: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        cursorDrag();
        setLedState(ledState,ledNumber);
        delay(5);
        break;
      }
      case OUTPUT_SCROLL: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        cursorScroll(); //Enter Scroll mode
        setLedState(ledState,ledNumber);
        delay(5);
        break;
      }
      case OUTPUT_MIDDLE_CLICK: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        //Perform cursor middle click
        cursorMiddleClick();
        setLedState(ledState,ledNumber);
        delay(5);
        break;
      }
      case OUTPUT_CURSOR_HOME_RESET: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        centerReset();
        setLedState(ledState,ledNumber);
        delay(5);
        break;
      }
   }
}

void cursorLeftClick(void) {
  Serial.println("Left Click");
  delay(100);
}

void cursorRightClick(void) {
  Serial.println("Right Click");  
  delay(100);
}

void cursorMiddleClick(void) {
  Serial.println("Middle Click");  
  delay(100);
}

void cursorDrag(void) {
  Serial.println("Drag");  
}

void cursorScroll(void) {
  Serial.println("Scroll");  
}

void centerReset(void) {
  Serial.println("Center Reset");  
}

void initLedFeedback(){
  led.setLedBlinkById(4,3,500,LED_CLR_GREEN,LED_BRIGHTNESS);
  ledStateArraySize=sizeof(ledStateArray)/sizeof(uint8_t);
  delay(5);
}

//Set led state after output action is performed 
void setLedState(int ledState, int ledNumber){
  if(ledNumber<=ledStateArraySize){
    if(ledState==LED_ACTION_BLINK) {ledStateArray[ledNumber-1]=LED_ACTION_OFF; }
    else{ledStateArray[ledNumber-1]=ledState;}
  }
  else if(ledNumber==ledStateArraySize+1){
      for (int i=0; i < ledStateArraySize; i++) {
        if(ledState==LED_ACTION_BLINK) {ledStateArray[i]=LED_ACTION_OFF; }
        else{ledStateArray[i]=ledState;}
      }
  }
}
