
#include <Wire.h>  
#include <StopWatch.h>

#include "LSConfig.h"
#include "LSUSB.h"
#include "LSBLE.h"
#include "LSMemory.h"
#include "LSCircularBuffer.h"
#include "LSInput.h"
#include "LSPressure.h"
#include "LSJoystick.h"
#include "LSOutput.h"

int conMode = 0;

int xVal;
int yVal;

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
    {OUTPUT_LEFT_CLICK,         SAP_MAIN_STATE_PUFF , {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,1,LED_CLR_RED},    0,1000},
    {OUTPUT_RIGHT_CLICK,        SAP_MAIN_STATE_SIP, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,3,LED_CLR_BLUE},   0,1000},
    {OUTPUT_DRAG,               SAP_MAIN_STATE_PUFF , {LED_ACTION_ON,1,LED_CLR_ORANGE,LED_ACTION_ON,1,LED_CLR_YELLOW}, 1000,3000},
    {OUTPUT_SCROLL,             SAP_MAIN_STATE_SIP, {LED_ACTION_ON,3,LED_CLR_ORANGE,LED_ACTION_ON,3,LED_CLR_GREEN},  1000,3000},
    {OUTPUT_CURSOR_CALIBRATION, SAP_MAIN_STATE_PUFF, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,2,LED_CLR_PURPLE},  3000,5000},
    {OUTPUT_MIDDLE_CLICK,       SAP_MAIN_STATE_SIP , {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,2,LED_CLR_ORANGE},  3000,5000}

};

typedef struct { 
  uint8_t inputActionNumber;
  uint8_t inputActionState;
  uint8_t inputActionLedState;
  uint8_t inputActionLedNumber;
  uint8_t inputActionColorNumber;
  unsigned long inputActionStartTime;
  unsigned long inputActionEndTime;
} inputActionStruct;




int inputActionSize;
inputStruct inputButtonActionState, inputSwitchActionState;

const inputActionStruct inputActionProperty[] {
    {OUTPUT_NOTHING,            0, LED_ACTION_OFF,    0,LED_CLR_NONE,   0,0},
    {OUTPUT_LEFT_CLICK,         1 , LED_ACTION_BLINK,  1,LED_CLR_RED,    0,1000},
    {OUTPUT_RIGHT_CLICK,        4, LED_ACTION_BLINK,  3,LED_CLR_BLUE,   0,1000},
    {OUTPUT_DRAG,               5 , LED_ACTION_BLINK,  1,LED_CLR_YELLOW, 1000,3000},
    {OUTPUT_SCROLL,             3, LED_ACTION_BLINK,  3,LED_CLR_GREEN,  1000,3000},
    {OUTPUT_CURSOR_CALIBRATION,  2, LED_ACTION_BLINK,  2,LED_CLR_PURPLE,  0,1000}
};


int inputButtonPinArray[] = {INPUT_BUTTON1_PIN,INPUT_BUTTON2_PIN,INPUT_BUTTON3_PIN};
int inputSwitchPinArray[] = {INPUT_SWITCH1_PIN,INPUT_SWITCH2_PIN,INPUT_SWITCH3_PIN};


sapStruct sapCurrState, sapPrevState, sapActionState;

StopWatch sapStateTimer[1];
StopWatch myTimer[1];

LSCircularBuffer <sapStruct> sapBuffer(12);   //Create a buffer of type sapStruct

LSInput ip(inputButtonPinArray,inputSwitchPinArray,INPUT_BUTTON_NUMBER,INPUT_SWITCH_NUMBER);   //Starts an instance of the object

LSJoystick js;                  //Starts an instance of the LSJoystick object

LSPressure ps;                  //Starts an instance of the LSPressure object

LSOutput led;                   //Starts an instance of the LSOutput led object

LSUSBMouse mouse;               //Starts an instance of the usb mouse object
LSBLEMouse btmouse; 

void setup() {

  mouse.begin();
  btmouse.begin();

  conMode=CON_MODE;
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  //while (!Serial) { delay(1); }
  
  delay(2000);

  initSipAndPuff();

  led.begin();    

  initInput();

  initJoystick(); 
  
  initLedFeedback();

  Scheduler.startLoop(inputLoop);
  
  Scheduler.startLoop(pressureLoop);

  Scheduler.startLoop(joystickLoop);
  
} //end setup

