/* 
* File: LipSync4_Gamepad.ino
* Firmware: LipSync4 Gamepad
* Developed by: MakersMakingChange
* Version: Alpha 2 (20 April 2022) 
* Copyright Neil Squire Society 2022. 
* License: This work is licensed under the CC BY SA 4.0 License: http://creativecommons.org/licenses/by-sa/4.0 .
*/


#include <Wire.h>
#include "LSTimer.h"
#include "LSUtils.h"
#include <ArduinoJson.h>
#include "LSOutput.h"
#include "LSConfig.h"
#include "LSGamepad.h"
#include "LSCircularBuffer.h"
#include "LSInput.h"
#include "LSPressure.h"
#include "LSJoystick.h"
#include "LSMemory.h"

//Operation mode and debug mode variables
int operationMode;                                                                                          // 0 = None, 1 = Digital , 2 = Analog 
int debugMode;                                                                                              // 0 = Debug mode is Off
                                                                                                            // 1 = Joystick debug mode is On
                                                                                                            // 2 = Pressure debug mode is On
                                                                                                            // 3 = Buttons debug mode is On
                                                                                                            // 4 = Switch debug mode is On
                                                                                                            // 5 = Sip & Puff state debug mode is On
//Bluetooth connection variables
bool btIsConnected = false;

//LED module variables
bool ledActionEnabled = false;

//LED Action for all available output actions 
const ledActionStruct ledActionProperty[]{
    {CONF_ACTION_NOTHING,             4,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_NONE},
    {CONF_ACTION_B1_PRESS,            4,LED_CLR_NONE,  LED_CLR_MAGENTA,LED_ACTION_BLINK},
    {CONF_ACTION_B2_PRESS,            4,LED_CLR_NONE,  LED_CLR_MAGENTA,LED_ACTION_BLINK},
    {CONF_ACTION_B3_PRESS,            4,LED_CLR_NONE,  LED_CLR_MAGENTA,LED_ACTION_BLINK},
    {CONF_ACTION_B4_PRESS,            4,LED_CLR_YELLOW,LED_CLR_MAGENTA,LED_ACTION_BLINK},
    {CONF_ACTION_B5_PRESS,            4,LED_CLR_YELLOW,LED_CLR_MAGENTA,LED_ACTION_BLINK},
    {CONF_ACTION_B6_PRESS,            4,LED_CLR_YELLOW,LED_CLR_MAGENTA,LED_ACTION_BLINK},
    {CONF_ACTION_B7_PRESS,            4,LED_CLR_YELLOW,LED_CLR_MAGENTA,LED_ACTION_BLINK},
    {CONF_ACTION_B8_PRESS,            4,LED_CLR_YELLOW,LED_CLR_MAGENTA,LED_ACTION_BLINK},
    {CONF_ACTION_JOYSTICK_CENTER,     2,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_NONE},
    {CONF_ACTION_JOYSTICK_CALIBRATION,4,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_NONE},
    {CONF_ACTION_DEC_SPEED,           1,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_NONE},
    {CONF_ACTION_INC_SPEED,           3,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_NONE},
    {CONF_ACTION_CHANGE_MODE,         2,LED_CLR_NONE,  LED_CLR_NONE,   LED_ACTION_NONE},
    {CONF_ACTION_FACTORY_RESET,       4,LED_CLR_YELLOW,LED_CLR_NONE,   LED_ACTION_NONE}
};

ledStateStruct* ledCurrentState = new ledStateStruct;

int ledTimerId[3];

LSTimer<ledStateStruct> ledStateTimer;

//Input module variables

int buttonActionSize, switchActionSize;
unsigned long buttonActionMaxTime, switchActionMaxTime;
inputStateStruct buttonState, switchState;

const inputActionStruct switchActionProperty[]{
    {CONF_ACTION_NOTHING,              INPUT_MAIN_STATE_NONE,       0,0},
    {CONF_ACTION_B1_PRESS,             INPUT_MAIN_STATE_S1_PRESSED, 0,3000},
    {CONF_ACTION_B2_PRESS,             INPUT_MAIN_STATE_S2_PRESSED, 0,3000},
    {CONF_ACTION_B3_PRESS,             INPUT_MAIN_STATE_S3_PRESSED, 0,3000},
    {CONF_ACTION_B4_PRESS,             INPUT_MAIN_STATE_S1_PRESSED, 3000,5000},
    {CONF_ACTION_B5_PRESS,             INPUT_MAIN_STATE_S2_PRESSED, 3000,5000},
    {CONF_ACTION_B6_PRESS,             INPUT_MAIN_STATE_S3_PRESSED, 3000,5000},
    {CONF_ACTION_B7_PRESS,             INPUT_MAIN_STATE_S1_PRESSED, 5000,10000},
    {CONF_ACTION_B8_PRESS,             INPUT_MAIN_STATE_S2_PRESSED, 5000,10000},
    {CONF_ACTION_NOTHING,              INPUT_MAIN_STATE_S3_PRESSED, 5000,10000},
};

const inputActionStruct buttonActionProperty[]{
    {CONF_ACTION_NOTHING,              INPUT_MAIN_STATE_NONE,        0,0},
    {CONF_ACTION_DEC_SPEED,            INPUT_MAIN_STATE_S1_PRESSED,  0,3000},
    {CONF_ACTION_JOYSTICK_CENTER,      INPUT_MAIN_STATE_S2_PRESSED,  0,3000},
    {CONF_ACTION_INC_SPEED,            INPUT_MAIN_STATE_S3_PRESSED,  0,3000},
    {CONF_ACTION_NOTHING,              INPUT_MAIN_STATE_S1_PRESSED,  3000,5000},
    {CONF_ACTION_CHANGE_MODE,          INPUT_MAIN_STATE_S2_PRESSED,  3000,5000},
    {CONF_ACTION_NOTHING,              INPUT_MAIN_STATE_S3_PRESSED,  3000,5000},
    {CONF_ACTION_JOYSTICK_CALIBRATION, INPUT_MAIN_STATE_S13_PRESSED, 0,3000},
    {CONF_ACTION_FACTORY_RESET,        INPUT_MAIN_STATE_S13_PRESSED, 3000,5000}
};

