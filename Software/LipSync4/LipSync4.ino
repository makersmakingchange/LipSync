#include <Wire.h>  
#include <StopWatch.h>
#include "LSTimer.h"
#include <ArduinoJson.h>
#include "LSConfig.h"
#include "LSUSB.h"
#include "LSBLE.h"
#include "LSCircularBuffer.h"
#include "LSInput.h"
#include "LSPressure.h"
#include "LSJoystick.h"
#include "LSOutput.h"
#include "LSMemory.h"

int comMethod = 0;


//Input module variables and structures 

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
    {CONF_ACTION_NOTHING,            0, LED_ACTION_OFF,    0,LED_CLR_NONE,   0,0},
    {CONF_ACTION_LEFT_CLICK,         1 , LED_ACTION_BLINK,  1,LED_CLR_RED,    0,1000},
    {CONF_ACTION_RIGHT_CLICK,        4, LED_ACTION_BLINK,  3,LED_CLR_BLUE,   0,1000},
    {CONF_ACTION_DRAG,               5 , LED_ACTION_BLINK,  1,LED_CLR_YELLOW, 1000,3000},
    {CONF_ACTION_SCROLL,             3, LED_ACTION_BLINK,  3,LED_CLR_GREEN,  1000,3000},
    {CONF_ACTION_NOTHING,            2, LED_ACTION_BLINK,  2,LED_CLR_PURPLE,  0,1000}
};


int inputButtonPinArray[] = {CONF_BUTTON1_PIN,CONF_BUTTON2_PIN,CONF_BUTTON3_PIN};
int inputSwitchPinArray[] = {CONF_SWITCH1_PIN,CONF_SWITCH2_PIN,CONF_SWITCH3_PIN};


//Pressure module variables and structures 
int sapState;
int outputAction;

float mainPressure; 
float refPressure;
float diffPressure;

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

const sapActionStruct sapActionProperty[] {
    {CONF_ACTION_NOTHING,            CONF_SAP_MAIN_STATE_NONE, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_OFF,0,LED_CLR_NONE},   0,0},
    {CONF_ACTION_LEFT_CLICK,         CONF_SAP_MAIN_STATE_PUFF , {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,1,LED_CLR_RED},    0,1000},
    {CONF_ACTION_RIGHT_CLICK,        CONF_SAP_MAIN_STATE_SIP, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,3,LED_CLR_BLUE},   0,1000},
    {CONF_ACTION_DRAG,               CONF_SAP_MAIN_STATE_PUFF , {LED_ACTION_ON,1,LED_CLR_ORANGE,LED_ACTION_ON,1,LED_CLR_YELLOW}, 1000,3000},
    {CONF_ACTION_SCROLL,             CONF_SAP_MAIN_STATE_SIP, {LED_ACTION_ON,3,LED_CLR_ORANGE,LED_ACTION_ON,3,LED_CLR_GREEN},  1000,3000},
    {CONF_ACTION_CURSOR_CALIBRATION, CONF_SAP_MAIN_STATE_PUFF, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,2,LED_CLR_PURPLE},  3000,5000},
    {CONF_ACTION__MIDDLE_CLICK,       CONF_SAP_MAIN_STATE_SIP , {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,2,LED_CLR_ORANGE},  3000,5000}

};


sapStruct sapCurrState, sapPrevState, sapActionState;

LSCircularBuffer <sapStruct> sapBuffer(12);   //Create a buffer of type sapStruct

//Joystick module variables and structures 

int xVal;
int yVal;

//Timer related variables 
StopWatch sapStateTimer[1];
StopWatch myTimer[1];

int pollTimerId[3];
int stateTimerId[1];

LSTimer mainPollTimer;
LSTimer mainStateTimer;

//Create instances of classes

LSMemory mem;

LSInput ib(inputButtonPinArray,CONF_BUTTON_NUMBER); 
LSInput is(inputSwitchPinArray,CONF_SWITCH_NUMBER);   //Starts an instance of the object

LSJoystick js;                  //Starts an instance of the LSJoystick object

LSPressure ps;                  //Starts an instance of the LSPressure object

LSOutput led;                   //Starts an instance of the LSOutput led object

LSUSBMouse mouse;               //Starts an instance of the usb mouse object
LSBLEMouse btmouse; 


