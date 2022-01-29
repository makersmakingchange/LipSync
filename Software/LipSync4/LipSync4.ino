#include <Wire.h>  
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
  unsigned long inputActionStartTime;
  unsigned long inputActionEndTime;
} inputActionStruct;

int inputActionSize;
inputStruct inputButtonActionState, inputSwitchActionState;

const inputActionStruct switchActionProperty[] {
    {CONF_ACTION_NOTHING,            0, 0,0},
    {CONF_ACTION_LEFT_CLICK,         1, 0,1000},
    {CONF_ACTION_RIGHT_CLICK,        4, 0,1000},
    {CONF_ACTION_DRAG,               1, 1000,3000},
    {CONF_ACTION_SCROLL,             4, 1000,3000},
    {CONF_ACTION_CURSOR_CALIBRATION, 1, 3000,10000},
    {CONF_ACTION_MIDDLE_CLICK,       4, 3000,10000}
};


int inputButtonPinArray[] = {CONF_BUTTON1_PIN,CONF_BUTTON2_PIN,CONF_BUTTON3_PIN};
int inputSwitchPinArray[] = {CONF_SWITCH1_PIN,CONF_SWITCH2_PIN,CONF_SWITCH3_PIN};


//Pressure module variables and structures 
int sapState;
int outputAction;
bool canOutputAction;

float mainPressure; 
float refPressure;
float diffPressure;

pressureStruct pressureValues = {0.0,0.0,0.0};

float sipThreshold;
float puffThreshold;

typedef struct {
  int mainState;            //none = 0, sip = 1, puff = 2
  int secondaryState;       //waiting = 0, started = 1, detected = 2
  unsigned long elapsedTime;     //in ms
} sapStruct;

typedef struct { 
  uint8_t ledOutputActionNumber;
  uint8_t ledNumber;
  uint8_t ledStartColor;
  uint8_t ledEndColor;
  uint8_t ledEndAction;
} ledActionStruct;


typedef struct { 
  uint8_t sapOutputActionNumber;
  uint8_t sapActionState;
  unsigned long sapActionStartTime;
  unsigned long sapActionEndTime;
} sapActionStruct;

int sapActionSize;

const sapActionStruct sapActionProperty[] {
    {CONF_ACTION_NOTHING,            CONF_SAP_MAIN_STATE_NONE,  0,0},
    {CONF_ACTION_LEFT_CLICK,         CONF_SAP_MAIN_STATE_PUFF,  0,1000},
    {CONF_ACTION_RIGHT_CLICK,        CONF_SAP_MAIN_STATE_SIP,   0,1000},
    {CONF_ACTION_DRAG,               CONF_SAP_MAIN_STATE_PUFF,  1000,3000},
    {CONF_ACTION_SCROLL,             CONF_SAP_MAIN_STATE_SIP,   1000,3000},
    {CONF_ACTION_CURSOR_CALIBRATION, CONF_SAP_MAIN_STATE_PUFF,  3000,5000},
    {CONF_ACTION_MIDDLE_CLICK,       CONF_SAP_MAIN_STATE_SIP ,  3000,5000}

};

const ledActionStruct ledActionProperty[] {
    {CONF_ACTION_NOTHING,            0,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_OFF},
    {CONF_ACTION_LEFT_CLICK,         1,LED_CLR_NONE,  LED_CLR_RED,    LED_ACTION_BLINK},
    {CONF_ACTION_RIGHT_CLICK,        3,LED_CLR_NONE,  LED_CLR_BLUE,   LED_ACTION_BLINK},
    {CONF_ACTION_DRAG,               1,LED_CLR_PURPLE,LED_CLR_RED,    LED_ACTION_ON},
    {CONF_ACTION_SCROLL,             3,LED_CLR_PURPLE,LED_CLR_BLUE,   LED_ACTION_ON},
    {CONF_ACTION_CURSOR_CALIBRATION, 2,LED_CLR_NONE,  LED_CLR_ORANGE, LED_ACTION_BLINK},
    {CONF_ACTION_MIDDLE_CLICK,       2,LED_CLR_NONE,  LED_CLR_PURPLE, LED_ACTION_BLINK}

};