int inputButtonPinArray[] = { CONF_BUTTON1_PIN, CONF_BUTTON2_PIN, CONF_BUTTON3_PIN };
int inputSwitchPinArray[] = { CONF_SWITCH1_PIN, CONF_SWITCH2_PIN, CONF_SWITCH3_PIN };

//Pressure module variables

pressureStruct pressureValues = { 0.0, 0.0, 0.0 };

inputStateStruct sapActionState;

int sapActionSize;
unsigned long sapActionMaxTime = 0;

const inputActionStruct sapActionProperty[]{
    {CONF_ACTION_NOTHING,            PRESS_SAP_MAIN_STATE_NONE,  0,0},
    {CONF_ACTION_B1_PRESS,           PRESS_SAP_MAIN_STATE_PUFF,  0,3000},
    {CONF_ACTION_B2_PRESS,           PRESS_SAP_MAIN_STATE_SIP,   0,3000},
    {CONF_ACTION_B3_PRESS,           PRESS_SAP_MAIN_STATE_PUFF,  3000,5000},
    {CONF_ACTION_B4_PRESS,           PRESS_SAP_MAIN_STATE_SIP,   3000,5000},
    {CONF_ACTION_JOYSTICK_CENTER,    PRESS_SAP_MAIN_STATE_PUFF,  5000,10000},
    {CONF_ACTION_B5_PRESS,           PRESS_SAP_MAIN_STATE_SIP,   5000,10000}
};

//Joystick module variables and structures

int xVal;
int yVal;

int actionTimerId[1];

LSTimer<int> actionTimer;

int calibTimerId[2];

LSTimer<int> calibTimer;

//Timer related variables

int pollTimerId[6];

LSTimer<void> pollTimer;

//General

int outputAction;
bool canOutputAction = true;

bool settingsEnabled = false;                        //Serial input settings command mode enabled or disabled

//Create instances of classes

LSMemory mem;

LSInput ib(inputButtonPinArray, CONF_BUTTON_NUMBER);
LSInput is(inputSwitchPinArray, CONF_SWITCH_NUMBER); //Starts an instance of the object

LSJoystick js;                                       //Starts an instance of the LSJoystick object

LSPressure ps;                                       //Starts an instance of the LSPressure object

LSOutput led;                                        //Starts an instance of the LSOutput LED object

LSGamepad gamepad;                                    //Starts an instance of the USB gamepad object


//***MICROCONTROLLER AND PERIPHERAL CONFIGURATION***//
// Function   : setup 
// 
// Description: This function handles the initialization of variables, pins, methods, libraries. This function only runs once at powerup or reset.
// 
// Parameters :  void
// 
// Return     : void
//*********************************//
void setup()
{
  // Begin HID gamepad 
  gamepad.begin();

  Serial.begin(115200);
  //while (!TinyUSBDevice.mounted())
  //while (!Serial) { delay(10); }

  delay(1000);                                                  //TO BE REMOVED 
  
  initMemory();                                                 //Initialize Memory 

  initLed();                                                    //Initialize LED Feedback 

  initSipAndPuff();                                             //Initialize Sip And Puff 

  initInput();                                                  //Initialize input buttons and input switches 

  initJoystick();                                               //Initialize Joystick 

  initOperationMode();                                          //Initialize operation Mode

  initDebug();                                                  //Initialize Debug Mode operation 

  startupFeedback();                                            //Startup IBM LED Feedback 

  //Configure poll timer to perform each feature as a separate loop
  pollTimerId[0] = pollTimer.setInterval(CONF_JOYSTICK_POLL_RATE, 0, joystickLoop);
  pollTimerId[1] = pollTimer.setInterval(CONF_PRESSURE_POLL_RATE, 0, pressureLoop);
  pollTimerId[2] = pollTimer.setInterval(CONF_INPUT_POLL_RATE, 0, inputLoop);
  pollTimerId[3] = pollTimer.setInterval(CONF_DEBUG_POLL_RATE, 0, debugLoop);
  
  enablePoll(false);                              //Enable it when the led IBM effect is complete 

} //end setup


//***START OF MAIN LOOP***//
// Function   : loop 
// 
// Description: This function loops consecutively and responses to changes.
// 
// Parameters :  void
// 
// Return     : void
//*********************************//
void loop()
{
  ledStateTimer.run();
  calibTimer.run();
  actionTimer.run();
  pollTimer.run();
  settingsEnabled=serialSettings(settingsEnabled); //Check to see if setting option is enabled in Lipsync
}

//***ENABLE POLL FUNCTION***//
// Function   : enablePoll 
// 
// Description: This function enables polling by enabling poll timers. It can be used to disable main features 
//              during initialization.
//
// Parameters : isEnabled : bool : enable polling if it's True
// 
// Return     : void 
//****************************************//
void enablePoll(bool isEnabled){
  if(isEnabled){
    getDebugMode(false,false);
    pollTimer.enable(1); 
    pollTimer.enable(2);    
  } else {
    pollTimer.disable(0);    
    pollTimer.disable(1);
    pollTimer.disable(2);
    pollTimer.disable(3);  
  }
}

//*********************************//
// Memory Functions
//*********************************//

//***INITIALIZE MEMORY FUNCTION***//
// Function   : initMemory 
// 
// Description: This function initializes flash memory to store settings 
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void initMemory()
{
  mem.begin();                                                      //Begin memory 
  //mem.format();                                                   //DON'T UNCOMMENT
  mem.initialize(CONF_SETTINGS_FILE, CONF_SETTINGS_JSON);           //Initialize flash memory to store settings 
}

