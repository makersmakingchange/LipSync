#include <Wire.h>
#include "LSTimer.h"
#include "LSUtils.h"
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

int comMode; //0 = None , 1 = USB , 2 = Wireless  
int debugMode;

//LED module variables
const ledActionStruct ledActionProperty[]{
    {CONF_ACTION_NOTHING,            1,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_NONE},
    {CONF_ACTION_LEFT_CLICK,         1,LED_CLR_NONE,  LED_CLR_RED,    LED_ACTION_BLINK},
    {CONF_ACTION_RIGHT_CLICK,        3,LED_CLR_NONE,  LED_CLR_BLUE,   LED_ACTION_BLINK},
    {CONF_ACTION_DRAG,               1,LED_CLR_PURPLE,LED_CLR_RED,    LED_ACTION_ON},
    {CONF_ACTION_SCROLL,             3,LED_CLR_PURPLE,LED_CLR_BLUE,   LED_ACTION_ON},
    {CONF_ACTION_CURSOR_CALIBRATION, 4,LED_CLR_NONE,  LED_CLR_ORANGE, LED_ACTION_BLINK},
    {CONF_ACTION_CURSOR_CENTER,      2,LED_CLR_NONE,  LED_CLR_ORANGE, LED_ACTION_BLINK},
    {CONF_ACTION_MIDDLE_CLICK,       2,LED_CLR_NONE,  LED_CLR_PURPLE, LED_ACTION_BLINK},
    {CONF_ACTION_DEC_SPEED,          1,LED_CLR_NONE,  LED_CLR_RED,    LED_ACTION_BLINK},
    {CONF_ACTION_INC_SPEED,          3,LED_CLR_NONE,  LED_CLR_BLUE,   LED_ACTION_BLINK},
    {CONF_ACTION_CHANGE_MODE,        2,LED_CLR_NONE,  LED_CLR_BLUE,   LED_ACTION_BLINK}
};

ledStateStruct* ledCurrentState = new ledStateStruct;

int ledTimerId;

LSTimer<ledStateStruct> ledStateTimer;

//Input module variables

int buttonActionSize, switchActionSize;
inputStateStruct buttonState, switchState;

