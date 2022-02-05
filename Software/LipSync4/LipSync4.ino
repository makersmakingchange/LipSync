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

int comMode = 0;

//LED module variables and structures 
typedef struct { 
  uint8_t ledOutputActionNumber;
  uint8_t ledNumber;
  uint8_t ledStartColor;
  uint8_t ledEndColor;
  uint8_t ledEndAction;
} ledActionStruct;

const ledActionStruct ledActionProperty[] {
    {CONF_ACTION_NOTHING,            0,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_NONE},
    {CONF_ACTION_LEFT_CLICK,         1,LED_CLR_NONE,  LED_CLR_RED,    LED_ACTION_BLINK},
    {CONF_ACTION_RIGHT_CLICK,        3,LED_CLR_NONE,  LED_CLR_BLUE,   LED_ACTION_BLINK},
    {CONF_ACTION_DRAG,               1,LED_CLR_PURPLE,LED_CLR_RED,    LED_ACTION_ON},
    {CONF_ACTION_SCROLL,             3,LED_CLR_PURPLE,LED_CLR_BLUE,   LED_ACTION_ON},
    {CONF_ACTION_CURSOR_CALIBRATION, 4,LED_CLR_NONE,  LED_CLR_ORANGE, LED_ACTION_BLINK},
    {CONF_ACTION_CURSOR_CENTER,      2,LED_CLR_NONE,  LED_CLR_ORANGE, LED_ACTION_BLINK},
    {CONF_ACTION_MIDDLE_CLICK,       2,LED_CLR_NONE,  LED_CLR_PURPLE, LED_ACTION_BLINK},
    {CONF_ACTION_DEC_SPEED,          1,LED_CLR_NONE,  LED_CLR_RED,    LED_ACTION_BLINK},
    {CONF_ACTION_INC_SPEED,          3,LED_CLR_NONE,  LED_CLR_BLUE,   LED_ACTION_BLINK},
    {CONF_ACTION_CHANGE_MODE,        2,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_NONE}

};

typedef struct { 
  uint8_t ledAction;            //none = 0, off = 1, on = 2, blink = 3
  uint8_t ledColorNumber;
  uint8_t ledNumber;
  uint8_t ledBlinkNumber;
  unsigned long ledBlinkTime;
  uint8_t ledBrightness;
} ledStateStruct; 

ledStateStruct* ledCurrentState; 

int ledTimerId[9];

LSTimer <void> ledStateTimer;

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
    {CONF_ACTION_NOTHING,            INPUT_MAIN_STATE_NONE,       0,0},
    {CONF_ACTION_LEFT_CLICK,         INPUT_MAIN_STATE_S1_PRESSED, 0,1000},
    {CONF_ACTION_MIDDLE_CLICK,       INPUT_MAIN_STATE_S2_PRESSED, 0,1000},
    {CONF_ACTION_RIGHT_CLICK,        INPUT_MAIN_STATE_S3_PRESSED, 0,1000},
    {CONF_ACTION_DRAG,               INPUT_MAIN_STATE_S1_PRESSED, 1000,3000},
    {CONF_ACTION_CHANGE_MODE,        INPUT_MAIN_STATE_S2_PRESSED, 1000,3000},
    {CONF_ACTION_SCROLL,             INPUT_MAIN_STATE_S3_PRESSED, 1000,3000},
    {CONF_ACTION_CURSOR_CALIBRATION, INPUT_MAIN_STATE_S1_PRESSED, 3000,10000},
    {CONF_ACTION_NOTHING,            INPUT_MAIN_STATE_S2_PRESSED, 3000,10000},
    {CONF_ACTION_MIDDLE_CLICK,       INPUT_MAIN_STATE_S3_PRESSED, 3000,10000},
};

const inputActionStruct buttonActionProperty[] {
    {CONF_ACTION_NOTHING,            INPUT_MAIN_STATE_NONE,        0,0},
    {CONF_ACTION_DEC_SPEED,          INPUT_MAIN_STATE_S1_PRESSED,  0,1000},
    {CONF_ACTION_INC_SPEED,          INPUT_MAIN_STATE_S3_PRESSED,  0,1000},
    {CONF_ACTION_CURSOR_CENTER,      INPUT_MAIN_STATE_S2_PRESSED,  0,1000},
    {CONF_ACTION_CHANGE_MODE,        INPUT_MAIN_STATE_S2_PRESSED,  1000,3000},
    {CONF_ACTION_CURSOR_CALIBRATION, INPUT_MAIN_STATE_S13_PRESSED, 0,1000}
};


int inputButtonPinArray[] = {CONF_BUTTON1_PIN,CONF_BUTTON2_PIN,CONF_BUTTON3_PIN};
int inputSwitchPinArray[] = {CONF_SWITCH1_PIN,CONF_SWITCH2_PIN,CONF_SWITCH3_PIN};