void setup() {

  mouse.begin();
  btmouse.begin();

  comMethod=CONF_COM_METHOD;
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  //while (!Serial) { delay(1); }
  
  delay(2000);

  initMemory();

  initSipAndPuff();

  led.begin();    

  initInput();

  initJoystick(); 
  
  initLedFeedback();

  pollTimerId[0] = mainPollTimer.setInterval(CONF_JOYSTICK_POLL_RATE, joystickLoop);
  pollTimerId[1] = mainPollTimer.setInterval(CONF_PRESSURE_POLL_RATE, pressureLoop);
  pollTimerId[2] = mainPollTimer.setInterval(CONF_INPUT_POLL_RATE, inputLoop);

  
} //end setup


void loop() {

  led.run();
  mainPollTimer.run();
  mainStateTimer.run();
}

//*********************************//
// Memory Functions
//*********************************//

void initMemory(){
  mem.begin();
  //mem.format();
  mem.initialize(CONF_SETTINGS_FILE,CONF_SETTINGS_JSON);  
}

//*********************************//
// Input Functions
//*********************************//


void initInput(){
  
  ib.begin();
  is.begin();
  inputActionSize=sizeof(inputActionProperty)/sizeof(inputActionStruct);

}


//The loop handling inputs 
void inputLoop() {

  ib.update();
  is.update();              //Request new values 

  
  //Get the last state change 
  inputButtonActionState = ib.getInputState();
  inputSwitchActionState = is.getInputState();
  
  //printInputData();
  //Output action logic
  
  for (int i=0; i < inputActionSize; i++) {
    if(inputButtonActionState.mainState==inputActionProperty[i].inputActionState && 
      inputButtonActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputButtonActionState.elapsedTime >= inputActionProperty[i].inputActionStartTime &&
      inputButtonActionState.elapsedTime < inputActionProperty[i].inputActionEndTime){
      
      performOutputAction(inputActionProperty[i].inputActionNumber,
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
      
      performOutputAction(inputActionProperty[i].inputActionNumber,
      inputActionProperty[i].inputActionLedState,
      inputActionProperty[i].inputActionLedNumber,
      inputActionProperty[i].inputActionColorNumber);
      
      break;
    }
  }
  
}

void printInputData() {

  Serial.print(" main: "); Serial.print(inputButtonActionState.mainState);Serial.print(": "); Serial.print(inputSwitchActionState.mainState);Serial.print(", ");
  Serial.print(" secondary: "); Serial.print(inputButtonActionState.secondaryState);Serial.print(": "); Serial.print(inputSwitchActionState.secondaryState);Serial.print(", ");
  Serial.print(" time: "); Serial.print(inputButtonActionState.elapsedTime); Serial.print(": "); Serial.print(inputSwitchActionState.elapsedTime);Serial.print(", ");
  
  Serial.println();
 
}

//*********************************//
// Sip and Puff Functions
//*********************************//

void initSipAndPuff() {

  ps.begin(PRESS_TYPE_DIFF);
  setSipAndPuffThreshold();
  initSipAndPuffArray();
  sapActionSize=sizeof(sapActionProperty)/sizeof(sapActionStruct);
}

void setSipAndPuffThreshold(){
  sipThreshold = CONF_SIP_THRESHOLD;
  puffThreshold = CONF_PUFF_THRESHOLD;
}

void initSipAndPuffArray(){

  //Push initial state to state Queue
  
  //sapStateArray[0] = {CONF_SAP_MAIN_STATE_NONE, CONF_SAP_SEC_STATE_WAITING, 0};
  sapCurrState = sapPrevState = {CONF_SAP_MAIN_STATE_NONE, CONF_SAP_SEC_STATE_WAITING, 0};
  sapBuffer.pushElement(sapCurrState);

  //Reset and start the timer   
  stateTimerId[0] =  mainStateTimer.startTimer();
//  sapStateTimer[0].stop();                                      
//  sapStateTimer[0].reset();                                                                        
//  sapStateTimer[0].start(); 
}

void updatePressure() {
  ps.update();
}


void readPressure() {

  pressureStruct pressureValues = ps.getAllPressure();
  refPressure = pressureValues.refPressure;
  mainPressure = pressureValues.mainPressure;
  diffPressure = pressureValues.diffPressure;
  
 
}

//The loop handling pressure polling, sip and puff state evaluation 
void pressureLoop() {

  updatePressure();               //Request new pressure difference from sensor and push it to array

  readPressure();                 //Read the pressure object (can be last value from array, average or other algorithms)

  //printPressureData(); 

  sapPrevState = sapBuffer.getLastElement();  //Get the previous state
  
  //check for sip and puff conditions
  if (diffPressure > puffThreshold)  { 
    sapState = CONF_SAP_MAIN_STATE_PUFF;
  } else if (diffPressure < sipThreshold)  { 
    sapState = CONF_SAP_MAIN_STATE_SIP;
  } else {
    sapState = CONF_SAP_MAIN_STATE_NONE;
  }

  //None:None, Sip:Sip, Puff:Puff
  //Update time
  if(sapPrevState.mainState == sapState){
    //sapCurrState = {sapState, sapPrevState.secondaryState, sapStateTimer[0].elapsed()};
    sapCurrState = {sapState, sapPrevState.secondaryState, mainStateTimer.elapsedTime(stateTimerId[0])};
    //Serial.println("a");
    sapBuffer.updateLastElement(sapCurrState);
  } else {  //None:Sip , None:Puff , Sip:None, Puff:None
      //State: Sip or puff
      //Previous state: {none, waiting, time} Note: There can't be sip or puff and waiting 
      //New state: {Sip or puff, started, 0}
      if(sapPrevState.secondaryState==CONF_SAP_SEC_STATE_WAITING){
        sapCurrState = {sapState, CONF_SAP_SEC_STATE_STARTED, 0};
        //Serial.println("b");
      } 
      //State: none
      //Previous state: {Sip or puff, started, time} Note: There can't be none and started 
      //New state: {Sip or puff, released, time}      
      else if(sapPrevState.secondaryState==CONF_SAP_SEC_STATE_STARTED){
        sapCurrState = {sapPrevState.mainState, CONF_SAP_SEC_STATE_RELEASED, sapPrevState.elapsedTime};
        //Serial.println("c");
      }
      //State: None
      //Previous state: {Sip or puff, released, time}
      //New state: {none, waiting, 0}
      else if(sapPrevState.secondaryState==CONF_SAP_SEC_STATE_RELEASED && sapState==CONF_SAP_MAIN_STATE_NONE){
        sapCurrState = {sapState, CONF_SAP_SEC_STATE_WAITING, 0};
        //Serial.println("d");
      }
      //State: Sip or puff
      //Previous state: {none, released, time}
      //New state: {Sip or puff, started, 0}
      else if(sapPrevState.secondaryState==CONF_SAP_SEC_STATE_RELEASED && sapState!=CONF_SAP_MAIN_STATE_NONE){
        sapCurrState = {sapState, CONF_SAP_SEC_STATE_STARTED, 0};
        //Serial.println("e");
      }      
      //Push the new state   
      sapBuffer.pushElement(sapCurrState);
      //Reset and start the timer
//      sapStateTimer[0].stop();      
//      sapStateTimer[0].reset();                                                                        
//      sapStateTimer[0].start(); 
      mainStateTimer.restartTimer(stateTimerId[0]);  
  }

  //No action in 1 minute : reset timer
  //if(sapPrevState.secondaryState==CONF_SAP_SEC_STATE_WAITING && sapStateTimer[0].elapsed()>30000){
  if(sapPrevState.secondaryState==CONF_SAP_SEC_STATE_WAITING && mainStateTimer.elapsedTime(stateTimerId[0])>30000){
      ps.setZeroPressure();                                   //Update pressure compensation value 
      //Reset and start the timer    
//      sapStateTimer[0].stop();                                     
//      sapStateTimer[0].reset();                                                                        
//      sapStateTimer[0].start();  
      mainStateTimer.restartTimer(stateTimerId[0]);   
  }
  
  //Get the last state change 
  sapActionState = sapBuffer.getLastElement(); 

  //printSipAndPuffData();
  //Output action logic

  bool canOutputAction = true;

  //Logic to Skip Sip and puff action if it's in drag or scroll mode

  if((sapActionState.secondaryState == CONF_SAP_SEC_STATE_STARTED ||
      sapActionState.secondaryState == CONF_SAP_SEC_STATE_RELEASED) &&
      (outputAction == CONF_ACTION_SCROLL ||
      outputAction == CONF_ACTION_DRAG)){
      releaseOutputAction();
      outputAction=CONF_ACTION_NOTHING;
      canOutputAction=false;
      
  }
  if(sapActionState.elapsedTime==0){
    canOutputAction=false;
  }
  int sapActionIndex = 0;

  //Perform output action and led action on sip and puff release 
  //Perform led action on sip and puff start
  while (sapActionIndex < sapActionSize && canOutputAction){
    //Perform output action and led action on sip and puff release 
    if(sapActionState.mainState==sapActionProperty[sapActionIndex].sapActionState && 
      sapActionState.secondaryState == CONF_SAP_SEC_STATE_RELEASED &&
      sapActionState.elapsedTime >= sapActionProperty[sapActionIndex].sapActionStartTime &&
      sapActionState.elapsedTime < sapActionProperty[sapActionIndex].sapActionEndTime){
      
      performOutputAction(sapActionIndex,
      sapActionProperty[sapActionIndex].sapActionLedState.sapLedEndAction,
      sapActionProperty[sapActionIndex].sapActionLedState.sapLedEndNumber,
      sapActionProperty[sapActionIndex].sapActionLedState.sapLedEndColor);
      
      outputAction=sapActionIndex;      //used for releasing drag or scroll
      
      break;
    } //Perform led action on sip and puff start
    else if(sapActionState.mainState==sapActionProperty[sapActionIndex].sapActionState && 
      sapActionState.secondaryState == CONF_SAP_SEC_STATE_STARTED &&
      sapActionState.elapsedTime >= sapActionProperty[sapActionIndex].sapActionStartTime &&
      sapActionState.elapsedTime < sapActionProperty[sapActionIndex].sapActionEndTime){
        
      led.setLedColorById(sapActionProperty[sapActionIndex].sapActionLedState.sapLedStartNumber, 
      sapActionProperty[sapActionIndex].sapActionLedState.sapLedStartColor, 
      CONF_LED_BRIGHTNESS); 
      break;
    }
    sapActionIndex++;
  }


}

void printSipAndPuffData() {
  Serial.print(" main: "); Serial.print(sapActionState.mainState);Serial.print(", ");
  Serial.print(" secondary: "); Serial.print(sapActionState.secondaryState);Serial.print(", ");
  Serial.print(" time: "); Serial.print(sapActionState.elapsedTime);Serial.print(", ");
  
  Serial.println();
 
}


void printPressureData() {

  Serial.print(" refPressure: "); Serial.print(refPressure);Serial.print(", ");
  Serial.print(" mainPressure: "); Serial.print(mainPressure);Serial.print(", ");
  Serial.print(" diffPressure: "); Serial.print(diffPressure);Serial.print(", ");
  
  Serial.println();

}


void releaseOutputAction(){
  led.clearLedAll();
  if(outputAction==CONF_ACTION_DRAG && (mouse.isPressed(MOUSE_LEFT) || btmouse.isPressed(MOUSE_LEFT))){
    mouse.release(MOUSE_LEFT);
    btmouse.release(MOUSE_LEFT);
  }
}

void performOutputAction(int action, int ledAction, int ledNumber, int ledColor) {
    
    led.setLedColorById(ledNumber,ledColor,CONF_LED_BRIGHTNESS);                      //Set the initial pre-output action led color 
    switch (action) {
      case CONF_ACTION_NOTHING: {
        led.setLedActionById(ledNumber, 1, 50, ledColor,CONF_LED_BRIGHTNESS, ledAction);
        break;
      }
      case CONF_ACTION_LEFT_CLICK: {
        cursorLeftClick();
        led.setLedActionById(ledNumber, 1, 50, ledColor,CONF_LED_BRIGHTNESS, ledAction);
        break;
      }
      case CONF_ACTION_RIGHT_CLICK: {
        cursorRightClick();
        led.setLedActionById(ledNumber, 1, 50, ledColor,CONF_LED_BRIGHTNESS, ledAction);
        break;
      }
      case CONF_ACTION_DRAG: {
        cursorDrag();
        led.setLedActionById(ledNumber, 1, 50, ledColor,CONF_LED_BRIGHTNESS, ledAction);
        break;
      }
      case CONF_ACTION_SCROLL: {
        cursorScroll(); //Enter Scroll mode
        led.setLedActionById(ledNumber, 1, 50, ledColor,CONF_LED_BRIGHTNESS, ledAction);
        break;
      }
      case CONF_ACTION_CURSOR_CALIBRATION: {
        setJoystickCalibration();
        led.setLedActionById(ledNumber, 1, 50, ledColor,CONF_LED_BRIGHTNESS, ledAction);
        break;
      }
      case CONF_ACTION__MIDDLE_CLICK: {
        //Perform cursor middle click
        cursorMiddleClick();
        led.setLedActionById(ledNumber, 1, 50, ledColor,CONF_LED_BRIGHTNESS, ledAction);
        break;
      }
   }
}


void cursorLeftClick(void) {
  //Serial.println("Left Click");
  if(comMethod==1){
    mouse.click(MOUSE_LEFT);
  } 
  else if(comMethod==2){
    btmouse.click(MOUSE_LEFT);
  }
  //delay(80);
}

void cursorRightClick(void) {
  //Serial.println("Right Click");  
  if(comMethod==1){
    mouse.click(MOUSE_RIGHT);
  } 
  else if(comMethod==2){
    btmouse.click(MOUSE_RIGHT);
  }
  //delay(80);
}

void cursorMiddleClick(void) {
  //Serial.println("Middle Click");  
  if(comMethod==1){
    mouse.click(MOUSE_MIDDLE);
  } 
  else if(comMethod==2){
    btmouse.click(MOUSE_MIDDLE);
  }
  //delay(80);
}

void cursorDrag(void) {
  //Serial.println("Drag");  
  if(comMethod==1){
    mouse.press(MOUSE_LEFT);
  } 
  else if(comMethod==2){
    btmouse.press(MOUSE_LEFT);
  }
  //delay(80);
}

void cursorScroll(void) {
  //Serial.println("Scroll");  
}

void setJoystickCenter(void) {
  js.getInputComp();
  pointFloatType centerPoint = js.readInputComp();
  printJoystickFloatData(centerPoint);
}


//*********************************//
// Joystick Functions
//*********************************//

void initJoystick() {

  js.begin();
  setJoystickCenter();
  getJoystickCalibration();
}

void getJoystickCalibration() {
  String commandKey;
  pointFloatType maxPoint;
  for (int i=1; i < 5; i++) {
    commandKey="CA"+String(i);
    maxPoint=mem.readPoint(CONF_SETTINGS_FILE,commandKey);
    printJoystickFloatData(maxPoint);
    /*
    Serial.print(maxPoint.x);  
    Serial.print(",");  
    Serial.println(maxPoint.y); 
    */
    js.setInputMax(i,maxPoint);
  }
  //js.setMinimumRadius();
}

void setJoystickCalibration() {
  led.setLedBlinkById(4,2,500,LED_CLR_ORANGE,CONF_LED_BRIGHTNESS);
  String commandKey;
  pointFloatType maxPoint;
  delay(1000);
  
  for (int i=1; i < 5; i++) {
    commandKey="CA"+String(i);
    led.setLedBlinkById(2,6,500,LED_CLR_ORANGE,CONF_LED_BRIGHTNESS);
    led.setLedColorById(2, LED_CLR_ORANGE, CONF_LED_BRIGHTNESS_HIGH); 
    maxPoint=js.getInputMax(i);
    mem.writePoint(CONF_SETTINGS_FILE,commandKey,maxPoint);
    printJoystickFloatData(maxPoint);
    /*
    Serial.print(maxPoint.x);  
    Serial.print(",");  
    Serial.println(maxPoint.y); 
    */
    led.clearLed(2);    
    delay(1000);
  }
 //js.setMinimumRadius();
}

void updateJoystick() {
  js.update();
}


void readJoystick() {

  pointIntType joyOutPoint = js.getXYVal();
  xVal = joyOutPoint.x;
  yVal = joyOutPoint.y;
  //printJoystickIntData(joyOutPoint);
}

//The loop handling joystick 

void joystickLoop() {

    updateJoystick();               //Request new values
  
    readJoystick();                 //Read the filtered values 
    
    performJystick();

}

void performJystick(){
  
  if(comMethod==1){
    (outputAction==CONF_ACTION_SCROLL)? mouse.scroll(yVal) : mouse.move(xVal, -yVal);
  }
  else if(comMethod==2){
    (outputAction==CONF_ACTION_SCROLL)? btmouse.scroll(yVal) : btmouse.move(xVal, -yVal);
  } 


}

void printJoystickFloatData(pointFloatType point) {

  Serial.print(" x: "); Serial.print(point.x);Serial.print(", ");
  Serial.print(" y: "); Serial.print(point.y);Serial.print(", ");
  
  Serial.println();
}

void printJoystickIntData(pointIntType point) {

  Serial.print(" x: "); Serial.print(point.x);Serial.print(", ");
  Serial.print(" y: "); Serial.print(point.y);Serial.print(", ");
  
  Serial.println();
 
}

//*********************************//
// LED Functions
//*********************************//

void initLedFeedback(){
  led.setLedBlinkById(4,3,500,LED_CLR_GREEN,CONF_LED_BRIGHTNESS);

}



//*********************************//
// Timer Functions
//*********************************//

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