//The loop handling inputs 
void inputLoop() {
  
  ip.update();              //Request new values 


  //Get the last state change 
  inputButtonActionState = ip.getButtonState();
  inputSwitchActionState = ip.getSwitchState();

  //printInputData();
  //Output action logic
  
  for (int i=0; i < inputActionSize; i++) {
    if(inputButtonActionState.mainState==inputActionProperty[i].inputActionState && 
      inputButtonActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputButtonActionState.elapsedTime >= inputActionProperty[i].inputActionStartTime &&
      inputButtonActionState.elapsedTime < inputActionProperty[i].inputActionEndTime){
      
      performAction(i,
      inputActionProperty[i].inputActionLedState,
      inputActionProperty[i].inputActionLedNumber,
      inputActionProperty[i].inputActionColorNumber);
      
      break;
    }
  }

  for (int i=0; i < inputActionSize; i++) {
    if(inputSwitchActionState.mainState==inputActionProperty[i].inputActionState && 
      inputSwitchActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputSwitchActionState.elapsedTime >= inputActionProperty[i].inputActionStartTime &&
      inputSwitchActionState.elapsedTime < inputActionProperty[i].inputActionEndTime){
      
      performAction(i,
      inputActionProperty[i].inputActionLedState,
      inputActionProperty[i].inputActionLedNumber,
      inputActionProperty[i].inputActionColorNumber);
      
      break;
    }
  }
  
  delay(20);
}