//Pressure module variables and structures 

pressureStruct pressureValues = {0.0,0.0,0.0};

sapStruct sapActionState;

int sapActionSize;

const inputActionStruct sapActionProperty[] {
    {CONF_ACTION_NOTHING,            PRESS_SAP_MAIN_STATE_NONE,  0,0},
    {CONF_ACTION_LEFT_CLICK,         PRESS_SAP_MAIN_STATE_PUFF,  0,1000},
    {CONF_ACTION_RIGHT_CLICK,        PRESS_SAP_MAIN_STATE_SIP,   0,1000},
    {CONF_ACTION_DRAG,               PRESS_SAP_MAIN_STATE_PUFF,  1000,3000},
    {CONF_ACTION_SCROLL,             PRESS_SAP_MAIN_STATE_SIP,   1000,3000},
    {CONF_ACTION_CURSOR_CALIBRATION, PRESS_SAP_MAIN_STATE_PUFF,  3000,5000},
    {CONF_ACTION_MIDDLE_CLICK,       PRESS_SAP_MAIN_STATE_SIP ,  3000,5000}

};

//Joystick module variables and structures 

int xVal;
int yVal;

//Timer related variables 

int pollTimerId[3];

LSTimer <void> pollTimer;

//General

int outputAction;
bool canOutputAction;

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

  comMode=CONF_COM_MODE;
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  //while (!Serial) { delay(1); }
  
  delay(1000);

  initMemory();

  initSipAndPuff();

  led.begin();    

  initInput();

  initJoystick(); 
  
  initLedFeedback();


  pollTimerId[0] = pollTimer.setInterval(CONF_JOYSTICK_POLL_RATE,0, joystickLoop);
  pollTimerId[1] = pollTimer.setInterval(CONF_PRESSURE_POLL_RATE,0, pressureLoop);
  pollTimerId[2] = pollTimer.setInterval(CONF_INPUT_POLL_RATE,0, inputLoop);


  
} //end setup


