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
typedef struct
{
  uint8_t ledOutputActionNumber;
  uint8_t ledNumber;
  uint8_t ledStartColor;
  uint8_t ledEndColor;
  uint8_t ledEndAction;
} ledActionStruct;

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
    {CONF_ACTION_CHANGE_MODE,        2,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_NONE}
};

typedef struct
{
  int ledAction; //none = 0, off = 1, on = 2, blink = 3
  int ledColorNumber;
  int ledNumber;
  int ledBlinkNumber;
  unsigned long ledBlinkTime;
  int ledBrightness;
} ledStateStruct;

ledStateStruct* ledCurrentState = new ledStateStruct;

int ledTimerId;

LSTimer<ledStateStruct> ledStateTimer;

//Input module variables and structures

typedef struct
{
  uint8_t inputActionNumber;
  uint8_t inputActionState;
  unsigned long inputActionStartTime;
  unsigned long inputActionEndTime;
} inputActionStruct;

int inputActionSize;
inputStruct inputButtonActionState, inputSwitchActionState;

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

//Pressure module variables and structures

pressureStruct pressureValues = { 0.0, 0.0, 0.0 };

sapStruct sapActionState;

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

int pollTimerId[3];

LSTimer<void> pollTimer;

//General

int outputAction;
bool canOutputAction;

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

  comMode = CONF_COM_MODE;

  Serial.begin(115200);
  // Wait until serial port is opened
  while (!TinyUSBDevice.mounted())
    delay(1);
  //while (!Serial) { delay(1); }

  delay(1000);
  
  initMemory();

  initSipAndPuff();

  initLed();

  initInput();

  initJoystick();

  initFeedback();

  pollTimerId[0] = pollTimer.setInterval(CONF_JOYSTICK_POLL_RATE, 0, joystickLoop);
  pollTimerId[1] = pollTimer.setInterval(CONF_PRESSURE_POLL_RATE, 0, pressureLoop);
  pollTimerId[2] = pollTimer.setInterval(CONF_INPUT_POLL_RATE, 0, inputLoop);

} //end setup