typedef struct { 
  uint8_t ledAction;            //off = 0, on = 1, blink = 2
  uint8_t ledColorNumber;
  uint8_t ledNumber;
  uint8_t ledBlinkNumber;
  unsigned long ledBlinkTime;
  uint8_t ledBrightness;
} ledStateStruct; 

ledStateStruct ledCurrentState; 

sapStruct sapCurrState, sapPrevState, sapActionState;

LSCircularBuffer <sapStruct> sapBuffer(12);   //Create a buffer of type sapStruct

//Joystick module variables and structures 

int xVal;
int yVal;

//Timer related variables 

int pollTimerId[3];
int sapStateTimerId[1];

int ledBlinkTimerId[2];
LSTimer ledStateTimer;

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


  pollTimerId[0] = mainPollTimer.setInterval(CONF_JOYSTICK_POLL_RATE,0, joystickLoop);
  pollTimerId[1] = mainPollTimer.setInterval(CONF_PRESSURE_POLL_RATE,0, pressureLoop);
  pollTimerId[2] = mainPollTimer.setInterval(CONF_INPUT_POLL_RATE,0, inputLoop);


  
} //end setup


void loop() {

  ledStateTimer.run();
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
  inputActionSize=sizeof(switchActionProperty)/sizeof(inputActionStruct);

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
  int tempActionIndex = 0;
  
  for (int i=0; i < inputActionSize; i++) {
    if(inputButtonActionState.mainState==switchActionProperty[i].inputActionState && 
      inputButtonActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputButtonActionState.elapsedTime >= switchActionProperty[i].inputActionStartTime &&
      inputButtonActionState.elapsedTime < switchActionProperty[i].inputActionEndTime){
      
      tempActionIndex=sapActionProperty[i].sapOutputActionNumber;  
      
      performOutputAction(tempActionIndex,
      ledActionProperty[tempActionIndex].ledNumber,
      ledActionProperty[tempActionIndex].ledEndColor);
       
      break;
    }
  }
  
  for (int i=0; i < inputActionSize; i++) {
    if(inputSwitchActionState.mainState==switchActionProperty[i].inputActionState && 
      inputSwitchActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputSwitchActionState.elapsedTime >= switchActionProperty[i].inputActionStartTime &&
      inputSwitchActionState.elapsedTime < switchActionProperty[i].inputActionEndTime){

      tempActionIndex=sapActionProperty[i].sapOutputActionNumber;  
      
      performOutputAction(tempActionIndex,
      ledActionProperty[tempActionIndex].ledNumber,
      ledActionProperty[tempActionIndex].ledEndColor);
      
      break;
    }
  }
  
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
  
  sapCurrState = sapPrevState = {CONF_SAP_MAIN_STATE_NONE, CONF_SAP_SEC_STATE_WAITING, 0};
  sapBuffer.pushElement(sapCurrState);

  //Reset and start the timer   
  sapStateTimerId[0] =  mainStateTimer.startTimer();
}

void updatePressure() {
  ps.update();
}


void readPressure() {

  pressureValues = ps.getAllPressure();
 
}