void loop() {

  ledStateTimer.run();
  pollTimer.run();
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

  canOutputAction = true;
  //Output action logic
  int tempActionIndex = 0;
  
  for (int i=0; i < inputActionSize; i++) {
    if(inputButtonActionState.mainState==buttonActionProperty[i].inputActionState && 
      inputButtonActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputButtonActionState.elapsedTime >= buttonActionProperty[i].inputActionStartTime &&
      inputButtonActionState.elapsedTime < buttonActionProperty[i].inputActionEndTime){
      
      tempActionIndex=buttonActionProperty[i].inputActionNumber;  
      
      performOutputAction(tempActionIndex,
      ledActionProperty[tempActionIndex].ledEndAction,
      ledActionProperty[tempActionIndex].ledNumber,
      ledActionProperty[tempActionIndex].ledEndColor);
       
      break;
    }
  }

  if((
      inputSwitchActionState.secondaryState == INPUT_SEC_STATE_RELEASED) &&
      (outputAction == CONF_ACTION_SCROLL ||
      outputAction == CONF_ACTION_DRAG)){
      releaseOutputAction();
      canOutputAction=false;
  }
  
  for (int i=0; i < inputActionSize && canOutputAction; i++) {
    if(inputSwitchActionState.mainState==switchActionProperty[i].inputActionState && 
      inputSwitchActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputSwitchActionState.elapsedTime >= switchActionProperty[i].inputActionStartTime &&
      inputSwitchActionState.elapsedTime < switchActionProperty[i].inputActionEndTime){

      tempActionIndex=switchActionProperty[i].inputActionNumber;  
      
      performOutputAction(tempActionIndex,
      ledActionProperty[tempActionIndex].ledEndAction,
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
  sapActionSize=sizeof(sapActionProperty)/sizeof(inputActionStruct);
}

void setSipAndPuffThreshold(){
  ps.setStateThreshold(CONF_SIP_THRESHOLD,CONF_PUFF_THRESHOLD);
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

  //Get the last state change 
  sapActionState = ps.getState();

  //printSipAndPuffData(2);
  //Output action logic

  canOutputAction = true;
  //Logic to Skip Sip and puff action if it's in drag or scroll mode
  if((
      sapActionState.secondaryState == PRESS_SAP_SEC_STATE_RELEASED) &&
      (outputAction == CONF_ACTION_SCROLL ||
      outputAction == CONF_ACTION_DRAG)){
      releaseOutputAction();
      canOutputAction=false;
  }

  int sapActionIndex = 0;
  int tempActionIndex = 0;
  //Perform output action and led action on sip and puff release 
  //Perform led action on sip and puff start
  while (sapActionIndex < sapActionSize && canOutputAction){
    //Perform output action and led action on sip and puff release 
    if(sapActionState.mainState==sapActionProperty[sapActionIndex].inputActionState && 
      sapActionState.secondaryState == PRESS_SAP_SEC_STATE_RELEASED &&
      sapActionState.elapsedTime >= sapActionProperty[sapActionIndex].inputActionStartTime &&
      sapActionState.elapsedTime < sapActionProperty[sapActionIndex].inputActionEndTime){

      tempActionIndex=sapActionProperty[sapActionIndex].inputActionNumber;      //used for releasing drag or scroll
   

      setLedState(ledActionProperty[tempActionIndex].ledEndAction, ledActionProperty[tempActionIndex].ledEndColor, ledActionProperty[tempActionIndex].ledNumber,  1, 200);

      outputAction=tempActionIndex;  
      
      performOutputAction(tempActionIndex,
      ledActionProperty[tempActionIndex].ledEndAction,
      ledActionProperty[tempActionIndex].ledNumber,
      ledActionProperty[tempActionIndex].ledEndColor);
      
    
      break;
    } //Perform led action on sip and puff start
    else if(sapActionState.mainState==sapActionProperty[sapActionIndex].inputActionState && 
      sapActionState.secondaryState == PRESS_SAP_SEC_STATE_STARTED &&
      sapActionState.elapsedTime >= sapActionProperty[sapActionIndex].inputActionStartTime &&
      sapActionState.elapsedTime < sapActionProperty[sapActionIndex].inputActionEndTime){

      tempActionIndex=sapActionProperty[sapActionIndex].inputActionNumber;      //used for releasing drag or scroll
        
      led.setLedColor(ledActionProperty[tempActionIndex].ledNumber, 
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
}

void performOutputAction(int action, int ledAction, int ledNumber, int ledColor) {
    if(ledAction==LED_ACTION_OFF){
      led.clearLedAll();
    }
    else if(ledAction==LED_ACTION_ON || ledAction==LED_ACTION_BLINK ){
      led.setLedColor(ledNumber,ledColor,CONF_LED_BRIGHTNESS); 
    }
    switch (action) {
      case CONF_ACTION_NOTHING: {
        break;
      }
      case CONF_ACTION_LEFT_CLICK: {
        cursorLeftClick();
        break;
      }
      case CONF_ACTION_RIGHT_CLICK: {
        cursorRightClick();
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
        setJoystickCalibration();
        break;
      }
      case CONF_ACTION_CURSOR_CENTER: {
        //Perform cursor center
        setJoystickCenter();
        break;
      }
      case CONF_ACTION_MIDDLE_CLICK: {
        //Perform cursor middle click
        cursorMiddleClick();
        break;
      }
      case CONF_ACTION_DEC_SPEED: {
        //Decrease cursor speed
        decreaseCursorSpeed();
        break;
      }
      case CONF_ACTION_INC_SPEED: {
        //Increase cursor speed
        increaseCursorSpeed();
        break;
      }
      case CONF_ACTION_CHANGE_MODE: {
        //Change communication mode
        changeComMode();
        break;
      }
   }

   if(action!=CONF_ACTION_DRAG && action!=CONF_ACTION_SCROLL && action!=CONF_ACTION_NOTHING){
    
    ledTimerId[0] = ledStateTimer.setTimeout(200,releaseOutputAction);
    //ledTimerId[0] = ledStateTimer.setTimeout(200,turnLedOnWithState,ledCurrentState);

    
   }
}


void cursorLeftClick(void) {
  //Serial.println("Left Click");
  if(comMode==1){
    mouse.click(MOUSE_LEFT);
  } 
  else if(comMode==2){
    btmouse.click(MOUSE_LEFT);
  }
}

void cursorRightClick(void) {
  //Serial.println("Right Click");  
  if(comMode==1){
    mouse.click(MOUSE_RIGHT);
  } 
  else if(comMode==2){
    btmouse.click(MOUSE_RIGHT);
  }
}

void cursorMiddleClick(void) {
  //Serial.println("Middle Click");  
  if(comMode==1){
    mouse.click(MOUSE_MIDDLE);
  } 
  else if(comMode==2){
    btmouse.click(MOUSE_MIDDLE);
  }
}

void cursorDrag(void) {
  //Serial.println("Drag");  
  if(comMode==1){
    mouse.press(MOUSE_LEFT);
  } 
  else if(comMode==2){
    btmouse.press(MOUSE_LEFT);
  }
}

void cursorScroll(void) {
  //Serial.println("Scroll");  
}

void setJoystickCenter(void) {
  js.updateInputComp();
  pointFloatType centerPoint = js.getInputComp();
  printJoystickFloatData(centerPoint);
}

void decreaseCursorSpeed(void) {
  Serial.println("Decrease Cursor Speed");  
}

void increaseCursorSpeed(void) {
  Serial.println("Increase Cursor Speed");  
}

void changeComMode(void) {
  Serial.println("Change Communication Mode");  
  if(comMode<2){ comMode++;}
  else { comMode=0;}
  Serial.println(comMode);  
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
  Serial.println("Calibration");  
//  for (int j = 1; j <= 7; j++) {
//    led.setLedColor(4,j,CONF_LED_BRIGHTNESS);
//    delay(500);
//  }
//  led.setLedColor(4,0,CONF_LED_BRIGHTNESS);
//  String commandKey;
//  pointFloatType maxPoint;
//  delay(1000);
//  
//  for (int i=1; i < 5; i++) {
//    commandKey="CA"+String(i);
//    led.setLedColor(2, LED_CLR_ORANGE, CONF_LED_BRIGHTNESS_HIGH); 
//    maxPoint=js.getInputMax(i);
//    mem.writePoint(CONF_SETTINGS_FILE,commandKey,maxPoint);
//    printJoystickFloatData(maxPoint);
//    led.clearLed(2);    
//    delay(1000);
//  }
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
  
  if(comMode==1){
    (outputAction==CONF_ACTION_SCROLL)? mouse.scroll(yVal/10) : mouse.move(xVal, -yVal);
  }
  else if(comMode==2){
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
    turnLedOff();
    setLedState(1,1,4,2,500);

//Serial.println(ledCurrentState->ledBrightness);
 ledStateStruct tempCurrentState ={2, 3, 1,  6,500,199};
 
   for (int i = 0; i <= 8; i++) {
    int color = i;
    if(color==8){color = 0; }
    //ledTimerId[i] = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime*(i+1),turnLedOnWithColor,(void *)color); 
   }
//turnLedOnWithState(&tempCurrentState);
   ledTimerId[0] = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime,turnLedOnWithState,(ledStateStruct*) ledCurrentState); 
}

void setLedState(int ledAction, int ledColorNumber, int ledNumber,  int ledBlinkNumber, unsigned long ledBlinkTime){ //Set led state after output action is performed 
  if(ledNumber<=OUTPUT_RGB_LED_NUM+1){
//    ledStateStruct tempCurrentState ={ledAction, ledColorNumber, ledNumber,  ledBlinkNumber,ledBlinkTime,CONF_LED_BRIGHTNESS};
//    ledCurrentState = &tempCurrentState;
    ledCurrentState->ledAction=ledAction;
    ledCurrentState->ledColorNumber=ledColorNumber;
    ledCurrentState->ledNumber=ledNumber;
    ledCurrentState->ledBlinkNumber=ledBlinkNumber;
    ledCurrentState->ledBlinkTime=ledBlinkTime;
    ledCurrentState->ledBrightness=CONF_LED_BRIGHTNESS;
  }

}

void turnLedOn(){
  led.setLedColor(ledCurrentState->ledNumber, ledCurrentState->ledColorNumber, ledCurrentState->ledBrightness);
} 

void turnLedOnWithColor(void * args){
  int color = (int)args;
  led.setLedColor(ledCurrentState->ledNumber, color, ledCurrentState->ledBrightness);
}

void turnLedOnWithState(ledStateStruct* args){

  //ledStateStruct* tempCurrentState = (ledStateStruct) args;
  Serial.println(args->ledNumber);

//    Serial.println(args->ledColorNumber);
//    Serial.println(args->ledBrightness);
//  int number = args->ledNumber;
//  int color = args->ledColorNumber;
//  int brightness = args->ledBrightness;
//  led.setLedColor(number, color, brightness);
}

void turnLedOff(){
    led.clearLed(ledCurrentState->ledNumber);
}

void turnLedAllOff(){
    led.clearLedAll();
    outputAction=CONF_ACTION_NOTHING;
}


void turnLedOnce(){
  ledTimerId[0] = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime,turnLedOn);
}


void blinkLed() {

  ledTimerId[0] = ledStateTimer.setTimer(ledCurrentState->ledBlinkTime, 0,ledCurrentState->ledBlinkNumber*2,turnLedOn);  
  ledTimerId[1] = ledStateTimer.setTimer(ledCurrentState->ledBlinkTime*2, 0,ledCurrentState->ledBlinkNumber+1,turnLedOff);   
 
}

void performLedAction(){
    switch (ledCurrentState->ledAction) {
      case LED_ACTION_OFF: {
        turnLedOff();
        break;
      }
      case LED_ACTION_ON: {
        turnLedOn();
        break;
      }
      case LED_ACTION_BLINK: {
        blinkLed();
        break;
      }
    }
}