void loop()
{

  ledStateTimer.run();
  calibTimer.run();
  pollTimer.run();
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

//*********************************//
// Input Functions
//*********************************//

void initInput()
{

  ib.begin();
  is.begin();
  inputActionSize = sizeof(switchActionProperty) / sizeof(inputActionStruct);
}

//The loop handling inputs
void inputLoop()
{

  ib.update();
  is.update(); //Request new values

  //Get the last state change
  inputButtonActionState = ib.getInputState();
  inputSwitchActionState = is.getInputState();

  //printInputData();

  bool canEvaluateAction = true;
  //Output action logic
  int tempActionIndex = 0;

  for (int buttonActionIndex = 0; buttonActionIndex < inputActionSize; buttonActionIndex++)
  {
    if (inputButtonActionState.mainState == buttonActionProperty[buttonActionIndex].inputActionState &&
      inputButtonActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputButtonActionState.elapsedTime >= buttonActionProperty[buttonActionIndex].inputActionStartTime &&
      inputButtonActionState.elapsedTime < buttonActionProperty[buttonActionIndex].inputActionEndTime)
    {

      tempActionIndex = buttonActionProperty[buttonActionIndex].inputActionNumber;

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
  }
  tempActionIndex = 0;
  if ( !canOutputAction ||((
    inputSwitchActionState.secondaryState == INPUT_SEC_STATE_RELEASED) &&
    (outputAction == CONF_ACTION_SCROLL ||
      outputAction == CONF_ACTION_DRAG)))
  {
    releaseOutputAction();
    canEvaluateAction = false;
  }

  for (int switchActionIndex = 0; switchActionIndex < inputActionSize && canEvaluateAction; switchActionIndex++)
  {
    if (inputSwitchActionState.mainState == switchActionProperty[switchActionIndex].inputActionState &&
      inputSwitchActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputSwitchActionState.elapsedTime >= switchActionProperty[switchActionIndex].inputActionStartTime &&
      inputSwitchActionState.elapsedTime < switchActionProperty[switchActionIndex].inputActionEndTime)
    {

      tempActionIndex = switchActionProperty[switchActionIndex].inputActionNumber;
      
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
  }
}

//*********************************//
// Sip and Puff Functions
//*********************************//

void initSipAndPuff()
{

  ps.begin(PRESS_TYPE_DIFF);
  setSipAndPuffThreshold();
  sapActionSize = sizeof(sapActionProperty) / sizeof(inputActionStruct);
}

void setSipAndPuffThreshold()
{
  ps.setStateThreshold(CONF_SIP_THRESHOLD, CONF_PUFF_THRESHOLD);
}

void updatePressure()
{
  ps.update();
}

void readPressure()
{

  pressureValues = ps.getAllPressure();
}

//The loop handling pressure polling, sip and puff state evaluation
void pressureLoop()
{

  updatePressure(); //Request new pressure difference from sensor and push it to array

  readPressure(); //Read the pressure object (can be last value from array, average or other algorithms)

  //Get the last state change
  sapActionState = ps.getState();

  //printSipAndPuffData(2);
  //Output action logic

  bool canEvaluateAction = true;
  //Logic to Skip Sip and puff action if it's in drag or scroll mode
  if (!canOutputAction || ((
    sapActionState.secondaryState == PRESS_SAP_SEC_STATE_RELEASED) &&
    (outputAction == CONF_ACTION_SCROLL ||
      outputAction == CONF_ACTION_DRAG)))
  {
    releaseOutputAction();
    canEvaluateAction = false;
  }

  int sapActionIndex = 0;
  int tempActionIndex = 0;
  //Perform output action and led action on sip and puff release
  //Perform led action on sip and puff start
  while (sapActionIndex < sapActionSize && canEvaluateAction)
  {
    //Perform output action and led action on sip and puff release
    if (sapActionState.mainState == sapActionProperty[sapActionIndex].inputActionState &&
      sapActionState.secondaryState == PRESS_SAP_SEC_STATE_RELEASED &&
      sapActionState.elapsedTime >= sapActionProperty[sapActionIndex].inputActionStartTime &&
      sapActionState.elapsedTime < sapActionProperty[sapActionIndex].inputActionEndTime)
    {

      tempActionIndex = sapActionProperty[sapActionIndex].inputActionNumber; //used for releasing drag or scroll

      setLedState(ledActionProperty[tempActionIndex].ledEndAction, 
      ledActionProperty[tempActionIndex].ledEndColor, 
      ledActionProperty[tempActionIndex].ledNumber, 
      1, 
      CONF_LED_REACTION_TIME, 
      CONF_LED_BRIGHTNESS);

      outputAction = tempActionIndex;

      performOutputAction(tempActionIndex);

      break;
    } //Perform led action on sip and puff start
    else if (sapActionState.mainState == sapActionProperty[sapActionIndex].inputActionState &&
      sapActionState.secondaryState == PRESS_SAP_SEC_STATE_STARTED &&
      sapActionState.elapsedTime >= sapActionProperty[sapActionIndex].inputActionStartTime &&
      sapActionState.elapsedTime < sapActionProperty[sapActionIndex].inputActionEndTime)
    {

      tempActionIndex = sapActionProperty[sapActionIndex].inputActionNumber; //used for releasing drag or scroll
      
      setLedState(ledActionProperty[tempActionIndex].ledEndAction, 
      ledActionProperty[tempActionIndex].ledStartColor, 
      ledActionProperty[tempActionIndex].ledNumber, 
      0, 
      0, 
      CONF_LED_BRIGHTNESS);

      outputAction = tempActionIndex;

      performLedAction(ledCurrentState);

      break;
    }
    sapActionIndex++;
  }
}

void releaseOutputAction()
{
  if (comMode == CONF_COMM_MODE_USB)
  {
    led.clearLedAll();
  }
  else if (comMode == CONF_COMM_MODE_BLE && btmouse.isConnected())
  {
    led.setLedColor(2, LED_CLR_BLUE, CONF_LED_BRIGHTNESS);
  }
  if (outputAction == CONF_ACTION_DRAG && (mouse.isPressed(MOUSE_LEFT) || btmouse.isPressed(MOUSE_LEFT)))
  {
    mouse.release(MOUSE_LEFT);
    btmouse.release(MOUSE_LEFT);
  }
  outputAction = CONF_ACTION_NOTHING;
  canOutputAction = true;
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
      setJoystickCalibration();
      break;
    }
    case CONF_ACTION_CURSOR_CENTER:
    {
      //Perform cursor center
      setJoystickCenter();
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
      decreaseCursorSpeed();
      break;
    }
    case CONF_ACTION_INC_SPEED:
    {
      //Increase cursor speed
      increaseCursorSpeed();
      break;
    }
    case CONF_ACTION_CHANGE_MODE:
    {
      //Change communication mode
      changeComMode();
      break;
    }
  }
  if(action!=CONF_ACTION_DRAG && action!=CONF_ACTION_SCROLL && action!=CONF_ACTION_NOTHING){
    outputAction=CONF_ACTION_NOTHING;
    canOutputAction = true;
  }
}

void cursorLeftClick(void)
{
  //Serial.println("Left Click");
  if (comMode == CONF_COMM_MODE_USB)
  {
    mouse.click(MOUSE_LEFT);
  }
  else if (comMode == CONF_COMM_MODE_BLE)
  {
    btmouse.click(MOUSE_LEFT);
  }
}

void cursorRightClick(void)
{
  //Serial.println("Right Click");
  if (comMode == CONF_COMM_MODE_USB)
  {
    mouse.click(MOUSE_RIGHT);
  }
  else if (comMode == CONF_COMM_MODE_BLE)
  {
    btmouse.click(MOUSE_RIGHT);
  }
}

void cursorMiddleClick(void)
{
  //Serial.println("Middle Click");
  if (comMode == CONF_COMM_MODE_USB)
  {
    mouse.click(MOUSE_MIDDLE);
  }
  else if (comMode == CONF_COMM_MODE_BLE)
  {
    btmouse.click(MOUSE_MIDDLE);
  }
}

void cursorDrag(void)
{
  //Serial.println("Drag");
  if (comMode == CONF_COMM_MODE_USB)
  {
    mouse.press(MOUSE_LEFT);
  }
  else if (comMode == CONF_COMM_MODE_BLE)
  {
    btmouse.press(MOUSE_LEFT);
  }
}

void cursorScroll(void)
{
  //Serial.println("Scroll");
}

void setJoystickCenter(void)
{
  js.updateInputComp();
  pointFloatType centerPoint = js.getInputComp();
  printJoystickFloatData(centerPoint);
}

void decreaseCursorSpeed(void)
{
  Serial.println("Decrease Cursor Speed");
}

void increaseCursorSpeed(void)
{
  Serial.println("Increase Cursor Speed");
}

void changeComMode(void)
{
  Serial.println("Change Communication Mode");
  if (comMode < 2)
  {
    comMode++;
  }
  else
  {
    comMode = 0;
  }
  Serial.println(comMode);
}

//*********************************//
// Joystick Functions
//*********************************//

void initJoystick()
{

  js.begin();
  js.setOutputScale(CONF_JOY_OUTPUT_SCALE);
  setJoystickCenter();
  getJoystickCalibration();
}

void getJoystickCalibration()
{
  String commandKey;
  pointFloatType maxPoint;
  for (int i = 1; i < 5; i++)
  {
    commandKey = "CA" + String(i);
    maxPoint = mem.readPoint(CONF_SETTINGS_FILE, commandKey);
    printJoystickFloatData(maxPoint);
    js.setInputMax(i, maxPoint);
  }
}

void setJoystickCalibration()
{
  js.clear();                                                                                           //Clear previous calibration values 
  int stepNumber = 0;
  calibTimerId[0] = calibTimer.setTimeout(CONF_JOY_CALIB_BLINK_TIME, performCalibration, (int *)stepNumber);  //Start the process  
  
}

void performCalibration(int* args)
{
  int stepNumber = (int)args;
  unsigned long readingDuration = CONF_JOY_CALIB_READING_DELAY*CONF_JOY_CALIB_READING_NUMBER;
  unsigned long currentReadingStart = CONF_JOY_CALIB_BLINK_TIME*((stepNumber*2)+1);
  unsigned long nextStepStart = currentReadingStart+readingDuration+CONF_JOY_CALIB_STEP_DELAY;

  if (stepNumber == 0)  //STEP 0: Joystick Compensation Center Point
  {
    setLedState(LED_ACTION_BLINK, LED_CLR_PURPLE, 2, 1, CONF_JOY_CALIB_BLINK_TIME,CONF_LED_BRIGHTNESS);  // LED Feedback to show start of setJoystickCenter
    performLedAction(ledCurrentState);
    setJoystickCenter();
    ++stepNumber;
    calibTimerId[0] = calibTimer.setTimeout(nextStepStart, performCalibration, (int *)stepNumber);      // Start next step
  }
  else if (stepNumber < 5) //STEP 1-4: Joystick Calibration Corner Points 
  {
    setLedState(LED_ACTION_BLINK, LED_CLR_PURPLE, 4, stepNumber, CONF_JOY_CALIB_BLINK_TIME,CONF_LED_BRIGHTNESS);    
    performLedAction(ledCurrentState);                                                                  // LED Feedback to show start of performCalibrationStep
    calibTimerId[1] = calibTimer.setTimer(CONF_JOY_CALIB_READING_DELAY, currentReadingStart, CONF_JOY_CALIB_READING_NUMBER, performCalibrationStep, (int *)stepNumber);
    ++stepNumber;                                                                                       //Set LED's feedback to show step is already started and get the max reading for the quadrant/step
    calibTimerId[0] = calibTimer.setTimeout(nextStepStart, performCalibration, (int *)stepNumber);      //Start next step
  } 
  else if (stepNumber == 5)
  {
    setLedState(LED_ACTION_OFF, LED_CLR_NONE, 4, 0, 0,CONF_LED_BRIGHTNESS);                            //Turn off Led's
    performLedAction(ledCurrentState);
    calibTimer.deleteTimer(0);                                                                          //Delete timer
  }

}

void performCalibrationStep(int* args)
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
    printJoystickFloatData(maxPoint);  
    setLedState(LED_ACTION_OFF, LED_CLR_NONE, 4, 0, 0,CONF_LED_BRIGHTNESS);                           
    performLedAction(ledCurrentState);      
  }
}