//The loop handling pressure polling, sip and puff state evaluation 
void pressureLoop() {

  updatePressure();               //Request new pressure difference from sensor and push it to array

  readPressure();                 //Read the pressure object (can be last value from array, average or other algorithms)

  sapPrevState = sapBuffer.getLastElement();  //Get the previous state
  
  //check for sip and puff conditions
  if (pressureValues.diffPressure > puffThreshold)  { 
    sapState = CONF_SAP_MAIN_STATE_PUFF;
  } else if (pressureValues.diffPressure < sipThreshold)  { 
    sapState = CONF_SAP_MAIN_STATE_SIP;
  } else {
    sapState = CONF_SAP_MAIN_STATE_NONE;
  }

  //None:None, Sip:Sip, Puff:Puff
  //Update time
  if(sapPrevState.mainState == sapState){
    sapCurrState = {sapState, sapPrevState.secondaryState, mainStateTimer.elapsedTime(sapStateTimerId[0])};
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
      mainStateTimer.restartTimer(sapStateTimerId[0]);  
  }

  //No action in 1 minute : reset timer
  if(sapPrevState.secondaryState==CONF_SAP_SEC_STATE_WAITING && mainStateTimer.elapsedTime(sapStateTimerId[0])>CONF_ACTION_TIMEOUT){
      ps.setZeroPressure();                                   //Update pressure compensation value 
      //Reset and start the timer    
      mainStateTimer.restartTimer(sapStateTimerId[0]);   
  }
  
  //Get the last state change 
  sapActionState = sapBuffer.getLastElement(); 

  printSipAndPuffData(2);
  //Output action logic

  canOutputAction = true;

  //Logic to Skip Sip and puff action if it's in drag or scroll mode

  if((
      sapActionState.secondaryState == CONF_SAP_SEC_STATE_RELEASED) &&
      (outputAction == CONF_ACTION_SCROLL ||
      outputAction == CONF_ACTION_DRAG)){
      releaseOutputAction();
  }
  if(sapActionState.elapsedTime==0){
    canOutputAction=false;
  }
  int sapActionIndex = 0;
  int tempActionIndex = 0;
  //Perform output action and led action on sip and puff release 
  //Perform led action on sip and puff start
  while (sapActionIndex < sapActionSize && canOutputAction){
    //Perform output action and led action on sip and puff release 
    if(sapActionState.mainState==sapActionProperty[sapActionIndex].sapActionState && 
      sapActionState.secondaryState == CONF_SAP_SEC_STATE_RELEASED &&
      sapActionState.elapsedTime >= sapActionProperty[sapActionIndex].sapActionStartTime &&
      sapActionState.elapsedTime < sapActionProperty[sapActionIndex].sapActionEndTime){

      tempActionIndex=sapActionProperty[sapActionIndex].sapOutputActionNumber;      //used for releasing drag or scroll

      
      performOutputAction(tempActionIndex,
      ledActionProperty[tempActionIndex].ledNumber,
      ledActionProperty[tempActionIndex].ledEndColor);

      outputAction=tempActionIndex;
            
      break;
    } //Perform led action on sip and puff start
    else if(sapActionState.mainState==sapActionProperty[sapActionIndex].sapActionState && 
      sapActionState.secondaryState == CONF_SAP_SEC_STATE_STARTED &&
      sapActionState.elapsedTime >= sapActionProperty[sapActionIndex].sapActionStartTime &&
      sapActionState.elapsedTime < sapActionProperty[sapActionIndex].sapActionEndTime){

      tempActionIndex=sapActionProperty[sapActionIndex].sapOutputActionNumber;      //used for releasing drag or scroll
        
      led.setLedColorById(ledActionProperty[tempActionIndex].ledNumber, 
      ledActionProperty[tempActionIndex].ledStartColor, 
      CONF_LED_BRIGHTNESS); 
      break;
    }
    sapActionIndex++;
  }


}


void releaseOutputAction(){
  led.clearLedAll();
  if(outputAction==CONF_ACTION_DRAG && (mouse.isPressed(MOUSE_LEFT) || btmouse.isPressed(MOUSE_LEFT))){
    mouse.release(MOUSE_LEFT);
    btmouse.release(MOUSE_LEFT);
  }
  outputAction=CONF_ACTION_NOTHING;
  canOutputAction=false;
}

void performOutputAction(int action, int ledNumber, int ledColor) {
    
    led.setLedColorById(ledNumber,ledColor,CONF_LED_BRIGHTNESS);                      //Set the initial pre-output action led color 
    switch (action) {
      case CONF_ACTION_NOTHING: {
        releaseOutputAction();
        break;
      }
      case CONF_ACTION_LEFT_CLICK: {
        cursorLeftClick();
        releaseOutputAction();
        break;
      }
      case CONF_ACTION_RIGHT_CLICK: {
        cursorRightClick();
        releaseOutputAction();
        break;
      }
      case CONF_ACTION_DRAG: {
        cursorDrag();
        break;
      }
      case CONF_ACTION_SCROLL: {
        cursorScroll(); //Enter Scroll mode
        break;
      }
      case CONF_ACTION_CURSOR_CALIBRATION: {
        //setJoystickCalibration();
        break;
      }
      case CONF_ACTION_MIDDLE_CLICK: {
        //Perform cursor middle click
        cursorMiddleClick();
        //releaseOutputAction();
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
    js.setInputMax(i,maxPoint);
  }
}

void setJoystickCalibration() {
    for (int j = 1; j <= 7; j++) {
      led.setLedColor(4,j,CONF_LED_BRIGHTNESS);
      delay(500);
    }
    led.setLedColor(4,0,CONF_LED_BRIGHTNESS);
   String commandKey;
  pointFloatType maxPoint;
  delay(1000);
  
  for (int i=1; i < 5; i++) {
    commandKey="CA"+String(i);
    led.setLedColorById(2, LED_CLR_ORANGE, CONF_LED_BRIGHTNESS_HIGH); 
    maxPoint=js.getInputMax(i);
    mem.writePoint(CONF_SETTINGS_FILE,commandKey,maxPoint);
    printJoystickFloatData(maxPoint);
    led.clearLed(2);    
    delay(1000);
  }
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
    (outputAction==CONF_ACTION_SCROLL)? mouse.scroll(yVal/10) : mouse.move(xVal, -yVal);
  }
  else if(comMethod==2){
    (outputAction==CONF_ACTION_SCROLL)? btmouse.scroll(yVal/10) : btmouse.move(xVal, -yVal);
  } 

}