//***RESET MEMORY FUNCTION***//
// Function   : resetMemory 
// 
// Description: This function formats and removes existing text files in flash memory.
//              It initializes flash memory to store settings after formatting.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void resetMemory()
{
  mem.format();                                                    //Format and remove existing text files in flash memory 
  mem.initialize(CONF_SETTINGS_FILE, CONF_SETTINGS_JSON);          //Initialize flash memory to store settings 
}


//*********************************//
// Communication Mode Functions
//*********************************//

//***INITIALIZE OPERATION FUNCTION***//
// Function   : initOperationMode 
// 
// Description: This function initializes operation mode or configures operation mode
//              based on stored settings in the flash memory (0 = None, 1 = Digital , 2 = Analog)
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void initOperationMode()
{
  operationMode = getOperationMode(false,false);
}


//*********************************//
// Input Functions
//*********************************//


//***INITIALIZE INPUTS FUNCTION***//
// Function   : initInput 
// 
// Description: This function initializes inputs including input buttons and input switches.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void initInput()
{

  ib.begin();                                                                     //Begin input buttons    
  is.begin();                                                                     //Begin input switches  
  buttonActionSize = sizeof(buttonActionProperty) / sizeof(inputActionStruct);    //Size of total available input button actions
  switchActionSize = sizeof(switchActionProperty) / sizeof(inputActionStruct);    //Size of total available input switch actions
  buttonActionMaxTime = getActionMaxTime(buttonActionSize,buttonActionProperty);  //Maximum button action end time
  switchActionMaxTime = getActionMaxTime(switchActionSize,switchActionProperty);  //Maximum switch action end time  
}

//***INPUT LOOP FUNCTION***//
// Function   : inputLoop 
// 
// Description: This function handles input button and input switch actions.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void inputLoop()
{
  //Request new values
  ib.update();
  is.update(); 

  //Get the last state change
  buttonState = ib.getInputState();
  switchState = is.getInputState();

  //Evaluate Output Actions
  evaluateOutputAction(buttonState, buttonActionMaxTime, buttonActionSize, buttonActionProperty);

  if(operationMode == CONF_OPR_MODE_DIGITAL){
     evaluateOutputAction(switchState, switchActionMaxTime, switchActionSize, switchActionProperty);
  }
  else if(operationMode == CONF_OPR_MODE_ANALOG){
     evaluateAnalogOutputAction(switchState, switchActionMaxTime, switchActionSize, switchActionProperty);
  }
}

//*********************************//
// Sip and Puff Functions
//*********************************//


//***INITIALIZE SIP AND PUFF FUNCTION***//
// Function   : initSipAndPuff 
// 
// Description: This function initializes sip and puff as inputs.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void initSipAndPuff()
{
  ps.begin();                                                                     //Begin sip and puff 
  getPressureMode(true,false);                                                    //Get the pressure mode stored in flash memory ( 1 = Absolute , 2 = Differential )
  getPressureThreshold(true,false);                                               //Get sip and puff pressure thresholds stored in flash memory 
  sapActionSize = sizeof(sapActionProperty) / sizeof(inputActionStruct);          //Size of total available sip and puff actions
  sapActionMaxTime = getActionMaxTime(sapActionSize,sapActionProperty);           //Maximum end action time
  
}


//***GET ACTION MAX TIME FUNCTION***//
// Function   : getActionMaxTime 
// 
// Description: This function finds the maximum end action time
//
// Parameters : actionSize : int : size of available actions
//              actionProperty : const inputActionStruct : array of all possible actions
// 
// Return     : actionMaxTime : unsigned long : maximum end action time
//****************************************//
unsigned long getActionMaxTime(int actionSize,const inputActionStruct actionProperty[])
{
  unsigned long actionMaxTime = 0;
  //Loop over all possible outputs
  for (int actionIndex = 0; actionIndex < actionSize; actionIndex++)
  {
    if(actionMaxTime < actionProperty[actionIndex].inputActionEndTime){
       actionMaxTime = actionProperty[actionIndex].inputActionEndTime;
    }
  }
  return actionMaxTime;
}

//***PRESSURE LOOP FUNCTION***//
// Function   : inputLoop 
// 
// Description: This function handles pressure polling, sip and puff state evaluation.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void pressureLoop()
{

  ps.update(); //Request new pressure difference from sensor and push it to array

  pressureValues = ps.getAllPressure(); //Read the pressure object (can be last value from array, average or other algorithms)

  //Get the last state change
  sapActionState = ps.getState();

  //Output action logic
  evaluateOutputAction(sapActionState, sapActionMaxTime, sapActionSize, sapActionProperty);
}

//***RELEASE OUTPUT FUNCTION***//
// Function   : releaseOutputAction 
// 
// Description: This function handles release of mouse hold actions.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void releaseOutputAction()
{
  //Set new state of current output action 
  outputAction = CONF_ACTION_NOTHING;
  canOutputAction = true;
}