const inputActionStruct switchActionProperty[]{
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

const inputActionStruct buttonActionProperty[]{
    {CONF_ACTION_NOTHING,            INPUT_MAIN_STATE_NONE,        0,0},
    {CONF_ACTION_DEC_SPEED,          INPUT_MAIN_STATE_S1_PRESSED,  0,1000},
    {CONF_ACTION_INC_SPEED,          INPUT_MAIN_STATE_S3_PRESSED,  0,1000},
    {CONF_ACTION_CURSOR_CENTER,      INPUT_MAIN_STATE_S2_PRESSED,  0,1000},
    {CONF_ACTION_CHANGE_MODE,        INPUT_MAIN_STATE_S2_PRESSED,  1000,3000},
    {CONF_ACTION_CURSOR_CALIBRATION, INPUT_MAIN_STATE_S13_PRESSED, 0,1000}
};

int inputButtonPinArray[] = { CONF_BUTTON1_PIN, CONF_BUTTON2_PIN, CONF_BUTTON3_PIN };
int inputSwitchPinArray[] = { CONF_SWITCH1_PIN, CONF_SWITCH2_PIN, CONF_SWITCH3_PIN };

//Pressure module variables

pressureStruct pressureValues = { 0.0, 0.0, 0.0 };

inputStateStruct sapActionState;

int sapActionSize;

const inputActionStruct sapActionProperty[]{
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

int calibTimerId[2];

LSTimer<int> calibTimer;

//Timer related variables

int pollTimerId[5];

LSTimer<void> pollTimer;

//General

int outputAction;
bool canOutputAction = true;

bool settingsEnabled = false;                        //Serial input settings command mode enabled or disabled

//Create instances of classes

LSMemory mem;

LSInput ib(inputButtonPinArray, CONF_BUTTON_NUMBER);
LSInput is(inputSwitchPinArray, CONF_SWITCH_NUMBER); //Starts an instance of the object

LSJoystick js; //Starts an instance of the LSJoystick object

LSPressure ps; //Starts an instance of the LSPressure object

LSOutput led; //Starts an instance of the LSOutput led object

LSUSBMouse mouse; //Starts an instance of the usb mouse object
LSBLEMouse btmouse;

void setup()
{

  mouse.begin();
  btmouse.begin();

  Serial.begin(115200);
  // Wait until serial port is opened
  //while (!TinyUSBDevice.mounted())
  //while (!Serial) { delay(10); }

  delay(1000);
  
  initMemory();

  initSipAndPuff();

  initLed();

  initInput();

  initJoystick();

  initCommunicationMode();

  initDebug();

  srartupFeedback();

  pollTimerId[0] = pollTimer.setInterval(CONF_JOYSTICK_POLL_RATE, 0, joystickLoop);
  pollTimerId[1] = pollTimer.setInterval(CONF_PRESSURE_POLL_RATE, 0, pressureLoop);
  pollTimerId[2] = pollTimer.setInterval(CONF_INPUT_POLL_RATE, 0, inputLoop);
  pollTimerId[3] = pollTimer.setInterval(CONF_BLE_FEEDBACK_POLL_RATE, 0, bleFeedbackLoop);
  pollTimerId[4] = pollTimer.setInterval(CONF_DEBUG_POLL_RATE, 0, debugLoop);

  
  enablePoll(false);                              //Enable when the led IBM effect is complete 

} //end setup

void loop()
{

  ledStateTimer.run();
  calibTimer.run();
  pollTimer.run();
  settingsEnabled=serialSettings(settingsEnabled); //Check to see if setting option is enabled in Lipsync
}

void enablePoll(bool isEnabled){
  if(isEnabled){
    getDebugMode(false,false);
    pollTimer.enable(1); 
    pollTimer.enable(2);    
    pollTimer.enable(3);  
    //setLedDefault();
  } else {
    pollTimer.disable(0);    
    pollTimer.disable(1);
    pollTimer.disable(2);
    pollTimer.disable(3);  
    pollTimer.disable(4); 
  }
}

//*********************************//
// Memory Functions
//*********************************//

void initMemory()
{
  mem.begin();
  //mem.format();
  mem.initialize(CONF_SETTINGS_FILE, CONF_SETTINGS_JSON);
}

void resetMemory()
{
  mem.format();
  mem.initialize(CONF_SETTINGS_FILE, CONF_SETTINGS_JSON);
}


//*********************************//
// Communication Mode Functions
//*********************************//

void initCommunicationMode()
{
  comMode = getCommunicationMode(false,false);
}


//*********************************//
// Input Functions
//*********************************//

void initInput()
{

  ib.begin();
  is.begin();
  buttonActionSize = sizeof(buttonActionProperty) / sizeof(inputActionStruct);
  switchActionSize = sizeof(switchActionProperty) / sizeof(inputActionStruct);
}

//The loop handling inputs
void inputLoop()
{

  ib.update();
  is.update(); //Request new values

  //Get the last state change
  buttonState = ib.getInputState();
  switchState = is.getInputState();

  evaluateOutputAction(buttonState, buttonActionSize,buttonActionProperty);
  evaluateOutputAction(switchState, switchActionSize,switchActionProperty);
}

//*********************************//
// Sip and Puff Functions
//*********************************//

void initSipAndPuff()
{

  ps.begin(PRESS_TYPE_DIFF);
  getPressureThreshold(true,false);
  sapActionSize = sizeof(sapActionProperty) / sizeof(inputActionStruct);
}


//The loop handling pressure polling, sip and puff state evaluation
void pressureLoop()
{

  ps.update(); //Request new pressure difference from sensor and push it to array

  pressureValues = ps.getAllPressure(); //Read the pressure object (can be last value from array, average or other algorithms)

  //Get the last state change
  sapActionState = ps.getState();

  //Output action logic
  evaluateOutputAction(sapActionState, sapActionSize,sapActionProperty);
}

void releaseOutputAction()
{
  setLedDefault();
  if (outputAction == CONF_ACTION_DRAG && (mouse.isPressed(MOUSE_LEFT) || btmouse.isPressed(MOUSE_LEFT)))
  {
    mouse.release(MOUSE_LEFT);
    btmouse.release(MOUSE_LEFT);
  }
  outputAction = CONF_ACTION_NOTHING;
  canOutputAction = true;
}

void evaluateOutputAction(inputStateStruct actionState, int actionSize,const inputActionStruct actionProperty[])
{
  bool canEvaluateAction = true;
  //Output action logic
  int tempActionIndex = 0;
  if ((
    actionState.secondaryState == INPUT_SEC_STATE_RELEASED) &&
    (outputAction == CONF_ACTION_SCROLL ||
      outputAction == CONF_ACTION_DRAG))
  {
    releaseOutputAction();
    canEvaluateAction = false;
  }

  for (int actionIndex = 0; actionIndex < actionSize && canEvaluateAction && canOutputAction; actionIndex++)
  {
    if (actionState.mainState == actionProperty[actionIndex].inputActionState &&
      actionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      actionState.elapsedTime >= actionProperty[actionIndex].inputActionStartTime &&
      actionState.elapsedTime < actionProperty[actionIndex].inputActionEndTime)
    {

      tempActionIndex = actionProperty[actionIndex].inputActionNumber;
      
      setLedState(ledActionProperty[tempActionIndex].ledEndAction, 
      ledActionProperty[tempActionIndex].ledEndColor, 
      ledActionProperty[tempActionIndex].ledNumber, 
      1, 
      CONF_LED_REACTION_TIME, 
      CONF_LED_BRIGHTNESS);
      outputAction = tempActionIndex;

      performOutputAction(tempActionIndex);

      break;
    }
    else if (actionState.mainState == actionProperty[actionIndex].inputActionState &&
      actionState.secondaryState == INPUT_SEC_STATE_STARTED &&
      actionState.elapsedTime >= actionProperty[actionIndex].inputActionStartTime &&
      actionState.elapsedTime < actionProperty[actionIndex].inputActionEndTime)
    {

      tempActionIndex = actionProperty[actionIndex].inputActionNumber; //used for releasing drag or scroll
      
      setLedState(ledActionProperty[tempActionIndex].ledEndAction, 
      ledActionProperty[tempActionIndex].ledStartColor, 
      ledActionProperty[tempActionIndex].ledNumber, 
      0, 
      0, 
      CONF_LED_BRIGHTNESS);

      performLedAction(ledCurrentState);

      break;
    }
  }  
}

void performOutputAction(int action)
{
  
  performLedAction(ledCurrentState);
  switch (action)
  {
    case CONF_ACTION_NOTHING:
    {
      break;
    }
    case CONF_ACTION_LEFT_CLICK:
    {
      cursorLeftClick();
      break;
    }
    case CONF_ACTION_RIGHT_CLICK:
    {
      cursorRightClick();
      break;
    }
    case CONF_ACTION_DRAG:
    {
      cursorDrag();
      break;
    }
    case CONF_ACTION_SCROLL:
    {
      cursorScroll(); //Enter Scroll mode
      break;
    }
    case CONF_ACTION_CURSOR_CALIBRATION:
    {
      setJoystickCalibration(false,false);
      break;
    }
    case CONF_ACTION_CURSOR_CENTER:
    {
      //Perform cursor center
      setJoystickInitialization(false,false);
      break;
    }
    case CONF_ACTION_MIDDLE_CLICK:
    {
      //Perform cursor middle click
      cursorMiddleClick();
      break;
    }
    case CONF_ACTION_DEC_SPEED:
    {
      //Decrease cursor speed
      decreaseJoystickSpeed(false,false);
      break;
    }
    case CONF_ACTION_INC_SPEED:
    {
      //Increase cursor speed
      increaseJoystickSpeed(false,false);
      break;
    }
    case CONF_ACTION_CHANGE_MODE:
    {
      //Change communication mode
      toggleCommunicationMode(false,false);
      break;
    }
  }
  if(action==CONF_ACTION_DRAG || action==CONF_ACTION_SCROLL){
    canOutputAction = false;
  }
  else {
    outputAction=CONF_ACTION_NOTHING;
    canOutputAction = true;
  }
}

void cursorLeftClick(void)
{
  //Serial.println("Left Click");
  if (comMode == CONF_COM_MODE_USB)
  {
    mouse.click(MOUSE_LEFT);
  }
  else if (comMode == CONF_COM_MODE_BLE)
  {
    btmouse.click(MOUSE_LEFT);
  }
}

void cursorRightClick(void)
{
  //Serial.println("Right Click");
  if (comMode == CONF_COM_MODE_USB)
  {
    mouse.click(MOUSE_RIGHT);
  }
  else if (comMode == CONF_COM_MODE_BLE)
  {
    btmouse.click(MOUSE_RIGHT);
  }
}

void cursorMiddleClick(void)
{
  //Serial.println("Middle Click");
  if (comMode == CONF_COM_MODE_USB)
  {
    mouse.click(MOUSE_MIDDLE);
  }
  else if (comMode == CONF_COM_MODE_BLE)
  {
    btmouse.click(MOUSE_MIDDLE);
  }
}

void cursorDrag(void)
{
  //Serial.println("Drag");
  if (comMode == CONF_COM_MODE_USB)
  {
    mouse.press(MOUSE_LEFT);
  }
  else if (comMode == CONF_COM_MODE_BLE)
  {
    btmouse.press(MOUSE_LEFT);
  }
}

void cursorScroll(void)
{
  //Serial.println("Scroll");
}


//*********************************//
// Joystick Functions
//*********************************//

void initJoystick()
{

  js.begin();
  js.setMagnetDirection(JOY_DIRECTION_DEFAULT,JOY_DIRECTION_INVERSE);       //x,y 
  getJoystickSpeed(true,false);
  setJoystickInitialization(true,false);
  getJoystickCalibration(true,false);
}

//void getJoystickCalibration()
//{
//  String commandKey;
//  pointFloatType maxPoint;
//  for (int i = 1; i < 5; i++)
//  {
//    commandKey = "CA" + String(i);
//    maxPoint = mem.readPoint(CONF_SETTINGS_FILE, commandKey);
//    printJoystickFloatData(maxPoint);
//    js.setInputMax(i, maxPoint);
//  }
//}

//void setJoystickCalibration()
//{
//  js.clear();                                                                                           //Clear previous calibration values 
//  int stepNumber = 0;
//  canOutputAction = false;
//  calibTimerId[0] = calibTimer.setTimeout(CONF_JOY_CALIB_BLINK_TIME, performJoystickCalibration, (int *)stepNumber);  //Start the process  
//}

void performJoystickCalibration(int* args)
{
  int stepNumber = (int)args;
  unsigned long readingDuration = CONF_JOY_CALIB_READING_DELAY*CONF_JOY_CALIB_READING_NUMBER;
  unsigned long currentReadingStart = CONF_JOY_CALIB_BLINK_TIME*((stepNumber*2)+1);
  unsigned long nextStepStart = currentReadingStart+readingDuration+CONF_JOY_CALIB_STEP_DELAY;
  pointFloatType centerPoint;

  if (stepNumber == 0)  //STEP 0: Joystick Compensation Center Point
  {
    setLedState(LED_ACTION_BLINK, LED_CLR_ORANGE, 2, 1, CONF_JOY_CALIB_BLINK_TIME,CONF_LED_BRIGHTNESS);  // LED Feedback to show start of setJoystickCenter
    performLedAction(ledCurrentState);
    setJoystickInitialization(false,false);
    centerPoint=js.getInputComp();
    printResponseFloatPoint(true,true,true,0,"CA,0",true,centerPoint);
    ++stepNumber;
    calibTimerId[0] = calibTimer.setTimeout(nextStepStart, performJoystickCalibration, (int *)stepNumber);      // Start next step
  }
  else if (stepNumber < 5) //STEP 1-4: Joystick Calibration Corner Points 
  {
    setLedState(LED_ACTION_BLINK, LED_CLR_PURPLE, 4, stepNumber, CONF_JOY_CALIB_BLINK_TIME,CONF_LED_BRIGHTNESS);    
    performLedAction(ledCurrentState);                                                                  // LED Feedback to show start of performJoystickCalibrationStep
    calibTimerId[1] = calibTimer.setTimer(CONF_JOY_CALIB_READING_DELAY, currentReadingStart, CONF_JOY_CALIB_READING_NUMBER, performJoystickCalibrationStep, (int *)stepNumber);
    ++stepNumber;                                                                                       //Set LED's feedback to show step is already started and get the max reading for the quadrant/step
    calibTimerId[0] = calibTimer.setTimeout(nextStepStart, performJoystickCalibration, (int *)stepNumber);      //Start next step
  } 
  else if (stepNumber == 5)
  {
    setLedState(LED_ACTION_NONE, LED_CLR_NONE, 4, 0, 0,CONF_LED_BRIGHTNESS_LOW);                            //Turn off Led's
    performLedAction(ledCurrentState);
    calibTimer.deleteTimer(0);                                                                          //Delete timer
    canOutputAction = true;
  }

}

void performJoystickCalibrationStep(int* args)
{
  int stepNumber = (int)args;
  String stepCommand = "CA"+String(stepNumber);                                                       //Command to write new calibration point to Flash memory 
  pointFloatType maxPoint;
  
  if(calibTimer.getNumRuns(0)==1){                                                                    //Turn Led's ON when timer is running for first time
    setLedState(LED_ACTION_ON, LED_CLR_ORANGE, 4, 0, 0,CONF_LED_BRIGHTNESS);
    performLedAction(ledCurrentState);   
  }
  
  maxPoint=js.getInputMax(stepNumber);
  if(calibTimer.getNumRuns(0)==CONF_JOY_CALIB_READING_NUMBER){                                        //Turn Led's OFF when timer is running for last time
    mem.writePoint(CONF_SETTINGS_FILE,stepCommand,maxPoint);                                          //Store the point in Flash Memory 
    setLedState(LED_ACTION_OFF, LED_CLR_NONE, 4, 0, 0,CONF_LED_BRIGHTNESS);                           
    performLedAction(ledCurrentState);      
    printResponseFloatPoint(true,true,true,0,"CA,1",true,maxPoint);
  }
}


//The loop handling joystick

void joystickLoop()
{

  js.update(); //Request new values

  pointIntType joyOutPoint = js.getXYOut(); //Read the filtered values

  performJystick(joyOutPoint);
}

void performJystick(pointIntType inputPoint)
{

  if (comMode == CONF_COM_MODE_USB)
  {
    (outputAction == CONF_ACTION_SCROLL) ? mouse.scroll(round(inputPoint.y / 5)) : mouse.move(inputPoint.x, -inputPoint.y);
  }
  else if (comMode == CONF_COM_MODE_BLE)
  {
    (outputAction == CONF_ACTION_SCROLL) ? btmouse.scroll(round(inputPoint.y / 5)) : btmouse.move(inputPoint.x, -inputPoint.y);
  }
}


//*********************************//
// Debug Functions
//*********************************//

void initDebug()
{
  debugMode = getDebugMode(false, false);
  setDebugState(debugMode);
}

void debugLoop(){
  if(debugMode==CONF_DEBUG_MODE_JOYSTICK){
    js.update(); //Request new values from joystick class
    pointFloatType debugJoystickArray[2];
    debugJoystickArray[0] = js.getXYIn();  //Read the raw values
    debugJoystickArray[1] = {(float)js.getXYOut().x,(float)js.getXYOut().y}; //Read the filtered values
    printResponseFloatPointArray(true,true,true,0,"DEBUG,1",true,"", 2, ',', debugJoystickArray);    
  }
  else if(debugMode==CONF_DEBUG_MODE_PRESSURE){  //Use update values from pressureLoop()
    //ps.update(); //Request new pressure difference from sensor and push it to array
    float debugPressureArray[3];
    debugPressureArray[0] = ps.getMainPressure();  //Read the main pressure 
    debugPressureArray[1] = ps.getRefPressure();   //Read the ref pressure
    debugPressureArray[2] = ps.getDiffPressure();  //Read the diff pressure
    printResponseFloatArray(true,true,true,0,"DEBUG,2",true,"", 3, ',', debugPressureArray);    
  }
  else if(debugMode==CONF_DEBUG_MODE_BUTTON){  
    int debugButtonArray[3];
    debugButtonArray[0] = buttonState.mainState;             //Read the main state 
    debugButtonArray[1] = buttonState.secondaryState;        //Read the secondary state
    debugButtonArray[2] = (int) buttonState.elapsedTime;     //Read the Elapsed Time     
    printResponseIntArray(true,true,true,0,"DEBUG,3",true,"", 3, ',', debugButtonArray);    
  }
  else if(debugMode==CONF_DEBUG_MODE_SWITCH){  
    int debugSwitchArray[3];
    debugSwitchArray[0] = switchState.mainState;             //Read the main state 
    debugSwitchArray[1] = switchState.secondaryState;        //Read the secondary state 
    debugSwitchArray[2] = (int) switchState.elapsedTime;     //Read the Elapsed Time    
    printResponseIntArray(true,true,true,0,"DEBUG,4",true,"", 3, ',', debugSwitchArray);    
  }
  else if(debugMode==CONF_DEBUG_MODE_SAP){  
    int debugSapArray[3];
    debugSapArray[0] = sapActionState.mainState;             //Read the main state 
    debugSapArray[1] = sapActionState.secondaryState;        //Read the secondary state 
    debugSapArray[2] = (int) sapActionState.elapsedTime;     //Read the Elapsed Time    
    printResponseIntArray(true,true,true,0,"DEBUG,5",true,"", 3, ',', debugSapArray);    
  }
}

void setDebugState(int inputDebugState) {
  if (inputDebugState==CONF_DEBUG_MODE_NONE) {
    pollTimer.enable(0);                      //Enable joystick data polling 
    pollTimer.disable(4);                     //Disable debug data polling 
  } 
  else if (inputDebugState==CONF_DEBUG_MODE_JOYSTICK) {
    pollTimer.disable(0);                     //Disable joystick data polling 
    pollTimer.enable(4);                      //Enable debug data polling 
  }
  else {
    pollTimer.enable(4);                      //Enable debug data polling 
  }
}


//*********************************//
// LED Functions
//*********************************//

void initLed()
{

  led.begin();
  ledTimerId=0;
  *ledCurrentState = { 0, 0, 0, 0, 0, 0 };
}


void srartupFeedback()
{
  setLedState(LED_ACTION_BLINK, 1, 4, 4, CONF_LED_STARTUP_COLOR_TIME, CONF_LED_BRIGHTNESS);
  ledTimerId = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime, ledIBMEffect, ledCurrentState);

}

void setLedState(int ledAction, int ledColorNumber, int ledNumber, int ledBlinkNumber, unsigned long ledBlinkTime, int ledBrightness)
{ //Set led state after output action is performed
  if (ledNumber <= OUTPUT_RGB_LED_NUM + 1)
  {
    
    ledCurrentState->ledAction = ledAction;
    ledCurrentState->ledColorNumber = ledColorNumber;
    ledCurrentState->ledNumber = ledNumber;
    ledCurrentState->ledBlinkNumber = ledBlinkNumber;
    ledCurrentState->ledBlinkTime = ledBlinkTime;
    ledCurrentState->ledBrightness = ledBrightness;
  }
}



void ledIBMEffect(ledStateStruct* args)
{
  
  if (args->ledColorNumber == 0)
  {
    //ledStateTimer.deleteTimer(0); 
    enablePoll(true);
  }
  else if (args->ledColorNumber < 7)
  {
    led.setLedColor(args->ledNumber, args->ledColorNumber, args->ledBrightness);
    setLedState(args->ledAction, (args->ledColorNumber)+1, args->ledNumber, args->ledBlinkNumber, (args->ledBlinkTime),args->ledBrightness);
    ledTimerId = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime, ledIBMEffect, ledCurrentState);
  } 
  else if (args->ledColorNumber == 7)
  {
    setLedState(LED_ACTION_OFF, 0, args->ledNumber, args->ledBlinkNumber, (args->ledBlinkTime),args->ledBrightness);
    performLedAction(ledCurrentState);
    ledTimerId = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime, ledIBMEffect, ledCurrentState);
     
  }

}

