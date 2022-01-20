
#include <Wire.h>  
#include <StopWatch.h>
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

uint8_t ledStateArray[3] = {LED_ACTION_OFF,LED_ACTION_OFF,LED_ACTION_OFF};
int ledStateArraySize;

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
    {OUTPUT_NOTHING,            2, LED_ACTION_BLINK,  2,LED_CLR_PURPLE,  0,1000}
};


int inputButtonPinArray[] = {INPUT_BUTTON1_PIN,INPUT_BUTTON2_PIN,INPUT_BUTTON3_PIN};
int inputSwitchPinArray[] = {INPUT_SWITCH1_PIN,INPUT_SWITCH2_PIN,INPUT_SWITCH3_PIN};

int sapState;
int outputAction;

float mainPressure; //read sensor
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
    {OUTPUT_NOTHING,            SAP_MAIN_STATE_NONE, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_OFF,0,LED_CLR_NONE},   0,0},
    {OUTPUT_LEFT_CLICK,         SAP_MAIN_STATE_PUFF , {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,1,LED_CLR_RED},    0,1000},
    {OUTPUT_RIGHT_CLICK,        SAP_MAIN_STATE_SIP, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,3,LED_CLR_BLUE},   0,1000},
    {OUTPUT_DRAG,               SAP_MAIN_STATE_PUFF , {LED_ACTION_ON,1,LED_CLR_ORANGE,LED_ACTION_ON,1,LED_CLR_YELLOW}, 1000,3000},
    {OUTPUT_SCROLL,             SAP_MAIN_STATE_SIP, {LED_ACTION_ON,3,LED_CLR_ORANGE,LED_ACTION_ON,3,LED_CLR_GREEN},  1000,3000},
    {OUTPUT_CURSOR_CALIBRATION, SAP_MAIN_STATE_PUFF, {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,2,LED_CLR_PURPLE},  3000,5000},
    {OUTPUT_MIDDLE_CLICK,       SAP_MAIN_STATE_SIP , {LED_ACTION_OFF,0,LED_CLR_NONE,LED_ACTION_BLINK,2,LED_CLR_ORANGE},  3000,5000}

};


sapStruct sapCurrState, sapPrevState, sapActionState;

LSCircularBuffer <sapStruct> sapBuffer(12);   //Create a buffer of type sapStruct

int xVal;
int yVal;

LSMemory mem;

LSInput ib(inputButtonPinArray,INPUT_BUTTON_NUMBER); 
//LSInput is(inputSwitchPinArray,INPUT_SWITCH_NUMBER);   //Starts an instance of the object

LSJoystick js;                  //Starts an instance of the LSJoystick object

LSPressure ps;                  //Starts an instance of the LSPressure object

LSOutput led;                   //Starts an instance of the LSOutput led object

LSUSBMouse mouse;               //Starts an instance of the usb mouse object
LSBLEMouse btmouse; 


StopWatch sapStateTimer[1];
StopWatch myTimer[1];

int joystickCounter =0;
unsigned long previousJoyMillis = 0;        
unsigned long previousPressureMillis = 0;
unsigned long previousInputMillis = 0;
unsigned long joyMillis = 20; 
unsigned long pressureMillis = 20; 
unsigned long inputMillis = 50; 

void setup() {

  mouse.begin();
  btmouse.begin();

  comMethod=COM_METHOD;
  
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

  //Scheduler.startLoop(inputLoop);
  
  //Scheduler.startLoop(pressureLoop);

  //Scheduler.startLoop(joystickLoop);
  
} //end setup


void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousJoyMillis >= joyMillis)
  {
    previousJoyMillis = currentMillis;  // Remember the time
    joystickLoop();
  }
  if(currentMillis - previousPressureMillis >= pressureMillis)
  {
    previousPressureMillis = currentMillis;  // Remember the time
    pressureLoop();
  }
  if(currentMillis - previousInputMillis >= inputMillis)
  {
    previousInputMillis = currentMillis;  // Remember the time
    inputLoop();
  }  
  //inputLoop();
  //pressureLoop();
  //printInputData();
  //printJoystickData();
}

//*********************************//
// Memory Functions
//*********************************//

void initMemory(){
  mem.begin();
  mem.format();
  mem.initialize(SETTINGS_FILE,SETTINGS_JSON);  
}

//*********************************//
// Input Functions
//*********************************//


void initInput(){
  
  ib.begin();
  //is.begin();
  inputActionSize=sizeof(inputActionProperty)/sizeof(inputActionStruct);

}