//***EVALUATE OUTPUT ACTION FUNCTION***//
// Function   : evaluateOutputAction 
// 
// Description: This function evaluates and performs output action
//
// Parameters : actionState : inputStateStruct : Current input state 
//              actionMaxEndTime : unsigned long : maximum end action time
//              actionSize : int : size of available actions
//              actionProperty : const inputActionStruct : array of all possible actions
// 
// Return     : void 
//****************************************//
void evaluateOutputAction(inputStateStruct actionState, unsigned long actionMaxEndTime, int actionSize, const inputActionStruct actionProperty[])
{
  bool canEvaluateAction = true;
  //Output action logic
  int tempActionIndex = 0;
  //Detected input release after defined time limits. 
  if (actionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      actionState.elapsedTime > actionMaxEndTime){
      //Set Led color to default 
      setLedDefault();      
  }

  //Loop over all possible outputs
  for (int actionIndex = 0; actionIndex < actionSize && canEvaluateAction && canOutputAction; actionIndex++)
  {
    //Detected input release in defined time limits. Perform output action based on action index
    if (actionState.mainState == actionProperty[actionIndex].inputActionState &&
      actionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      actionState.elapsedTime >= actionProperty[actionIndex].inputActionStartTime &&
      actionState.elapsedTime < actionProperty[actionIndex].inputActionEndTime)
    {
      //Get action index 
      tempActionIndex = actionProperty[actionIndex].inputActionNumber;

      //Set Led color to default 
      setLedDefault();
      //Set Led state 
      setLedState(ledActionProperty[tempActionIndex].ledEndAction, 
      ledActionProperty[tempActionIndex].ledEndColor, 
      ledActionProperty[tempActionIndex].ledNumber, 
      CONF_INPUT_LED_BLINK, 
      CONF_INPUT_LED_DELAY, 
      CONF_LED_BRIGHTNESS);
      outputAction = tempActionIndex;

      //Perform led action 
      performLedAction(ledCurrentState);
      //Perform output action 
      performOutputAction(tempActionIndex);

      break;
    } //Detected input start in defined time limits. Perform led action based on action index
    else if (actionState.mainState == actionProperty[actionIndex].inputActionState &&
      actionState.secondaryState == INPUT_SEC_STATE_STARTED &&
      actionState.elapsedTime >= actionProperty[actionIndex].inputActionStartTime &&
      actionState.elapsedTime < actionProperty[actionIndex].inputActionEndTime)
    {
      //Get action index 
      tempActionIndex = actionProperty[actionIndex].inputActionNumber; 

      //Set Led color to default 
      setLedDefault();
      //Set Led state 
      setLedState(LED_ACTION_ON, 
      ledActionProperty[tempActionIndex].ledStartColor, 
      ledActionProperty[tempActionIndex].ledNumber, 
      0, 
      0, 
      CONF_LED_BRIGHTNESS);
      //Perform led action 
      performLedAction(ledCurrentState);

      break;
    }
  }  
}

//***EVALUATE OUTPUT ANALOG ACTION FUNCTION***//
// Function   : evaluateAnalogOutputAction 
// 
// Description: This function evaluates and performs output analog action
//
// Parameters : actionState : inputStateStruct : Current input state 
//              actionMaxEndTime : unsigned long : maximum end action time
//              actionSize : int : size of available actions
//              actionProperty : const inputActionStruct : array of all possible actions
// 
// Return     : void 
//****************************************//
void evaluateAnalogOutputAction(inputStateStruct actionState, unsigned long actionMaxEndTime, int actionSize, const inputActionStruct actionProperty[])
{
  bool canEvaluateAction = true;
  //Output action logic
  int tempActionIndex = 0;

  //Loop over all possible outputs
  for (int actionIndex = 0; actionIndex < actionSize && canEvaluateAction && canOutputAction; actionIndex++)
  {
    //Detected input release in defined time limits. Perform output action based on action index
    if (actionState.mainState == actionProperty[actionIndex].inputActionState &&
      actionState.secondaryState == INPUT_SEC_STATE_STARTED)
    {
      //Get action index 
      tempActionIndex = actionProperty[actionIndex].inputActionNumber;

      //Set Led color to default 
      setLedDefault();
      //Set Led state 
      setLedState(LED_ACTION_ON, 
      ledActionProperty[tempActionIndex].ledEndColor, 
      ledActionProperty[tempActionIndex].ledNumber, 
      0, 
      0, 
      CONF_LED_BRIGHTNESS);
      outputAction = tempActionIndex;

      //Perform led action 
      performLedAction(ledCurrentState);
      //Perform output action 
      performOutputAction(tempActionIndex);

      break;
    } //Detected input start in defined time limits. Perform led action based on action index
    else if (actionState.mainState == actionProperty[actionIndex].inputActionState &&
      actionState.secondaryState == INPUT_SEC_STATE_RELEASED)
    {
      //Get action index 
      tempActionIndex = actionProperty[actionIndex].inputActionNumber; 

      //Set Led color to default 
      setLedDefault();
      //Perform led action 
      setLedState(LED_ACTION_OFF, 
      LED_CLR_NONE, 
      ledActionProperty[tempActionIndex].ledNumber, 
      0, 
      0, 
      CONF_LED_BRIGHTNESS);
      //Perform led action 
      performLedAction(ledCurrentState);
      //Perform output action 
      performOutputAction(CONF_ACTION_NOTHING);

      break;
    }    
  }  
}

//***PERFORM OUTPUT ACTION FUNCTION***//
// Function   : performOutputAction 
// 
// Description: This function performs output action
//
// Parameters : action : int : action index number 
// 
// Return     : void 
//****************************************//
void performOutputAction(int action)
{
  
  switch (action)
  {
    case CONF_ACTION_NOTHING:
    {
      //actionTimerId[0] = actionTimer.setTimeout(CONF_BUTTON_PRESS_DELAY, gamepadButtonRelease, (int *)action); 
      gamepadButtonReleaseAll();
      break;
    }
    case CONF_ACTION_B1_PRESS:
    {
      gamepadButtonPress(action);
      break;
    }
    case CONF_ACTION_B2_PRESS:
    {
      gamepadButtonPress(action);
      break;
    }
    case CONF_ACTION_B3_PRESS:
    {
      gamepadButtonPress(action);
      break;
    }
    case CONF_ACTION_B4_PRESS:
    {
      gamepadButtonPress(action);
      break;
    }
    case CONF_ACTION_B5_PRESS:
    {
      gamepadButtonPress(action);
      break;
    }
    case CONF_ACTION_B6_PRESS:
    {
      gamepadButtonPress(action);
      break;
    }
    case CONF_ACTION_B7_PRESS:
    {
      gamepadButtonPress(action);
      break;
    }
    case CONF_ACTION_B8_PRESS:
    {
      gamepadButtonPress(action);
      break;
    }
    case CONF_ACTION_JOYSTICK_CENTER:
    {
      //Perform cursor center
      setJoystickInitialization(true,false);
      break;
    }
    case CONF_ACTION_JOYSTICK_CALIBRATION:
    {
      setJoystickCalibration(true,false);
      break;
    }
    case CONF_ACTION_DEC_SPEED:
    {
      //Decrease cursor speed
      decreaseJoystickSpeed(true,false);
      break;
    }
    case CONF_ACTION_INC_SPEED:
    {
      //Increase cursor speed
      increaseJoystickSpeed(true,false);
      break;
    }
    case CONF_ACTION_CHANGE_MODE:
    {
      //Change operation mode
      toggleOperationMode(true,false);
      break;
    }
    case CONF_ACTION_FACTORY_RESET:
    {
      //Perform Factory Reset
      factoryReset(true,false);
      break;
    }
  }
  if(operationMode == CONF_OPR_MODE_DIGITAL){
    actionTimerId[0] = actionTimer.setTimeout(CONF_BUTTON_PRESS_DELAY, gamepadButtonRelease, (int *)action); 
    outputAction=CONF_ACTION_NOTHING;   
  }
  canOutputAction = true;
}