//*********************************//
// Print Functions
//*********************************//

void printInputData() {

  Serial.print(" main: "); Serial.print(inputButtonActionState.mainState);Serial.print(": "); Serial.print(inputSwitchActionState.mainState);Serial.print(", ");
  Serial.print(" secondary: "); Serial.print(inputButtonActionState.secondaryState);Serial.print(": "); Serial.print(inputSwitchActionState.secondaryState);Serial.print(", ");
  Serial.print(" time: "); Serial.print(inputButtonActionState.elapsedTime); Serial.print(": "); Serial.print(inputSwitchActionState.elapsedTime);Serial.print(", ");
  
  Serial.println();
 
}


void printSipAndPuffData(int type) {

  if(type==1){
    Serial.print(" refPressure: "); Serial.print(pressureValues.refPressure);Serial.print(", ");
    Serial.print(" mainPressure: "); Serial.print(pressureValues.mainPressure);Serial.print(", ");
    Serial.print(" diffPressure: "); Serial.print(pressureValues.diffPressure);
  } else if(type==2){
    Serial.print(" main: "); Serial.print(sapActionState.mainState);Serial.print(", ");
    Serial.print(" secondary: "); Serial.print(sapActionState.secondaryState);Serial.print(", ");
    Serial.print(" time: "); Serial.print(sapActionState.elapsedTime);
    Serial.println();
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
    for (int j = 1; j <= 7; j++) {
      led.setLedColor(4,j,CONF_LED_BRIGHTNESS);
      delay(500);
    }
    led.setLedColor(4,0,CONF_LED_BRIGHTNESS);
    setLedState(1,3,4,2,600);
    performLedBlink();
}


void setLedState(int ledAction, int ledColorNumber, int ledNumber,  int ledBlinkNumber, unsigned long ledBlinkTime){ //Set led state after output action is performed 
  if(ledNumber<=OUTPUT_RGB_LED_NUM+1){
    ledCurrentState.ledAction=ledAction;
    ledCurrentState.ledColorNumber=ledColorNumber;
    ledCurrentState.ledNumber=ledNumber;
    ledCurrentState.ledBlinkNumber=ledBlinkNumber;
    ledCurrentState.ledBlinkTime=ledBlinkTime;
    ledCurrentState.ledBrightness=CONF_LED_BRIGHTNESS;
  }

}

void performLed(){
  led.setLedColor(ledCurrentState.ledNumber, ledCurrentState.ledColorNumber, ledCurrentState.ledBrightness);
    
}

void clearLed(){
    led.clearLed(ledCurrentState.ledNumber);
  
}

//***SET RGB LED BLINK BY ID FUNCTION***//

void performLedBlink() {

  ledBlinkTimerId[0] = ledStateTimer.setTimer(ledCurrentState.ledBlinkTime, ledCurrentState.ledBlinkTime,performLed,ledCurrentState.ledBlinkNumber);   
  ledBlinkTimerId[1] = ledStateTimer.setTimer(ledCurrentState.ledBlinkTime*2, ledCurrentState.ledBlinkTime*2,clearLed,ledCurrentState.ledBlinkNumber+1);   

}

void performLedAction(){
    switch (ledCurrentState.ledAction) {
      case LED_ACTION_OFF: {
        clearLed();
        break;
      }
      case LED_ACTION_ON: {
        performLed();
        break;
      }
      case LED_ACTION_BLINK: {
        performLedBlink();
        break;
      }
    }
}