//The loop handling pressure polling, sip and puff state evaluation 
void pressureLoop() {
  //resetTimer();
  updatePressure();               //Request new pressure difference from sensor and push it to array

  readPressure();                 //Read the pressure object (can be last value from array, average or other algorithms)

  //printPressureData(); 

  sapPrevState = sapBuffer.getLastElement();  //Get the previous state
  
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
    sapBuffer.updateLastElement(sapCurrState);
  } else {  //None:Sip , None:Puff , Sip:None, Puff:None
      //State: Sip or puff
      //Previous state: {none, waiting, time} Note: There can't be sip or puff and waiting 
      //New state: {Sip or puff, started, 0}
      if(sapPrevState.secondaryState==SAP_SEC_STATE_WAITING){
        sapCurrState = {sapState, SAP_SEC_STATE_STARTED, 0};
        //Serial.println("b");
      } 
      //State: none
      //Previous state: {Sip or puff, started, time} Note: There can't be none and started 
      //New state: {Sip or puff, released, time}      
      else if(sapPrevState.secondaryState==SAP_SEC_STATE_STARTED){
        sapCurrState = {sapPrevState.mainState, SAP_SEC_STATE_RELEASED, sapPrevState.elapsedTime};
        //Serial.println("c");
      }
      //State: None
      //Previous state: {Sip or puff, released, time}
      //New state: {none, waiting, 0}
      else if(sapPrevState.secondaryState==SAP_SEC_STATE_RELEASED && sapState==SAP_MAIN_STATE_NONE){
        sapCurrState = {sapState, SAP_SEC_STATE_WAITING, 0};
        //Serial.println("d");
      }
      //State: Sip or puff
      //Previous state: {none, released, time}
      //New state: {Sip or puff, started, 0}
      else if(sapPrevState.secondaryState==SAP_SEC_STATE_RELEASED && sapState!=SAP_MAIN_STATE_NONE){
        sapCurrState = {sapState, SAP_SEC_STATE_STARTED, 0};
        //Serial.println("e");
      }      
      //Push the new state   
      sapBuffer.pushElement(sapCurrState);
      //Reset and start the timer
      sapStateTimer[0].stop();      
      sapStateTimer[0].reset();                                                                        
      sapStateTimer[0].start(); 
  }

  //No action in 1 minute : reset timer
  if(sapPrevState.secondaryState==SAP_SEC_STATE_WAITING && sapStateTimer[0].elapsed()>30000){
      ps.setZeroPressure();                                   //Update pressure compensation value 
      sapStateTimer[0].stop();                                //Reset and start the timer         
      sapStateTimer[0].reset();                                                                        
      sapStateTimer[0].start();     
  }
  
  for (int i=0; i < ledStateArraySize; i++) {
    if(ledStateArray[i]==LED_ACTION_OFF) {
     led.clearLed(i+1);
     
    }
  }
  //Get the last state change 
  sapActionState = sapBuffer.getLastElement(); 

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
  if(sapActionState.elapsedTime==0){
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
      
      performAction(sapActionIndex,
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
  //Serial.println(getTime());  

}


void loop() {

}


void printInputData() {

  Serial.print(" main: "); Serial.print(inputButtonActionState.mainState);Serial.print(": "); Serial.print(inputSwitchActionState.mainState);Serial.print(", ");
  Serial.print(" secondary: "); Serial.print(inputButtonActionState.secondaryState);Serial.print(": "); Serial.print(inputSwitchActionState.secondaryState);Serial.print(", ");
  Serial.print(" time: "); Serial.print(inputButtonActionState.elapsedTime); Serial.print(": "); Serial.print(inputSwitchActionState.elapsedTime);Serial.print(", ");
  
  Serial.println();
 
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
  sapBuffer.pushElement(sapCurrState);

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
  if(outputAction==OUTPUT_DRAG && (mouse.isPressed(MOUSE_LEFT) || btmouse.isPressed(MOUSE_LEFT))){
    mouse.release(MOUSE_LEFT);
    btmouse.release(MOUSE_LEFT);
  }
  delay(200);
}

void performAction(int action, int ledState, int ledNumber, int ledColor) {
  
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
      case OUTPUT_CURSOR_CALIBRATION: {
        led.setLedBlinkById(ledNumber,2,500,ledColor,LED_BRIGHTNESS);
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        calibrateJoystick();
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
   }
}

void cursorLeftClick(void) {
  //Serial.println("Left Click");
  if(conMode==1){
    mouse.click(MOUSE_LEFT);
  } 
  else if(conMode==2){
    btmouse.click(MOUSE_LEFT);
  }
  delay(80);
}

void cursorRightClick(void) {
  //Serial.println("Right Click");  
  if(conMode==1){
    mouse.click(MOUSE_RIGHT);
  } 
  else if(conMode==2){
    btmouse.click(MOUSE_RIGHT);
  }
  delay(80);
}

void cursorMiddleClick(void) {
  //Serial.println("Middle Click");  
  if(conMode==1){
    mouse.click(MOUSE_MIDDLE);
  } 
  else if(conMode==2){
    btmouse.click(MOUSE_MIDDLE);
  }
  delay(80);
}

void cursorDrag(void) {
  //Serial.println("Drag");  
  if(conMode==1){
    mouse.press(MOUSE_LEFT);
  } 
  else if(conMode==2){
    btmouse.press(MOUSE_LEFT);
  }
  delay(80);
}

void cursorScroll(void) {
  //Serial.println("Scroll");  
}

void centerJoystick(void) {
  //Serial.println("Center Joystick");  
  js.setInputComp();
}

//The loop handling joystick 
void joystickLoop() {
  updateJoystick();               //Request new values

  readJoystick();                 //Read the filtered values 
  
  performJystick();
  
  delay(20);  
}

void initJoystick() {

  js.begin();
  js.setMagDirection(MAG_DIRECTION_INVERSE);
  js.setInputComp();
  calibrateJoystick();
}

void calibrateJoystick() {
  led.setLedBlinkById(4,3,500,LED_CLR_YELLOW,LED_BRIGHTNESS);
  delay(1000);
  centerJoystick();
  js.setInputMax(1);
  js.setInputMax(2);
  js.setInputMax(3);
  js.setInputMax(4);

  /*
  for (int i=1; i < 5; i++) {
  led.setLedBlinkById(6,3,500,LED_CLR_YELLOW,LED_BRIGHTNESS);
  js.setRawMax(1);
  led.setLedBlinkById(1,3,1000,LED_CLR_RED,LED_BRIGHTNESS);    
  }
  */
}

void updateJoystick() {
  js.update();
}


void readJoystick() {

  joystickInputStruct joystickValues = js.getAllVal();
  xVal = joystickValues.x;
  yVal = joystickValues.y;
 
}

void performJystick(){
  
  if(conMode==1){
    (outputAction==OUTPUT_SCROLL)? mouse.scroll(yVal) : mouse.move(xVal, -yVal);
  }
  else if(conMode==2){
    (outputAction==OUTPUT_SCROLL)? btmouse.scroll(yVal) : btmouse.move(xVal, -yVal);
  } 


}

void printJoystickData() {

  Serial.print(" xRaw: "); Serial.print(xVal);Serial.print(", ");
  Serial.print(" yRaw: "); Serial.print(yVal);Serial.print(", ");
  
  Serial.println();
 
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

void initInput(){
  
  ip.begin();
  inputActionSize=sizeof(inputActionProperty)/sizeof(inputActionStruct);

}



void resetTimer() {
  myTimer[0].stop();                                //Reset and start the timer         
  myTimer[0].reset();                                                                        
  myTimer[0].start(); 
}

unsigned long getTime() {
  unsigned long finalTime = myTimer[0].elapsed(); 
  myTimer[0].stop();                                //Reset and start the timer         
  myTimer[0].reset(); 
  return finalTime;
}