//***GAMEPAD BUTTON PRESS FUNCTION***//
// Function   : gamepadButtonPress 
// 
// Description: This function performs button press action.
//
// Parameters : int : buttonNumber
// 
// Return     : void 
//****************************************//
void gamepadButtonPress(int buttonNumber)
{
  //Serial.println("Button Press");
  if (buttonNumber >0 && buttonNumber <=8 )
  {
    gamepad.press(buttonNumber-1);
    gamepad.send();
  }
}

//***GAMEPAD BUTTON CLICK FUNCTION***//
// Function   : gamepadButtonClick
// 
// Description: This function performs button click action.
//
// Parameters : int : buttonNumber
// 
// Return     : void 
//****************************************//
void gamepadButtonClick(int buttonNumber)
{
  //Serial.println("Button click");
  if (buttonNumber >0 && buttonNumber <=8 )
  {
    gamepad.press(buttonNumber-1);
    gamepad.send();
    actionTimerId[0] = actionTimer.setTimeout(CONF_BUTTON_PRESS_DELAY, gamepadButtonRelease, (int *)buttonNumber); 
  }

}

//***GAMEPAD BUTTON RELEASE FUNCTION***//
// Function   : gamepadButtonRelease 
// 
// Description: This function performs button release action.
//
// Parameters : int* : args
// 
// Return     : void 
//****************************************//
void gamepadButtonRelease(int* args)
{
  int buttonNumber = (int)args;
  //Serial.println("Button Release");
  if (buttonNumber >0 && buttonNumber <=8){
    gamepad.release(buttonNumber-1);
    gamepad.send();
  }

}



//***GAMEPAD BUTTON RELEASE FUNCTION***//
// Function   : gamepadButtonReleaseAll 
// 
// Description: This function performs button release action.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void gamepadButtonReleaseAll()
{
  //Serial.println("Button Release");
  gamepad.releaseAll();
  gamepad.send();
}



//*********************************//
// Joystick Functions
//*********************************//

//***INITIALIZE JOYSTICK FUNCTION***//
// Function   : initJoystick 
// 
// Description: This function initializes joystick as input.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void initJoystick()
{
  js.begin();                                                               //Begin joystick 
  js.setMagnetDirection(JOY_DIRECTION_DEFAULT,JOY_DIRECTION_INVERSE);       //Set x and y magnet direction 
  getJoystickDeadZone(true,false);                                          //Get joystick deadzone stored in flash memory 
  js.setOutputMode(JOY_OUTPUT_GAMEPAD_MODE);                                //Set output mode to gamepad
  getJoystickSpeed(true,false);                                             //Get joystick cursor speed stored in flash memory 
  setJoystickInitialization(true,false);                                    //Perform joystick center initialization
  getJoystickCalibration(true,false);                                       //Get joystick calibration points stored in flash memory 
}

//***PERFORM JOYSTICK CENTER FUNCTION***//
// Function   : performJoystickCenter 
// 
// Description: This function performs joystick center point initialization.
//
// Parameters : * args : int : pointer of step number 
// 
// Return     : void 
//****************************************//
void performJoystickCenter(int* args)
{
  int stepNumber = (int)args;
  unsigned long readingDuration = CONF_JOY_INIT_READING_DELAY*CONF_JOY_INIT_READING_NUMBER; //Duration of the center point readings (500 seconds )
  unsigned long currentReadingStart = CONF_JOY_INIT_START_DELAY + (CONF_JOY_INIT_STEP_BLINK_DELAY*((CONF_JOY_INIT_STEP_BLINK*2)+1)); //(500 + 150*3)                      //Time until start of current reading.
  unsigned long nextStepStart = currentReadingStart+readingDuration;                        //Time until start of next step. (1450 seconds )
  pointFloatType centerPoint;

  if (stepNumber == 0)  //STEP 0: Joystick Compensation Center Point
  {
    if(ledActionEnabled){
      setLedState(LED_ACTION_BLINK, CONF_JOY_INIT_STEP_BLINK_COLOR, CONF_JOY_INIT_LED_NUMBER, CONF_JOY_INIT_STEP_BLINK, CONF_JOY_INIT_STEP_BLINK_DELAY,CONF_LED_BRIGHTNESS);    
      performLedAction(ledCurrentState);                                                                  // LED Feedback to show start of performJoystickCalibrationStep
    }
    //Start timer to get 5 reading every 100ms
    calibTimerId[1] = calibTimer.setTimer(CONF_JOY_INIT_READING_DELAY, currentReadingStart, CONF_JOY_INIT_READING_NUMBER, performJoystickCenterStep, (int *)stepNumber);
    ++stepNumber;                                  
    ///Start exit step                                                     
    calibTimerId[0] = calibTimer.setTimeout(nextStepStart, performJoystickCenter, (int *)stepNumber);      
  } else
  {
    js.evaluateInputCenter();                                                      //Evaluate the center point using values in the buffer 
    js.setMinimumRadius();                                                         //Update minimum radius of operation            
    centerPoint = js.getInputCenter();                                             //Get the new center for API output  
    printResponseFloatPoint(true,true,true,0,"IN,1",true,centerPoint);
    calibTimer.deleteTimer(0);                                                     //Delete timer
    setLedDefault();                                                               //Set default led feedback 
    canOutputAction = true;
  }

}