void updateJoystick()
{
  js.update();
}

void readJoystick()
{

  pointIntType joyOutPoint = js.getXYVal();
  xVal = joyOutPoint.x;
  yVal = joyOutPoint.y;
  //printJoystickIntData(joyOutPoint);
}

//The loop handling joystick

void joystickLoop()
{

  updateJoystick(); //Request new values

  readJoystick(); //Read the filtered values

  performJystick();
}

void performJystick()
{

  if (comMode == CONF_COMM_MODE_USB)
  {
    (outputAction == CONF_ACTION_SCROLL) ? mouse.scroll(round(yVal / 5)) : mouse.move(xVal, -yVal);
  }
  else if (comMode == CONF_COMM_MODE_BLE)
  {
    (outputAction == CONF_ACTION_SCROLL) ? btmouse.scroll(round(yVal / 5)) : btmouse.move(xVal, -yVal);
  }
}

//*********************************//
// Print Functions
//*********************************//

void printInputData()
{

  Serial.print(" main: ");
  Serial.print(inputButtonActionState.mainState);
  Serial.print(": ");
  Serial.print(inputSwitchActionState.mainState);
  Serial.print(", ");
  Serial.print(" secondary: ");
  Serial.print(inputButtonActionState.secondaryState);
  Serial.print(": ");
  Serial.print(inputSwitchActionState.secondaryState);
  Serial.print(", ");
  Serial.print(" time: ");
  Serial.print(inputButtonActionState.elapsedTime);
  Serial.print(": ");
  Serial.print(inputSwitchActionState.elapsedTime);
  Serial.print(", ");

  Serial.println();
}