void ledBlinkEffect(ledStateStruct* args){
  if(ledStateTimer.getNumRuns(0) % 2){
     led.setLedColor(args->ledNumber, 0, args->ledBrightness);
  } 
  else{
    led.setLedColor(args->ledNumber, args->ledColorNumber, args->ledBrightness);
  }

  if(ledStateTimer.getNumRuns(0)==((args->ledBlinkNumber)*2)+1){
    
     setLedState(LED_ACTION_NONE, LED_CLR_NONE, 0, 0, 0,CONF_LED_BRIGHTNESS_LOW);
     
  }   
}

void turnLedAllOff()
{
  led.clearLedAll();
}


void turnLedOff(ledStateStruct* args)
{
  led.clearLed(args->ledNumber);
}


void turnLedOn(ledStateStruct* args)
{
  led.setLedColor(args->ledNumber, args->ledColorNumber, args->ledBrightness);
}

void blinkLed(ledStateStruct* args)
{
  ledTimerId = ledStateTimer.setTimer(args->ledBlinkTime, 0, ((args->ledBlinkNumber)*2)+1, ledBlinkEffect, ledCurrentState);
}

void setLedDefault(){
  if (comMode == CONF_COM_MODE_USB)
  {
    led.clearLedAll();
  }
  else if (comMode == CONF_COM_MODE_BLE && btmouse.isConnected())
  {
    led.clearLedAll();
    led.setLedColor(2, LED_CLR_BLUE, CONF_LED_BRIGHTNESS);
  }
}

void bleFeedbackLoop()
{
  if (comMode == CONF_COM_MODE_BLE && btmouse.isConnected())
  {
    led.setLedColor(2, LED_CLR_BLUE, CONF_LED_BRIGHTNESS);
  }
  else if (comMode == CONF_COM_MODE_BLE && btmouse.isConnected()==false)
  {
    led.clearLed(2);
  }
}


void performLedAction(ledStateStruct* args)
{
  switch (args->ledAction)
  {
  case LED_ACTION_NONE:
  {
    setLedDefault();
    break;
  }
  case LED_ACTION_OFF:
  {
    turnLedOff(args);
    break;
  }
  case LED_ACTION_ON:
  {
    turnLedOn(args);
    break;
  }
  case LED_ACTION_BLINK:
  {
    blinkLed(args);
    break;
  }
  }
}