//***PERFORM JOYSTICK CENTER STEP FUNCTION***//
// Function   : performJoystickCenterStep 
// 
// Description: This function performs the actual joystick center point initialization step.
//
// Parameters : * args : int : pointer of step number 
// 
// Return     : void 
//****************************************//
void performJoystickCenterStep(int* args)
{
  //Turn on and set the second led to orange to indicate start of the process 
  if(calibTimer.getNumRuns(1)==1 && ledActionEnabled){                                                                    //Turn Led's ON when timer is running for first time
    setLedState(LED_ACTION_ON, CONF_JOY_INIT_LED_COLOR, CONF_JOY_INIT_LED_NUMBER, 0, 0,CONF_LED_BRIGHTNESS);
    performLedAction(ledCurrentState);   
  }
  //Push new center values to be evaluated at the end of the process 
  js.updateInputCenterBuffer();

  //Turn off the second led to orange to indicate end of the process 
  if(calibTimer.getNumRuns(1)==CONF_JOY_INIT_READING_NUMBER && ledActionEnabled){                                        //Turn Led's OFF when timer is running for last time
    setLedState(LED_ACTION_OFF, LED_CLR_NONE, CONF_JOY_INIT_LED_NUMBER, 0, 0,CONF_LED_BRIGHTNESS);                           
    performLedAction(ledCurrentState);      
  }
}

//***PERFORM JOYSTICK CALIBRATION FUNCTION***//
// Function   : performJoystickCalibration 
// 
// Description: This function performs joystick maximum point calibration in a recursive fashion.
//
// Parameters : * args : int : pointer of step number 
// 
// Return     : void 
//****************************************//
void performJoystickCalibration(int* args)
{
  int stepNumber = (int)args;
  unsigned long readingDuration = CONF_JOY_CALIB_READING_DELAY*CONF_JOY_CALIB_READING_NUMBER; //Duration of the max corner reading ( 2 seconds )
  unsigned long currentReadingStart = CONF_JOY_CALIB_STEP_DELAY + (CONF_JOY_CALIB_STEP_BLINK_DELAY*((CONF_JOY_CALIB_STEP_BLINK*2)+1));                             //Time until start of current reading
  //Time until start of current reading. (1.5 + (3*300) seconds )
  unsigned long nextStepStart = currentReadingStart+readingDuration+CONF_JOY_CALIB_START_DELAY; //Time until start of next reading. ( 2.4 + 2 + 1 seconds )

  if (stepNumber == 0)  //STEP 0: Calibration started
  {
    pollTimer.disable(0);                                                                                             //Disable joystick data polling 
    setLedState(LED_ACTION_BLINK, CONF_JOY_CALIB_START_LED_COLOR, CONF_JOY_CALIB_LED_NUMBER, CONF_JOY_CALIB_STEP_BLINK, CONF_JOY_CALIB_STEP_BLINK_DELAY,CONF_LED_BRIGHTNESS);    
    performLedAction(ledCurrentState);   
    ++stepNumber;
    calibTimerId[0] = calibTimer.setTimeout(currentReadingStart, performJoystickCalibration, (int *)stepNumber);      // Start next step
  }
  else if (stepNumber < 5) //STEP 1-4: Joystick Calibration Corner Points 
  {
    setLedState(LED_ACTION_BLINK, CONF_JOY_CALIB_STEP_BLINK_COLOR, CONF_JOY_CALIB_LED_NUMBER, CONF_JOY_CALIB_STEP_BLINK, CONF_JOY_CALIB_STEP_BLINK_DELAY,CONF_LED_BRIGHTNESS);    
    performLedAction(ledCurrentState);                                                                  // LED Feedback to show start of performJoystickCalibrationStep
    js.zeroInputMax(stepNumber);                                                                        //Clear the existing calibration value 

    calibTimerId[1] = calibTimer.setTimer(CONF_JOY_CALIB_READING_DELAY, currentReadingStart, CONF_JOY_CALIB_READING_NUMBER, performJoystickCalibrationStep, (int *)stepNumber);
    ++stepNumber;                                                                                                               //Set LED's feedback to show step is already started and get the max reading for the quadrant/step
    calibTimerId[0] = calibTimer.setTimeout(nextStepStart, performJoystickCalibration, (int *)stepNumber);                      //Start next step
  } 
  else if (stepNumber == 5) //STEP 5 : Joystick center point initialization
  {
    setJoystickInitialization(false, false); 
    ++stepNumber; 
    calibTimerId[0] = calibTimer.setTimeout(nextStepStart, performJoystickCalibration, (int *)stepNumber);                      //Start next step  
  }
  else //STEP 6: Calibration ended
  {
    setLedState(LED_ACTION_BLINK, CONF_JOY_CALIB_START_LED_COLOR, CONF_JOY_CALIB_LED_NUMBER, CONF_JOY_CALIB_STEP_BLINK, CONF_JOY_CALIB_STEP_BLINK_DELAY,CONF_LED_BRIGHTNESS);                          //Turn off Led's
    performLedAction(ledCurrentState);
    js.setMinimumRadius();                                                                                                      //Update the minimum cursor operating radius 
    setLedDefault();
    canOutputAction = true;
    pollTimer.enable(0);                                                                                                        //Enable joystick data polling 
  }

}
//***PERFORM JOYSTICK CALIBRATION STEP FUNCTION***//
// Function   : performJoystickCalibrationStep 
// 
// Description: This function performs the actual joystick maximum point calibration step in a recursive fashion.
//
// Parameters : * args : int : pointer of step number 
// 
// Return     : void 
//****************************************//
void performJoystickCalibrationStep(int* args)
{
  int stepNumber = (int)args;
  String stepKey = "CA"+String(stepNumber);                                                            //Key to write new calibration point to Flash memory 
  String stepCommand = "CA,"+String(stepNumber);                                                       //Command to output calibration point via serial
  pointFloatType maxPoint;

  //Turn on and set the all leds to orange to indicate start of the process 
  if(calibTimer.getNumRuns(0)==1){                                                                    //Turn Led's ON when timer is running for first time
    setLedState(LED_ACTION_ON, CONF_JOY_CALIB_LED_COLOR, CONF_JOY_CALIB_LED_NUMBER, 0, 0,CONF_LED_BRIGHTNESS);
    performLedAction(ledCurrentState);   
  }
  
  maxPoint=js.getInputMax(stepNumber);                                                                //Get maximum x and y for the step number 

  //Turn off all the leds to orange to indicate end of the process 
  if(calibTimer.getNumRuns(0)==CONF_JOY_CALIB_READING_NUMBER){                                        //Turn Led's OFF when timer is running for last time
    mem.writePoint(CONF_SETTINGS_FILE,stepKey,maxPoint);                                          //Store the point in Flash Memory 
    setLedState(LED_ACTION_OFF, LED_CLR_NONE, CONF_JOY_CALIB_LED_NUMBER, 0, 0,CONF_LED_BRIGHTNESS);                           
    performLedAction(ledCurrentState);      
    printResponseFloatPoint(true,true,true,0,stepCommand,true,maxPoint);
  }
}