void printSipAndPuffData(int type)
{

  if (type == 1)
  {
    Serial.print(" refPressure: ");
    Serial.print(pressureValues.refPressure);
    Serial.print(", ");
    Serial.print(" mainPressure: ");
    Serial.print(pressureValues.mainPressure);
    Serial.print(", ");
    Serial.print(" diffPressure: ");
    Serial.print(pressureValues.diffPressure);
  }
  else if (type == 2)
  {
    Serial.print(" main: ");
    Serial.print(sapActionState.mainState);
    Serial.print(", ");
    Serial.print(" secondary: ");
    Serial.print(sapActionState.secondaryState);
    Serial.print(", ");
    Serial.print(" time: ");
    Serial.print(sapActionState.elapsedTime);
    Serial.println();
  }
}

void printJoystickFloatData(pointFloatType point)
{

  Serial.print(" x: ");
  Serial.print(point.x);
  Serial.print(", ");
  Serial.print(" y: ");
  Serial.print(point.y);
  Serial.print(", ");

  Serial.println();
}

void printJoystickIntData(pointIntType point)
{

  Serial.print(" x: ");
  Serial.print(point.x);
  Serial.print(", ");
  Serial.print(" y: ");
  Serial.print(point.y);
  Serial.print(", ");

  Serial.println();
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


void initFeedback()
{
  canOutputAction = false;
  setLedState(LED_ACTION_BLINK, 1, 4, 4, 300, CONF_LED_BRIGHTNESS);
  ledTimerId = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime, ledIBMEffect, ledCurrentState);
  //setJoystickCalibration();
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
  led.setLedColor(args->ledNumber, args->ledColorNumber, args->ledBrightness);
  if (args->ledColorNumber == 0)
  {
    setLedState(LED_ACTION_NONE, LED_CLR_NONE, 0, 0, 0,0);
    ledStateTimer.deleteTimer(0);
  }
  else if (args->ledColorNumber < 7)
  {
    setLedState(args->ledAction, (args->ledColorNumber)+1, args->ledNumber, args->ledBlinkNumber, (args->ledBlinkTime)+500,args->ledBrightness);
    ledTimerId = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime, ledIBMEffect, ledCurrentState);
  } 
  else if (args->ledColorNumber == 7)
  {
    setLedState(args->ledAction, 0, args->ledNumber, args->ledBlinkNumber, (args->ledBlinkTime)+500,args->ledBrightness);
    ledTimerId = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime, ledIBMEffect, ledCurrentState);
    canOutputAction = true;
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
    
     setLedState(LED_ACTION_NONE, LED_CLR_NONE, 0, 0, 0,0);
     
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

void turnLedDefault(){
  Serial.println("test");

  if (comMode == CONF_COMM_MODE_BLE && btmouse.isConnected())
  {
    led.setLedColor(2, LED_CLR_BLUE, CONF_LED_BRIGHTNESS);
  }
}

void performLedAction(ledStateStruct* args)
{
  switch (args->ledAction)
  {
  case LED_ACTION_NONE:
  {
    turnLedDefault();
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