//The loop handling inputs 
void inputLoop() {

  ib.update();
  //is.update();              //Request new values 

  
  //Get the last state change 
  inputButtonActionState = ib.getInputState();
  //inputSwitchActionState = is.getInputState();
  
  //printInputData();
  //Output action logic
  
  for (int i=0; i < inputActionSize; i++) {
    if(inputButtonActionState.mainState==inputActionProperty[i].inputActionState && 
      inputButtonActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputButtonActionState.elapsedTime >= inputActionProperty[i].inputActionStartTime &&
      inputButtonActionState.elapsedTime < inputActionProperty[i].inputActionEndTime){
      
      performAction(inputActionProperty[i].inputActionNumber,
      inputActionProperty[i].inputActionLedState,
      inputActionProperty[i].inputActionLedNumber,
      inputActionProperty[i].inputActionColorNumber);
        Serial.println(inputActionProperty[i].inputActionNumber);

      break;
    }
  }
  /*
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
  */
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
  //resetTimer();
  updatePressure();               //Request new pressure difference from sensor and push it to array

  readPressure();                 //Read the pressure object (can be last value from array, average or other algorithms)

  //printPressureData(); 

  sapPrevState = sapBuffer.getLastElement();  //Get the previous state
  
  //check for sip and puff conditions
  if (diffPressure > puffThreshold)  { 
    sapState = SAP_MAIN_STATE_PUFF;
  } else if (diffPressure < sipThreshold)  { 
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

  bool canPerformAction = true;

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

  //delay(20);
  //Serial.println(getTime());  

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


void releaseHoldAction(){
  for (int i=0; i < ledStateArraySize; i++) {
     led.clearLed(i+1);
     ledStateArray[i]=LED_ACTION_OFF;
  }
  if(outputAction==OUTPUT_DRAG && (mouse.isPressed(MOUSE_LEFT) || btmouse.isPressed(MOUSE_LEFT))){
    mouse.release(MOUSE_LEFT);
    btmouse.release(MOUSE_LEFT);
  }
  //delay(200);
}

void performAction(int action, int ledState, int ledNumber, int ledColor) {
  
    switch (action) {
      case OUTPUT_NOTHING: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        setLedState(ledState,ledNumber);
        //do nothing
        break;
      }
      case OUTPUT_LEFT_CLICK: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        cursorLeftClick();
        setLedState(ledState,ledNumber);
        //delay(5);
        break;
      }
      case OUTPUT_RIGHT_CLICK: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        cursorRightClick();
        setLedState(ledState,ledNumber);
        //delay(5);
        break;
      }
      case OUTPUT_DRAG: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        cursorDrag();
        setLedState(ledState,ledNumber);
        //delay(5);
        break;
      }
      case OUTPUT_SCROLL: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        cursorScroll(); //Enter Scroll mode
        setLedState(ledState,ledNumber);
        //delay(5);
        break;
      }
      case OUTPUT_CURSOR_CALIBRATION: {
        //led.setLedBlinkById(ledNumber,2,500,ledColor,LED_BRIGHTNESS);
        //led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        setJoystickCalibration();
        setLedState(ledState,ledNumber);
        //delay(5);
        break;
      }
      case OUTPUT_MIDDLE_CLICK: {
        led.setLedColorById(ledNumber,ledColor,LED_BRIGHTNESS);
        //Perform cursor middle click
        cursorMiddleClick();
        setLedState(ledState,ledNumber);
        //delay(5);
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
  //Serial.println("Center Joystick");  
  js.setInputComp();
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
    maxPoint=mem.readPoint(SETTINGS_FILE,commandKey);
    
    Serial.print(maxPoint.x);  
    Serial.print(",");  
    Serial.println(maxPoint.y); 
    
    js.setInputMax(i,maxPoint);
  }
}

void setJoystickCalibration() {
  led.setLedBlinkById(4,2,500,LED_CLR_ORANGE,LED_BRIGHTNESS);
  String commandKey;
  pointFloatType maxPoint;
  delay(1000);
  
  for (int i=1; i < 5; i++) {
    commandKey="CA"+String(i);
    led.setLedBlinkById(2,6,500,LED_CLR_ORANGE,LED_BRIGHTNESS);
    led.setLedColorById(2, LED_CLR_ORANGE, LED_BRIGHTNESS_HIGH); 
    maxPoint=js.getInputMax(i);
    mem.writePoint(SETTINGS_FILE,commandKey,maxPoint);
    Serial.print(maxPoint.x);  
    Serial.print(",");  
    Serial.println(maxPoint.y); 
    led.clearLed(2);    
    delay(1000);
  }
  /*
  js.setInputMax(1);
  js.setInputMax(2);
  js.setInputMax(3);
  js.setInputMax(4);
  */
}

void updateJoystick() {
  js.update();
}


void readJoystick() {

  pointIntType joystickValues = js.getAllVal();
  xVal = joystickValues.x;
  yVal = joystickValues.y;
 
}

//The loop handling joystick 

void joystickLoop() {

    updateJoystick();               //Request new values
  
    readJoystick();                 //Read the filtered values 
  
    //printJoystickData();
    
    performJystick();


    //delay(20);  
}

void performJystick(){
  
  if(comMethod==1){
    (outputAction==OUTPUT_SCROLL)? mouse.scroll(yVal) : mouse.move(xVal, -yVal);
  }
  else if(comMethod==2){
    (outputAction==OUTPUT_SCROLL)? btmouse.scroll(yVal) : btmouse.move(xVal, -yVal);
  } 


}

void printJoystickData() {

  Serial.print(" xRaw: "); Serial.print(xVal);Serial.print(", ");
  Serial.print(" yRaw: "); Serial.print(yVal);Serial.print(", ");
  
  Serial.println();
 
}

//*********************************//
// LED Functions
//*********************************//

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