//***JOYSTICK LOOP FUNCTION***//
// Function   : joystickLoop 
// 
// Description: This function handles joystick move and scroll actions.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void joystickLoop()
{

  js.update();                                                        //Request new values

  pointIntType joyOutPoint = js.getXYOut();                           //Read the filtered values

  performJoystick(joyOutPoint);                                       //Perform joystick move action
}

//***PERFORM JOYSTICK FUNCTION***//
// Function   : performJoystick 
// 
// Description: This function performs joystick move
//
// Parameters : inputPoint : pointIntType : The output gamepad x and y
// 
// Return     : void 
//****************************************//
void performJoystick(pointIntType inputPoint)
{
  gamepad.move(inputPoint.x, -inputPoint.y);
  gamepad.send();
}


//*********************************//
// Debug Functions
//*********************************//

//***INITIALIZE DEBUG FUNCTION***//
// Function   : initDebug 
// 
// Description: This function initializes debug mode and debug state.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void initDebug()
{
  debugMode = getDebugMode(false, false);                                   //Get debug mode number stored in flash memory 
  setDebugState(debugMode);                                                 //Set debug operation state based on the debug mode
}

//***DEBUG LOOP FUNCTION***//
// Function   : debugLoop 
// 
// Description: This function outputs debug data based on debug mode.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void debugLoop(){
  //Debug mode is off if the debug mode is #0
  if(debugMode==CONF_DEBUG_MODE_JOYSTICK){                   //Debug #1
    js.update(); //Request new values from joystick class
    pointFloatType debugJoystickArray[3];
    debugJoystickArray[0] = js.getXYRaw();                                                          //Read the raw values
    debugJoystickArray[1] = {(float)js.getXYIn().x,(float)js.getXYIn().y};                          //Read the filtered values
    debugJoystickArray[2] = {(float)js.getXYOut().x,(float)js.getXYOut().y};                        //Read the output values
    printResponseFloatPointArray(true,true,true,0,"DEBUG,1",true,"", 3, ',', debugJoystickArray);    
  }
  else if(debugMode==CONF_DEBUG_MODE_PRESSURE){              //Debug #2
    //ps.update(); //Request new pressure difference from sensor and push it to array
    float debugPressureArray[3];
    debugPressureArray[0] = ps.getMainPressure();  //Read the main pressure 
    debugPressureArray[1] = ps.getRefPressure();   //Read the ref pressure
    debugPressureArray[2] = ps.getDiffPressure();  //Read the diff pressure
    printResponseFloatArray(true,true,true,0,"DEBUG,2",true,"", 3, ',', debugPressureArray);    
  }
  else if(debugMode==CONF_DEBUG_MODE_BUTTON){                //Debug #3
    int debugButtonArray[3];
    debugButtonArray[0] = buttonState.mainState;             //Read the main state 
    debugButtonArray[1] = buttonState.secondaryState;        //Read the secondary state
    debugButtonArray[2] = (int) buttonState.elapsedTime;     //Read the Elapsed Time     
    printResponseIntArray(true,true,true,0,"DEBUG,3",true,"", 3, ',', debugButtonArray);    
  }
  else if(debugMode==CONF_DEBUG_MODE_SWITCH){                //Debug #4
    int debugSwitchArray[3];
    debugSwitchArray[0] = switchState.mainState;             //Read the main state 
    debugSwitchArray[1] = switchState.secondaryState;        //Read the secondary state 
    debugSwitchArray[2] = (int) switchState.elapsedTime;     //Read the Elapsed Time    
    printResponseIntArray(true,true,true,0,"DEBUG,4",true,"", 3, ',', debugSwitchArray);    
  }
  else if(debugMode==CONF_DEBUG_MODE_SAP){                   //Debug #5
    int debugSapArray[3];
    debugSapArray[0] = sapActionState.mainState;             //Read the main state 
    debugSapArray[1] = sapActionState.secondaryState;        //Read the secondary state 
    debugSapArray[2] = (int) sapActionState.elapsedTime;     //Read the Elapsed Time    
    printResponseIntArray(true,true,true,0,"DEBUG,5",true,"", 3, ',', debugSapArray);    
  }
}

//***SET DEBUG STATE FUNCTION***//
// Function   : setDebugState 
// 
// Description: This function configures the state of operation based on the debug mode
//
// Parameters : inputDebugMode : int : The current debug mode
// 
// Return     : void 
//****************************************//
void setDebugState(int inputDebugMode) {
  if (inputDebugMode==CONF_DEBUG_MODE_NONE) {
    pollTimer.enable(0);                      //Enable joystick data polling 
    pollTimer.disable(3);                     //Disable debug data polling 
  } 
  else if (inputDebugMode==CONF_DEBUG_MODE_JOYSTICK) {
    pollTimer.disable(0);                     //Disable joystick data polling 
    pollTimer.enable(3);                      //Enable debug data polling 
  }
  else {
    pollTimer.enable(3);                      //Enable debug data polling 
  }
}


//*********************************//
// LED Functions
//*********************************//

//***INITIALIZE LED FUNCTION***//
// Function   : initLed 
// 
// Description: This function initializes LED output as feedback method.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void initLed()
{
  led.begin();
  *ledCurrentState = { 0, 0, 0, 0, 0, 0 };
}

//***STARTUP INITIALIZATION LED FUNCTION***//
// Function   : startupFeedback 
// 
// Description: This function performs the startup initialization feedback.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void startupFeedback()
{
  setLedState(LED_ACTION_BLINK, 1, 4, 4, CONF_STARTUP_LED_STEP_TIME, CONF_LED_BRIGHTNESS);
  ledTimerId[0] = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime, ledIBMEffect, ledCurrentState);

}

//***SET LED STATE FUNCTION***//
// Function   : setLedState 
// 
// Description: This function sets LED states based on led number, blink number, color number, blink delay , and brightness.
//
// Parameters : ledAction      : int : led action ( NOTHING , ON , OFF , BLINK )
//              ledColorNumber : int : led color number 
//              ledNumber      : int : led number ( 1, 2, 3, 4 or all )
//              ledBlinkNumber : int : led blink number 
//              ledBlinkTime : unsigned long : led blink delay time 
//              ledBrightness : int : led brightness
// 
// Return     : void 
//****************************************//
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

//***LED IBM EFFECT FUNCTION***//
// Function   : ledIBMEffect 
// 
// Description: This function performs the IBM LED effect based on step number.
//
// Parameters : args : ledStateStruct* : It includes step number.
// 
// Return     : void 
//****************************************//
void ledIBMEffect(ledStateStruct* args)
{
  if (args->ledColorNumber < 7)
  {
    led.setLedColor(args->ledNumber, args->ledColorNumber, args->ledBrightness);
    setLedState(args->ledAction, (args->ledColorNumber)+1, args->ledNumber, args->ledBlinkNumber, (args->ledBlinkTime),args->ledBrightness);
    ledTimerId[1] = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime, ledIBMEffect, ledCurrentState);
  } 
  else if (args->ledColorNumber == 7)
  {
    setLedDefault();
    ledActionEnabled = true;
    enablePoll(true);
  }

}

//***LED BLINK EFFECT FUNCTION***//
// Function   : ledBlinkEffect 
// 
// Description: This function performs the blink LED effect based on step number, led number passed by arguments.
//
// Parameters : args : ledStateStruct* : It includes step number, led number.
// 
// Return     : void 
//****************************************//
void ledBlinkEffect(ledStateStruct* args){
  if(ledStateTimer.getNumRuns(0) % 2){
     led.setLedColor(args->ledNumber, 0, args->ledBrightness);
  } 
  else{
    led.setLedColor(args->ledNumber, args->ledColorNumber, args->ledBrightness);
  }

  if(ledStateTimer.getNumRuns(0)==((args->ledBlinkNumber)*2)+1){
    
     setLedDefault();
  }   
}


//***TURN ALL LEDS OFF FUNCTION***//
// Function   : turnLedAllOff 
// 
// Description: This function turns off all the LEDs.
//
// Parameters : void
// 
// Return     : void 
//****************************************//
void turnLedAllOff()
{
  led.clearLedAll();
}

//***TURN LED OFF FUNCTION***//
// Function   : turnLedOff 
// 
// Description: This function turns off the specified LED based on led number passed by arguments.
//
// Parameters : args : ledStateStruct* : It includes led number.
// 
// Return     : void 
//****************************************//
void turnLedOff(ledStateStruct* args)
{
  led.clearLed(args->ledNumber);
}

//***TURN LED ON FUNCTION***//
// Function   : turnLedOff 
// 
// Description: This function turns on the specified LED based on led number passed by arguments.
//
// Parameters : args : ledStateStruct* : It includes led number.
// 
// Return     : void 
//****************************************//
void turnLedOn(ledStateStruct* args)
{
  led.setLedColor(args->ledNumber, args->ledColorNumber, args->ledBrightness);
}


//***BLINK LED FUNCTION***//
// Function   : blinkLed 
// 
// Description: This function blinks the specified LED based on led number and number of blinks passed by arguments.
//
// Parameters : args : ledStateStruct* : It includes led number and number of blinks.
// 
// Return     : void 
//****************************************//
void blinkLed(ledStateStruct* args)
{
  ledTimerId[2] = ledStateTimer.setTimer(args->ledBlinkTime, 0, ((args->ledBlinkNumber)*2)+1, ledBlinkEffect, ledCurrentState);
}

//***SET DEFAULT LED EFFECT FUNCTION***//
// Function   : setLedDefault 
// 
// Description: This function sets the LED to default operating color based on led number and color passed by arguments.
//
// Parameters : args : ledStateStruct* : It includes led number and color.
// 
// Return     : void 
//****************************************//
void setLedDefault(){
  //Clear 
  led.clearLedAll();
}


//***PERFORM LED ACTION FUNCTION***//
// Function   : performLedAction 
// 
// Description: This function performs LED actions based on led number, blink number, color number, blink delay , and brightness.
//
// Parameters : args : ledStateStruct* : It includes led number, blink number, color number, blink delay , and brightness.
// 
// Return     : void 
//****************************************//
void performLedAction(ledStateStruct* args)
{
  switch (args->ledAction)                                            //Get led action 
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
