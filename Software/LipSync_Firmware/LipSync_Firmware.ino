/* 
* File: LipSync_Firmware.ino
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: Beta (09 January 2024)
  License: GPL v3.0 or later

  Copyright (C) 2023 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>
*/


#include <Wire.h>
#include "LSUtils.h"
#include "LSConfig.h"
#include "LSTimer.h"
#include <ArduinoJson.h>
#include "LSOutput.h"
#include "LSUSB.h"
#include "LSBLE.h"
#include "LSCircularBuffer.h"
#include "LSInput.h"
#include "LSPressure.h"
#include "LSJoystick.h"
#include "LSMemory.h"
#include "LSScreen.h"
#include "LSBuzzer.h"


//Communication mode and debug mode variables
int comMode;        // 0 = None , 1 = USB , 2 = Wireless
int operatingMode;  // 0 = None, 1 = Mouse, 2 = Wireless, 3 = Gamepad, 4=Menu
int soundMode;      // 0 = None, 1 = Basic, 2 = Advanced

int debugMode;      // 0 = Debug mode is Off
                    // 1 = Joystick debug mode is On
                    // 2 = Pressure debug mode is On
                    // 3 = Buttons debug mode is On
                    // 4 = Switch debug mode is On
                    // 5 = Sip & Puff state debug mode is On

//Bluetooth connection variables
bool btIsConnected = false;

//LED module variables
bool ledActionEnabled = false;

ledStateStruct* ledCurrentState = new ledStateStruct;  //pointer to LED current state structure

//Input module variables
int buttonActionSize, switchActionSize;
unsigned long buttonActionMaxTime, switchActionMaxTime;
inputStateStruct buttonState, switchState;

int inputButtonPinArray[] = { CONF_BUTTON1_PIN, CONF_BUTTON2_PIN };
int inputSwitchPinArray[] = { CONF_SWITCH1_PIN, CONF_SWITCH2_PIN, CONF_SWITCH3_PIN };

inputStateStruct sapActionState;

int sapActionSize;
unsigned long sapActionMaxTime = 0;

// Timer related variables
int actionTimerId[1]; // 1 action timer
LSTimer<int> actionTimer;

int calibTimerId[2]; // 2 calibration timers
LSTimer<int> calibTimer;

int pollTimerId[8]; // 8 poll timers
LSTimer<void> pollTimer;

int ledTimerId[3];
LSTimer<ledStateStruct> ledStateTimer;

//Joystick module variables and structures
int xVal;
int yVal;
int acceleration = 0;
int scrollLevel = 0;

//Pressure module variables
pressureStruct pressureValues = { 0.0, 0.0, 0.0 };

int outputAction;
bool canOutputAction = true;
bool startupCenterReset = true;

bool settingsEnabled = false;  //Serial input settings command mode enabled or disabled

//Create instances of classes

LSMemory mem;

LSInput ib(inputButtonPinArray, CONF_BUTTON_NUMBER);  // Instance of input button object (tactile buttons)
LSInput is(inputSwitchPinArray, CONF_SWITCH_NUMBER);  // Create an instance of the input switch object (external switches)

LSJoystick js;  //Starts an instance of the LSJoystick object

LSPressure ps;  //Starts an instance of the LSPressure object

LSOutput led;  //Starts an instance of the LSOutput LED object

LSScreen screen;  //Create an instance of the LSScreen Object for OLED Screen

LSBuzzer buzzer;  //Create an instance of the LSBuzzer Object

LSUSBMouse usbmouse;   //Starts an instance of the USB mouse object
LSBLEMouse btmouse;    //Starts an instance of the BLE mouse object
LSUSBGamepad gamepad;  //Create an instance of the USB gamepad object



//***MICROCONTROLLER AND PERIPHERAL CONFIGURATION***//
// Function   : setup
//
// Description: This function handles the initialization of variables, pins, methods, libraries. This function only runs once at powerup or reset.
//
// Parameters :  void
//
// Return     : void
//*********************************//
void setup() {

  Serial.begin(115200);
  //while (!Serial) { delay(1); }  // Wait until serial port is opened
  
  initLed();  //Initialize LED Feedback
  ledWaitFeedback();

  initMemory();  //Initialize Memory

  beginComOpMode();  //Initialize Operating Mode, Communication Mode, and start instance of mouse or gamepad

  //initOperatingMode();                                          //Initialize Operating Mode

  //initCommunicationMode();                                      //Initialize Communication Mode

  if (USB_DEBUG) {
    Serial.print("USBDEBUG: comMode: ");
    Serial.println(comMode);
  }
  if (USB_DEBUG) {
    Serial.print("USBDEBUG: operatingMode: ");
    Serial.println(operatingMode);
  }

  initSipAndPuff();  //Initialize Sip And Puff

  initInput();  //Initialize input buttons and input switches

  initJoystick();  //Initialize Joystick

  initAcceleration();  //Initialize Cursor Acceleration

  initBuzzer();  //Initialize Buzzer

  initScreen();  //Initialize screen

  initDebug();  //Initialize Debug Mode operation

  //ledReadyFeedback();

  startupFeedback();  //Startup LED Feedback

  //Configure poll timer to perform each feature as a separate loop
  pollTimerId[CONF_TIMER_JOYSTICK] = pollTimer.setInterval(CONF_JOYSTICK_POLL_RATE, 0, joystickLoop);
  pollTimerId[CONF_TIMER_PRESSURE] = pollTimer.setInterval(CONF_PRESSURE_POLL_RATE, 0, pressureLoop);
  pollTimerId[CONF_TIMER_INPUT] = pollTimer.setInterval(CONF_INPUT_POLL_RATE, 0, inputLoop);
  pollTimerId[CONF_TIMER_BLUETOOTH] = pollTimer.setInterval(CONF_BT_FEEDBACK_POLL_RATE, 0, btFeedbackLoop);
  pollTimerId[CONF_TIMER_DEBUG] = pollTimer.setInterval(CONF_DEBUG_POLL_RATE, 0, debugLoop);
  pollTimerId[CONF_TIMER_SCROLL] = pollTimer.setInterval(CONF_JOYSTICK_POLL_RATE, CONF_SCROLL_POLL_RATE, joystickLoop);
  pollTimerId[CONF_TIMER_SCREEN] = pollTimer.setInterval(CONF_SCREEN_POLL_RATE, 0, screenLoop);

  enablePoll(true);
  ledActionEnabled = true;

  if (USB_DEBUG) { Serial.println("End setup");}

}  //end setup


//***START OF MAIN LOOP***//
// Function   : loop
//
// Description: This function loops consecutively and responses to changes.
//
// Parameters :  void
//
// Return     : void
//*********************************//
void loop() {
  ledStateTimer.run();  // Timer for lights

  calibTimer.run();  // Timer for calibration measurements

  actionTimer.run();

  pollTimer.run();  // Timer for normal joystick functions

  settingsEnabled = serialSettings(settingsEnabled);  //Check to see if setting option is enabled in Lipsync

  if (USB_DEBUG) {Serial.print("USB_DEBUG: Loop: "); Serial.println(millis());}
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
void enablePoll(bool isEnabled) {
  if (isEnabled) {
    getDebugMode(false, false);
    pollTimer.enable(CONF_TIMER_PRESSURE);
    pollTimer.enable(CONF_TIMER_INPUT);
    pollTimer.enable(CONF_TIMER_BLUETOOTH);
  } else {
    pollTimer.disable(CONF_TIMER_JOYSTICK);
    pollTimer.disable(CONF_TIMER_PRESSURE);
    pollTimer.disable(CONF_TIMER_INPUT);
    pollTimer.disable(CONF_TIMER_BLUETOOTH);
    pollTimer.disable(CONF_TIMER_DEBUG);
    pollTimer.disable(CONF_TIMER_SCROLL);
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
void initMemory() {
  mem.begin();  //Begin memory
  //mem.format();                                                   //DON'T UNCOMMENT - use a factory reset through the serial if need to wipe memory (FR,1:1)
  mem.initialize(CONF_SETTINGS_FILE, CONF_SETTINGS_JSON);  //Initialize flash memory to store settings
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
void resetMemory() {
  mem.format();                                            //Format and remove existing text files in flash memory
  mem.initialize(CONF_SETTINGS_FILE, CONF_SETTINGS_JSON);  //Initialize flash memory to store settings
}

//*********************************//
// Screen Functions
//*********************************//

//***INITIALIZE SCREEN FUNCTION***//
// Function   : initScreen
//
// Description: This function initializes the screen
//
// Parameters : void
//
// Return     : void
//****************************************//
void initScreen() {
  screen.begin();         //Begin screen
  screen.splashScreen();  //Show splash screen
}

//***CLEAR MENU SCREEN FUNCTION***//
// Function   : closeMenu
//
// Description: This function closes the menu and clears the screen
//
// Parameters : void
//
// Return     : void
//****************************************//
void closeMenu() {
  screen.deactivateMenu();
}

//***CLEAR SPLASH SCREEN FUNCTION***//
// Function   : clearSplashScreen
//
// Description: This function clears the splash screen
//
// Parameters : void
//
// Return     : void
//****************************************//
void clearSplashScreen() {
  //Check if menu is active, only turn off screen if menu is not open
  if (!screen.isMenuActive()) {
    screen.deactivateMenu();
  }
}

//***SHOW CENTER RESET COMPLETE PAGE SCREEN FUNCTION***//
// Function   : showCenterResetComplete
//
// Description: This function shows the center reset complete page
//
// Parameters : void
//
// Return     : void
//****************************************//
void showCenterResetComplete() {
  //Show center reset complete page once center reset is done
  screen.centerResetCompletePage();
}


//***SCREEN LOOP FUNCTION***//
// Function   : screenLoop
//
// Description: This function handles updates to the screen.
//
// Parameters : void
//
// Return     : void
//****************************************//
void screenLoop() {

  if (USB_DEBUG) { Serial.println("USBDEBUG: screenLoop"); }
  //Request update
  screen.update();

  if (USB_DEBUG) { Serial.println("USBDEBUG: end of screenLoop"); }
}

//*********************************//
// Buzzer Functions
//*********************************//

//***INITIALIZE BUZZER FUNCTION***//
// Function   : initBuzzer
//
// Description: This function initializes the Buzzer as feedback method.
//
// Parameters : void
//
// Return     : void
//****************************************//
void initBuzzer() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: Initializing Buzzer"); }
  buzzer.begin();
  //buzzer.startup();         // moved to be called in center calibration function to ensure it happens after center calibration is complete
}

//***BUZZER LOOP FUNCTION***//
// Function   : buzzerLoop
//
// Description: This function handles updates to the buzzer.
//
// Parameters : void
//
// Return     : void
//****************************************//
void buzzerLoop() {

  //if (USB_DEBUG) { Serial.println("USBDEBUG: buzzerLoop");  }
  //Request update
  buzzer.update();
}

//***BUZZER SOUND ON FUNCTION***//
// Function   : buzzerSoundOn
//
// Description: This function turns on the sound from the buzzer by calling the buzzer enable function.
//
// Parameters : void
//
// Return     : void
//****************************************//
void buzzerSoundOn() {
  buzzer.enable();
}


//***BUZZER SOUND OFF FUNCTION***//
// Function   : buzzerSoundOff
//
// Description: This function turns off the sound from the buzzer.
//
// Parameters : void
//
// Return     : void
//****************************************//
void buzzerSoundOff() {
  buzzer.disable();
}


//*********************************//
// Acceleration Functions
//*********************************//

//***INITIALIZE ACCELERATION FUNCTION***//
// Function   : initAcceleration
//
// Description: This function initializes acceletaion
//
// Parameters : void
//
// Return     : void
//****************************************//
void initAcceleration() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: Initializing Acceleration"); }
  //  acceleration=getJoystickAcceleration(false,false);
}

//*********************************//
// Communication Mode Functions
//*********************************//

//***INITIALIZE COMMUNICATION FUNCTION***//
// Function   : initCommunicationMode
//
// Description: This function initializes communication mode or configures communication mode
//              based on stored settings in the flash memory (0 = None , 1 = USB , 2 = Wireless)
//
// Parameters : void
//
// Return     : void
//****************************************//
void initCommunicationMode() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: Initializing Communication Mode"); }
  comMode = getCommunicationMode(false, false);
}


//*********************************//
// Operating Mode Functions
//*********************************//



//***INITIALIZE OPERATING MODE FUNCTION***//
// Function   : initOperatingMode
//
// Description: This function initializes the operating mode based on the
//              based on stored settings in the flash memory (0 = None , 1 = USB Mouse , 2 = Wireless Mouse, 3 = USB Gamepad)
//
// Parameters : void
//
// Return     : void
//****************************************//
void initOperatingMode() {

  //operatingMode = getOperatingMode(false,false); // retrieve operating mode from memory
  operatingMode = mem.readInt(CONF_SETTINGS_FILE, "OM");

  /*
   if (operatingMode==CONF_OPERATING_MODE_MOUSE) {
    usbmouse.begin();    
  } 
  else if (operatingMode==CONF_OPERATING_MODE_GAMEPAD) {
    gamepad.begin();
  }
  else if (operatingMode==CONF_OPERATING_MODE_BTMOUSE) {
    btmouse.begin();
  }
  else
  {
    
  }
  */
}



//***CHANGE OPERATING MODE FUNCTION***//
// Function   : changeOperatingMode
//
// Description: This function configures the state of operation based on the current and desired operating mode
//
// Parameters : inputOperatingMode : int : The operating mode to change to
//
// Return     : void
//****************************************//
void changeOperatingMode(int inputOperatingState) {
  if (inputOperatingState == operatingMode) {
    //do nothing
  } else {
    softwareReset();
  }
  operatingMode = inputOperatingState;
}

//***INITIALIZE OPERATING MODE FUNCTION***//
// Function   : beginComOpMode                  //TODO: rename this?
//
// Description: This function calls functions to initialize communication mode and operating mode
//              and begins instance of either USB mouse, Bluetooth mouse, or USB Gamepad
//
// Parameters : void
//
// Return     : void
//****************************************//
void beginComOpMode() {

  initCommunicationMode();
  initOperatingMode();

  switch (operatingMode) {
    case CONF_OPERATING_MODE_MOUSE:
      switch (comMode) {
        case CONF_COM_MODE_USB:  // USB Mouse
          usbmouse.begin();
          break;
        case CONF_COM_MODE_BLE:  // Bluetooth Mouse
          btmouse.begin();
          break;
      }
      break;
    case CONF_OPERATING_MODE_GAMEPAD:  //USB Gamepad
      gamepad.begin();
      break;
      //default:
      //TODO: error handling?
  }
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
void initInput() {
  //if (USB_DEBUG) {    Serial.println("USBDEBUG: Initializing Input");  }
  ib.begin();                                                                      //Begin input buttons
  is.begin();                                                                      //Begin input switches
  buttonActionSize = sizeof(buttonActionProperty) / sizeof(inputActionStruct);     //Size of total available input button actions
  switchActionSize = sizeof(switchActionProperty) / sizeof(inputActionStruct);     //Size of total available input switch actions
  buttonActionMaxTime = getActionMaxTime(buttonActionSize, buttonActionProperty);  //Maximum button action end time
  switchActionMaxTime = getActionMaxTime(switchActionSize, switchActionProperty);  //Maximum switch action end time
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
void inputLoop() {

  if (USB_DEBUG) { Serial.println("USBDEBUG: inputLoop"); }
  //Read new values
  ib.update();  // update buttons
  is.update();  //update external assistive switch inputs

  //Get the last state change
  buttonState = ib.getInputState();
  switchState = is.getInputState();

  if (USB_DEBUG) { Serial.println("USBDEBUG: got input states"); }

  //Evaluate Output Actions
  evaluateOutputAction(buttonState, buttonActionMaxTime, buttonActionSize, buttonActionProperty);
  evaluateOutputAction(switchState, switchActionMaxTime, switchActionSize, switchActionProperty);


  if (USB_DEBUG) { Serial.println("USBDEBUG: End of inputLoop"); }
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
void initSipAndPuff() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: Initializing Sip and Puff"); }
  ps.begin();                                                             //Begin sip and puff
  getPressureMode(true, false);                                           //Get the pressure mode stored in flash memory ( 1 = Absolute , 2 = Differential )
  getPressureThreshold(true, false);                                      //Get sip and puff pressure thresholds stored in flash memory
  sapActionSize = sizeof(sapActionProperty) / sizeof(inputActionStruct);  //Size of total available sip and puff actions
  sapActionMaxTime = getActionMaxTime(sapActionSize, sapActionProperty);  //Maximum end action time
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
unsigned long getActionMaxTime(int actionSize, const inputActionStruct actionProperty[]) {
  unsigned long actionMaxTime = 0;
  //Loop over all possible outputs
  for (int actionIndex = 0; actionIndex < actionSize; actionIndex++) {
    if (actionMaxTime < actionProperty[actionIndex].inputActionEndTime) {
      actionMaxTime = actionProperty[actionIndex].inputActionEndTime;
    }
  }
  return actionMaxTime;
}

//***PRESSURE LOOP FUNCTION***//
// Function   : pressureLoop
//
// Description: This function handles pressure polling, sip and puff state evaluation.
//
// Parameters : void
//
// Return     : void
//****************************************//
void pressureLoop() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: pressureLoop()"); }
  ps.update();  //Request new pressure difference from sensor and push it to array

  pressureValues = ps.getAllPressure();  //Read the pressure object (can be last value from array, average or other algorithms)

  //Get the last state change
  sapActionState = ps.getState();

  //Output action logic
  evaluateOutputAction(sapActionState, sapActionMaxTime, sapActionSize, sapActionProperty);
}

//***RELEASE OUTPUT FUNCTION***//
// Function   : releaseOutputAction
//
// Description: This function handles release of usbmouse hold actions.
//
// Parameters : void
//
// Return     : void
//****************************************//
void releaseOutputAction() {
  //Release left click if it's in drag mode and left mouse button is pressed.
  if (outputAction == CONF_ACTION_DRAG && (usbmouse.isPressed(MOUSE_LEFT) || btmouse.isPressed(MOUSE_LEFT))) {
    usbmouse.release(MOUSE_LEFT);
    btmouse.release(MOUSE_LEFT);
  }
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
void evaluateOutputAction(inputStateStruct actionState, unsigned long actionMaxEndTime, int actionSize, const inputActionStruct actionProperty[]) {
  bool canEvaluateAction = true;

  //Output action logic
  int tempActionIndex = 0;

  //Handle input action when it's in hold state
  if ((actionState.secondaryState == INPUT_SEC_STATE_RELEASED) && (outputAction == CONF_ACTION_SCROLL || outputAction == CONF_ACTION_DRAG)) {
    setLedDefault();  //Set default led feedback
    //Set new state of current output action
    releaseOutputAction();
    canEvaluateAction = false;
  }  //Detected input release after defined time limits.
  else if (actionState.secondaryState == INPUT_SEC_STATE_RELEASED && actionState.elapsedTime > actionMaxEndTime) {
    //Set Led color to default
    setLedDefault();
  }

  // Code to switch between joystick controlled scroll and joystick controlled cursor movement
  if (outputAction == CONF_ACTION_SCROLL) {
    pollTimer.enable(CONF_TIMER_SCROLL);
    pollTimer.disable(CONF_TIMER_JOYSTICK);
  } else if (outputAction != CONF_ACTION_SCROLL) {
    pollTimer.enable(CONF_TIMER_JOYSTICK);
    pollTimer.disable(CONF_TIMER_SCROLL);
  }

  //Loop over all possible outputs
  for (int actionIndex = 0; actionIndex < actionSize && canEvaluateAction && canOutputAction; actionIndex++) {
    //Detected input release in defined time limits. Perform output action based on action index
    if (actionState.mainState == actionProperty[actionIndex].inputActionState && actionState.secondaryState == INPUT_SEC_STATE_RELEASED && actionState.elapsedTime >= actionProperty[actionIndex].inputActionStartTime && actionState.elapsedTime < actionProperty[actionIndex].inputActionEndTime) {
      //Get action index
      if (screen.isMenuActive()) {
        tempActionIndex = actionProperty[actionIndex].menuOutputActionNumber;
      } else {
        switch (operatingMode) {
          case CONF_OPERATING_MODE_MOUSE:
            tempActionIndex = actionProperty[actionIndex].mouseOutputActionNumber;
            break;
          case CONF_OPERATING_MODE_GAMEPAD:
            tempActionIndex = actionProperty[actionIndex].gamepadOutputActionNumber;
            break;
        }
      }

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
    }  //Detected input start in defined time limits. Perform led action based on action index
    else if (actionState.mainState == actionProperty[actionIndex].inputActionState && actionState.secondaryState == INPUT_SEC_STATE_STARTED && actionState.elapsedTime >= actionProperty[actionIndex].inputActionStartTime && actionState.elapsedTime < actionProperty[actionIndex].inputActionEndTime) {
      //Get action index
      if (screen.isMenuActive()) {
        tempActionIndex = actionProperty[actionIndex].menuOutputActionNumber;
      } else {
        switch (operatingMode) {
          case CONF_OPERATING_MODE_MOUSE:
            tempActionIndex = actionProperty[actionIndex].mouseOutputActionNumber;
            break;
          case CONF_OPERATING_MODE_GAMEPAD:
            tempActionIndex = actionProperty[actionIndex].gamepadOutputActionNumber;
            break;
        }
      }

      //Set Led color to default
      setLedDefault();
      //Set Led state
      setLedState(LED_ACTION_ON,
                  ledActionProperty[tempActionIndex].ledStartColor,
                  ledActionProperty[tempActionIndex].ledNumber,
                  0,                     //number of blinks
                  0,                     //blink time
                  CONF_LED_BRIGHTNESS);  //brightness
      //Perform led action
      performLedAction(ledCurrentState);

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
void performOutputAction(int action) {
  switch (action) {
    case CONF_ACTION_NOTHING:
      {
        if (operatingMode == CONF_OPERATING_MODE_GAMEPAD) {
          gamepadButtonReleaseAll();
          Serial.println("Released all");
        }
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
        cursorScroll();  //Enter Scroll mode
        break;
      }
    case CONF_ACTION_CURSOR_CENTER:
    {
      //Perform cursor center
      screen.centerResetPage();
      //setJoystickInitialization(true,false);
      break;
    }
    case CONF_ACTION_CURSOR_CALIBRATION:
      {
        setJoystickCalibration(true, false);
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
        decreaseCursorSpeed(true, false);
        break;
      }
    case CONF_ACTION_INC_SPEED:
      {
        //Increase cursor speed
        increaseCursorSpeed(true, false);
        break;
      }
    case CONF_ACTION_CHANGE_MODE:
      {
        //Change communication mode
        toggleCommunicationMode(true, false);
        break;
      }
    case CONF_ACTION_START_MENU:
      {
        // Activate Menu
        screen.activateMenu();
        break;
      }
    case CONF_ACTION_STOP_MENU:
      {
        // Deactivate Menu
        screen.deactivateMenu();
        break;
      }
    case CONF_ACTION_B1_PRESS:
      {
        Serial.println("Button 1 press");
        gamepadButtonPress(1);
        break;
      }
    case CONF_ACTION_B2_PRESS:
      {
        gamepadButtonPress(2);
        break;
      }
    case CONF_ACTION_B3_PRESS:
      {
        gamepadButtonPress(3);
        break;
      }
    case CONF_ACTION_B4_PRESS:
      {
        gamepadButtonPress(4);
        break;
      }
    case CONF_ACTION_B5_PRESS:
      {
        gamepadButtonPress(5);
        break;
      }
    case CONF_ACTION_B6_PRESS:
      {
        gamepadButtonPress(6);
        break;
      }
    case CONF_ACTION_B7_PRESS:
      {
        gamepadButtonPress(7);
        break;
      }
    case CONF_ACTION_B8_PRESS:
      {
        gamepadButtonPress(8);
        break;
      }
    case CONF_ACTION_NEXT_MENU_ITEM:
      {
        // Move to next menu item
        screen.nextMenuItem();
        break;
      }
    case CONF_ACTION_SELECT_MENU_ITEM:
      {
        // Move to next menu item
        screen.selectMenuItem();
        break;
      }
    case CONF_ACTION_RESET:
      {
        softwareReset();
        break;
      }
    case CONF_ACTION_FACTORY_RESET:
      {
        //Perform Factory Reset
        factoryReset(true, false);
        break;
      }
  }
  if (operatingMode == CONF_OPERATING_MODE_GAMEPAD) {
    //actionTimerId[0] = actionTimer.setTimeout(CONF_BUTTON_PRESS_DELAY, gamepadButtonRelease, (int *)action);
    actionTimerId[0] = actionTimer.setTimeout(CONF_BUTTON_PRESS_DELAY, gamepadButtonReleaseAll);  //TODO: Change this to just release one
    outputAction = CONF_ACTION_NOTHING;
  }

  if (action == CONF_ACTION_DRAG || action == CONF_ACTION_SCROLL) {
    canOutputAction = false;
  } else {
    outputAction = CONF_ACTION_NOTHING;
    canOutputAction = true;
  }
}


//*********************************//
// Mouse Functions
//*********************************//

//***CURSOR LEFT CLICK FUNCTION***//
// Function   : cursorLeftClick
//
// Description: This function performs cursor left click action.
//
// Parameters : void
//
// Return     : void
//****************************************//
void cursorLeftClick(void) {
  //Serial.println("Left Click");
  if (comMode == CONF_COM_MODE_USB) {
    usbmouse.click(MOUSE_LEFT);
  } else if (comMode == CONF_COM_MODE_BLE) {
    Serial.println("Bluetooth left click");
    btmouse.click(MOUSE_LEFT);
  }
}

//***CURSOR RIGHT CLICK FUNCTION***//
// Function   : cursorRightClick
//
// Description: This function performs cursor right click action.
//
// Parameters : void
//
// Return     : void
//****************************************//
void cursorRightClick(void) {
  //Serial.println("Right Click");
  if (comMode == CONF_COM_MODE_USB) {
    usbmouse.click(MOUSE_RIGHT);
  } else if (comMode == CONF_COM_MODE_BLE) {
    btmouse.click(MOUSE_RIGHT);
  }
}

//***CURSOR MIDDLE CLICK FUNCTION***//
// Function   : cursorMiddleClick
//
// Description: This function performs cursor middle click action.
//
// Parameters : void
//
// Return     : void
//****************************************//
void cursorMiddleClick(void) {
  //Serial.println("Middle Click");
  if (comMode == CONF_COM_MODE_USB) {
    usbmouse.click(MOUSE_MIDDLE);
  } else if (comMode == CONF_COM_MODE_BLE) {
    btmouse.click(MOUSE_MIDDLE);
  }
}

//***DRAG FUNCTION***//
// Function   : cursorDrag
//
// Description: This function performs cursor drag action.
//
// Parameters : void
//
// Return     : void
//********************//
void cursorDrag(void) {
  //Serial.println("Drag");
  if (comMode == CONF_COM_MODE_USB) {
    usbmouse.press(MOUSE_LEFT);
  } else if (comMode == CONF_COM_MODE_BLE) {
    btmouse.press(MOUSE_LEFT);
  }
}
//***CURSOR SCROLL FUNCTION***//
// Function   : cursorScroll
//
// Description: This function is an operating mode that enables scrolling action.
//
// Parameters : void
//
// Return     : void
//****************************************//
void cursorScroll(void) {
  //Serial.println("Scroll");
  outputAction = CONF_ACTION_SCROLL;
}


//*********************************//
// Gamepad Functions
//*********************************//

//***GAMEPAD BUTTON PRESS FUNCTION***//
// Function   : gamepadButtonPress
//
// Description: This function performs button press action.
//
// Parameters : int : buttonNumber
//
// Return     : void
//****************************************//
void gamepadButtonPress(int buttonNumber) {
  //Serial.println("Button Press");
  if (buttonNumber > 0 && buttonNumber <= 8) {
    gamepad.press(buttonNumber - 1);
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
void gamepadButtonClick(int buttonNumber) {
  //Serial.println("Button click");
  if (buttonNumber > 0 && buttonNumber <= 8) {
    gamepad.press(buttonNumber - 1);
    gamepad.send();
    actionTimerId[0] = actionTimer.setTimeout(CONF_BUTTON_PRESS_DELAY, gamepadButtonRelease, (int*)buttonNumber);
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
void gamepadButtonRelease(int* args) {
  int buttonNumber = (int)args;
  //Serial.println("Button Release");
  if (buttonNumber > 0 && buttonNumber <= 8) {
    gamepad.release(buttonNumber - 1);
    gamepad.send();
  }
}


//***GAMEPAD BUTTON RELEASE ALL FUNCTION***//
// Function   : gamepadButtonReleaseAll
//
// Description: This function performs button release action.
//
// Parameters : void
//
// Return     : void
//****************************************//
void gamepadButtonReleaseAll() {
  //Serial.println("Button Release All");
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
void initJoystick() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: Initializing Joystick"); }
  js.begin();                                                           //Begin joystick
  js.setMagnetDirection(JOY_DIRECTION_DEFAULT, JOY_DIRECTION_INVERSE);  //Set x and y magnet direction
  getJoystickDeadZone(true, false);                                     //Get joystick deadzone stored in flash memory
  getCursorSpeed(true, false);                                          //Get joystick cursor speed stored in flash memory
  scrollLevel = getScrollLevel(true, false);                            //Get scroll level stored in flash memory
  setJoystickInitialization(true, false);                               //Perform joystick center initialization
  getJoystickCalibration(true, false);                                  //Get joystick calibration points stored in flash memory
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
void performJoystickCenter(int* args) {
  int stepNumber = (int)args;
  unsigned long readingDuration = CONF_JOY_INIT_READING_DELAY * CONF_JOY_INIT_READING_NUMBER;                                               //Duration of the center point readings (500 seconds )
  unsigned long currentReadingStart = CONF_JOY_INIT_START_DELAY + (CONF_JOY_INIT_STEP_BLINK_DELAY * ((CONF_JOY_INIT_STEP_BLINK * 2) + 1));  //(500 + 150*3)                      //Time until start of current reading.
  unsigned long nextStepStart = currentReadingStart + readingDuration;                                                                      //Time until start of next step. (1450 seconds )
  pointFloatType centerPoint;

  if (stepNumber == 0)  //STEP 0: Joystick Compensation Center Point
  {
    if (ledActionEnabled) {
      setLedState(LED_ACTION_BLINK, CONF_JOY_INIT_STEP_BLINK_COLOR, CONF_JOY_INIT_LED_NUMBER, CONF_JOY_INIT_STEP_BLINK, CONF_JOY_INIT_STEP_BLINK_DELAY, CONF_LED_BRIGHTNESS);
      performLedAction(ledCurrentState);  // LED Feedback to show start of performJoystickCalibrationStep
    }
    //Start timer to get 5 reading every 100ms
    calibTimerId[1] = calibTimer.setTimer(CONF_JOY_INIT_READING_DELAY, currentReadingStart, CONF_JOY_INIT_READING_NUMBER, performJoystickCenterStep, (int*)stepNumber);
    ++stepNumber;
    ///Start exit step
    calibTimerId[0] = calibTimer.setTimeout(nextStepStart, performJoystickCenter, (int*)stepNumber);
  } else {
    js.evaluateInputCenter();           //Evaluate the center point using values in the buffer
    js.setMinimumRadius();              //Update minimum radius of operation
    centerPoint = js.getInputCenter();  //Get the new center for API output
    printResponseFloatPoint(true, true, true, 0, "IN,1", true, centerPoint);
    calibTimer.deleteTimer(0);  //Delete timer
    setLedDefault();            //Set default led feedback
    canOutputAction = true;
    if (startupCenterReset){    // Checks variable to only play sound and show splash screen on startup
      buzzer.startup();
      screen.splashScreen2();
    }                             
    if (screen.showCenterResetComplete){screen.centerResetCompletePage();}      // Checks variable so center reset complete page only shows if accessed from menu, not on startup or during full calibration
    startupCenterReset = false;
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
void performJoystickCenterStep(int* args) {
  //Turn on and set the second led to orange to indicate start of the process
  if (calibTimer.getNumRuns(1) == 1 && ledActionEnabled) {  //Turn Led's ON when timer is running for first time
    setLedState(LED_ACTION_ON, CONF_JOY_INIT_LED_COLOR, CONF_JOY_INIT_LED_NUMBER, 0, 0, CONF_LED_BRIGHTNESS);
    performLedAction(ledCurrentState);
  }
  //Push new center values to be evaluated at the end of the process
  js.updateInputCenterBuffer();

  //Turn off the second led to orange to indicate end of the process
  if (calibTimer.getNumRuns(1) == CONF_JOY_INIT_READING_NUMBER && ledActionEnabled) {  //Turn Led's OFF when timer is running for last time
    setLedState(LED_ACTION_OFF, LED_CLR_NONE, CONF_JOY_INIT_LED_NUMBER, 0, 0, CONF_LED_BRIGHTNESS);
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
void performJoystickCalibration(int* args) {
  int stepNumber = (int)args;
  unsigned long readingDuration = CONF_JOY_CALIB_READING_DELAY * CONF_JOY_CALIB_READING_NUMBER;                                               //Duration of the max corner reading ( 2 seconds )
  unsigned long currentReadingStart = CONF_JOY_CALIB_STEP_DELAY + (CONF_JOY_CALIB_STEP_BLINK_DELAY * ((CONF_JOY_CALIB_STEP_BLINK * 2) + 1));  //Time until start of current reading
  //Time until start of current reading. (1.5 + (3*300) seconds )
  unsigned long nextStepStart = currentReadingStart + readingDuration + CONF_JOY_CALIB_START_DELAY;  //Time until start of next reading. ( 2.4 + 2 + 1 seconds )

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
    screen.fullCalibrationPrompt(stepNumber);
    setLedState(LED_ACTION_BLINK, CONF_JOY_CALIB_STEP_BLINK_COLOR, CONF_JOY_CALIB_LED_NUMBER, CONF_JOY_CALIB_STEP_BLINK, CONF_JOY_CALIB_STEP_BLINK_DELAY,CONF_LED_BRIGHTNESS);    
    performLedAction(ledCurrentState);                                                                  // LED Feedback to show start of performJoystickCalibrationStep
    js.zeroInputMax(stepNumber);                                                                        //Clear the existing calibration value 

    calibTimerId[1] = calibTimer.setTimer(CONF_JOY_CALIB_READING_DELAY, currentReadingStart, CONF_JOY_CALIB_READING_NUMBER, performJoystickCalibrationStep, (int *)stepNumber);
    ++stepNumber;                                                                                                               //Set LED's feedback to show step is already started and get the max reading for the quadrant/step
    calibTimerId[0] = calibTimer.setTimeout(nextStepStart, performJoystickCalibration, (int *)stepNumber);                      //Start next step
  } 
  else if (stepNumber == 5) //STEP 5 : Joystick center point initialization
  {
    screen.fullCalibrationPrompt(stepNumber);
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
    pollTimer.enable(CONF_TIMER_JOYSTICK);                                                                                      //Enable joystick data polling 
    pollTimer.enable(CONF_TIMER_SCROLL);                                                                                        //Enable joystick data polling 
    screen.fullCalibrationPrompt(stepNumber);
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
void performJoystickCalibrationStep(int* args) {
  int stepNumber = (int)args;
  String stepKey = "CA" + String(stepNumber);       //Key to write new calibration point to Flash memory
  String stepCommand = "CA," + String(stepNumber);  //Command to output calibration point via serial
  pointFloatType maxPoint;

  //Turn on and set all leds to orange to indicate start of the process //TODO Jake update to non-neopixel LEDS
  if (calibTimer.getNumRuns(0) == 1) {  //Turn Led's ON when timer is running for first time
    setLedState(LED_ACTION_ON, CONF_JOY_CALIB_LED_COLOR, CONF_JOY_CALIB_LED_NUMBER, 0, 0, CONF_LED_BRIGHTNESS);
    performLedAction(ledCurrentState);
  }

  maxPoint = js.getInputMax(stepNumber);  //Get maximum x and y for the step number

  //Turn off all the leds to orange to indicate end of the process
  if (calibTimer.getNumRuns(0) == CONF_JOY_CALIB_READING_NUMBER) {  //Turn Led's OFF when timer is running for last time
    mem.writePoint(CONF_SETTINGS_FILE, stepKey, maxPoint);          //Store the point in Flash Memory
    setLedState(LED_ACTION_OFF, LED_CLR_NONE, CONF_JOY_CALIB_LED_NUMBER, 0, 0, CONF_LED_BRIGHTNESS);
    performLedAction(ledCurrentState);
    printResponseFloatPoint(true, true, true, 0, stepCommand, true, maxPoint);
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
void joystickLoop() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: joystickLoop"); }
  js.update();  //Request new values

  pointIntType joyOutPoint = js.getXYOut();  //Read the filtered values

  performJoystick(joyOutPoint);  //Perform joystick move action

  //if (USB_DEBUG) { Serial.println("USBDEBUG: End of joystickLoop");  }
}

//***PERFORM JOYSTICK FUNCTION***//
// Function   : performJoystick
//
// Description: This function performs joystick move and scroll actions.
//
// Parameters : inputPoint : pointIntType : The output cursor x and y
//
// Return     : void
//****************************************//
void performJoystick(pointIntType inputPoint) {
  if (operatingMode == CONF_OPERATING_MODE_MOUSE) {
    //0 = None , 1 = USB , 2 = Wireless
    if (comMode == CONF_COM_MODE_USB) {
      //(outputAction == CONF_ACTION_SCROLL) ? usbmouse.scroll(scrollModifier(round(inputPoint.y),js.getMinimumRadius(),scrollLevel)) : usbmouse.move(accelerationModifier(round(inputPoint.x),js.getMinimumRadius(),acceleration), accelerationModifier(round(-inputPoint.y),js.getMinimumRadius(),acceleration));
      (outputAction == CONF_ACTION_SCROLL) ? usbmouse.scroll(scrollModifier(round(inputPoint.y), js.getMinimumRadius(), scrollLevel)) : usbmouse.move(inputPoint.x, inputPoint.y);

    } else if (comMode == CONF_COM_MODE_BLE) {
      //(outputAction == CONF_ACTION_SCROLL) ? btmouse.scroll(scrollModifier(round(inputPoint.y),js.getMinimumRadius(),scrollLevel)) : btmouse.move(accelerationModifier(round(inputPoint.x),js.getMinimumRadius(),acceleration), accelerationModifier(round(-inputPoint.y),js.getMinimumRadius(),acceleration));
      (outputAction == CONF_ACTION_SCROLL) ? btmouse.scroll(scrollModifier(round(inputPoint.y), js.getMinimumRadius(), scrollLevel)) : btmouse.move(inputPoint.x, inputPoint.y);
    }
  } else if (operatingMode == CONF_OPERATING_MODE_GAMEPAD) {
    //Gamepad is USB only, if wireless gamepad functionality is added, add that here
    //Serial.print("X: \t"); Serial.print(inputPoint.x); Serial.print("\t Y: \t"); Serial.println(inputPoint.y);
    gamepad.move(inputPoint.x, inputPoint.y);
    gamepad.send();
  }
}

//***FSR SCROLL MOVEMENT MODIFIER FUNCTION***//
// Function   : scrollModifier
//
// Description: This function converts y cursor movements to y scroll movements based on y cursor value and scroll speed level.
//
// Parameters : cursorValue : const int : y cursor value.
//              cursorMaxValue : const int : maximum y cursor value.
//              scrollLevelValue : const int : scroll speed level value.
//
// Return     : cursorOutput : int : The modified scroll value.
//****************************************//
int scrollModifier(const int cursorValue, const int cursorMaxValue, const int scrollLevelValue) {
  int scrollOutput = 0;
  int scrollMaxSpeed = round((1.0 * pow(CONF_SCROLL_MOVE_MAX, scrollLevelValue / 10.0)) + CONF_SCROLL_MOVE_BASE);

  scrollOutput = map(cursorValue, 0, cursorMaxValue, 0, scrollMaxSpeed);
  scrollOutput = -1 * constrain(scrollOutput, -1 * scrollMaxSpeed, scrollMaxSpeed);
  return scrollOutput;
}


//***ACCELERATION MOVEMENT MODIFIER FUNCTION***//
// Function   : accelerationModifier
//
// Description: This function converts y cursor movements to y scroll movements based on y cursor value and acceleration level.
//
// Parameters : cursorValue : const int : y cursor value.
//              cursorMaxValue : const int : maximum y cursor value.
//              accelerationValue : const int : acceleration level value.
//
// Return     : cursorOutput : int : The modified acceleration value.
//****************************************//
int accelerationModifier(const int cursorValue, const int cursorMaxValue, const int accelerationValue) {
  int accelerationOutput = 0;

  if (accelerationOutput < 0) {
    accelerationOutput = round((1.0 * pow(cursorValue, 1.0 + (acceleration * 0.025))));
  } else if (accelerationOutput > 0) {
    accelerationOutput = round(1.0 * pow(cursorValue, -1.0 * (1.0 + (acceleration * 0.025))));
  }
  accelerationOutput = -1 * constrain(accelerationOutput, -1 * cursorMaxValue, cursorMaxValue);
  return accelerationOutput;
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
void initDebug() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: Initializing Debug"); }
  debugMode = getDebugMode(false, false);  //Get debug mode number stored in flash memory
  setDebugState(debugMode);                //Set debug operation state based on the debug mode
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
void debugLoop() {


  if (USB_DEBUG) { Serial.println("USBDEBUG: debugLoop"); }
  //Debug mode is off if the debug mode is #0
  if (debugMode == CONF_DEBUG_MODE_JOYSTICK) {  //Debug #1
    js.update();                                //Request new values from joystick class
    pointFloatType debugJoystickArray[3];
    debugJoystickArray[0] = js.getXYRaw();                                       //Read the raw values
    debugJoystickArray[1] = { (float)js.getXYIn().x, (float)js.getXYIn().y };    //Read the filtered values
    debugJoystickArray[2] = { (float)js.getXYOut().x, (float)js.getXYOut().y };  //Read the output values
    printResponseFloatPointArray(true, true, true, 0, "DEBUG,1", true, "", 3, ',', debugJoystickArray);
  } else if (debugMode == CONF_DEBUG_MODE_PRESSURE) {  //Debug #2
    //ps.update(); //Request new pressure difference from sensor and push it to array
    float debugPressureArray[3];
    debugPressureArray[0] = ps.getSapPressureAbs();   //Read the main pressure
    debugPressureArray[1] = ps.getAmbientPressure();  //Read the ref pressure
    debugPressureArray[2] = ps.getSapPressure();      //Read the diff pressure
    printResponseFloatArray(true, true, true, 0, "DEBUG,2", true, "", 3, ',', debugPressureArray);
  } else if (debugMode == CONF_DEBUG_MODE_BUTTON) {  //Debug #3
    int debugButtonArray[3];
    debugButtonArray[0] = buttonState.mainState;         //Read the main state
    debugButtonArray[1] = buttonState.secondaryState;    //Read the secondary state
    debugButtonArray[2] = (int)buttonState.elapsedTime;  //Read the Elapsed Time
    printResponseIntArray(true, true, true, 0, "DEBUG,3", true, "", 3, ',', debugButtonArray);
  } else if (debugMode == CONF_DEBUG_MODE_SWITCH) {  //Debug #4
    int debugSwitchArray[3];
    debugSwitchArray[0] = switchState.mainState;         //Read the main state
    debugSwitchArray[1] = switchState.secondaryState;    //Read the secondary state
    debugSwitchArray[2] = (int)switchState.elapsedTime;  //Read the Elapsed Time
    printResponseIntArray(true, true, true, 0, "DEBUG,4", true, "", 3, ',', debugSwitchArray);
  } else if (debugMode == CONF_DEBUG_MODE_SAP) {  //Debug #5
    int debugSapArray[3];
    debugSapArray[0] = sapActionState.mainState;         //Read the main state
    debugSapArray[1] = sapActionState.secondaryState;    //Read the secondary state
    debugSapArray[2] = (int)sapActionState.elapsedTime;  //Read the Elapsed Time
    printResponseIntArray(true, true, true, 0, "DEBUG,5", true, "", 3, ',', debugSapArray);
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
  if (inputDebugMode == CONF_DEBUG_MODE_NONE) {
    pollTimer.enable(CONF_TIMER_JOYSTICK);  //Enable joystick data polling
    pollTimer.disable(CONF_TIMER_DEBUG);    //Disable debug data polling
    pollTimer.enable(CONF_TIMER_SCROLL);    //Enable scroll data polling

  } else if (inputDebugMode == CONF_DEBUG_MODE_JOYSTICK) {
    pollTimer.disable(CONF_TIMER_JOYSTICK);  //Disable joystick data polling
    pollTimer.disable(CONF_TIMER_SCROLL);    //Disable scroll data polling
    pollTimer.enable(CONF_TIMER_DEBUG);      //Enable debug data polling
  } else {
    pollTimer.enable(CONF_TIMER_DEBUG);  //Enable debug data polling
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
void initLed() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: Initializing LEDs"); }
  led.begin();
  *ledCurrentState = { 0, 0, 0, 0, 0, 0 };  //Initialize pointer ledAction, ledColorNumber, ledBlinkNumber, ledBlinkTime, ledBrightness
  turnLedAllOff();
}


//***LED WAIT FEEDBACK FUNCTION***//
// Function   : ledWaitFeedback
//
// Description: This function sets the LEDS to cue the user to wait for initialization.
//
// Parameters : void
//
// Return     : void
//****************************************//
void ledWaitFeedback() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: All LEDS on"); }
  setLedState(LED_ACTION_ON, LED_CLR_RED, CONF_LED_ALL, 0, 0, CONF_LED_BRIGHTNESS);  // Turn on all LEDS
  performLedAction(ledCurrentState);
}

//***LED WAIT FEEDBACK FUNCTION***//
// Function   : ledWaitFeedback
//
// Description: This function sets the LEDS to cue the user to wait for initialization.
//
// Parameters : void
//
// Return     : void
//****************************************//
void ledReadyFeedback() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: ledReadyFeedback"); }
  setLedState(LED_ACTION_OFF, LED_CLR_NONE, CONF_LED_ALL, 0, 0, CONF_LED_BRIGHTNESS);    // Turn off all LEDS
  setLedState(LED_ACTION_ON, LED_CLR_GREEN, CONF_LED_MICRO, 0, 0, CONF_LED_BRIGHTNESS);  // Turn micro LED green
  performLedAction(ledCurrentState);
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
void startupFeedback() {
  if (USB_DEBUG) { Serial.println("USBDEBUG: startupFeedback"); }
  // if (USB_DEBUG)   {    Serial.println("USBDEBUG: All LEDS on");  }
  //setLedState(LED_ACTION_BLINK, LED_CLR_RED, CONF_LED_ALL, 4, CONF_STARTUP_LED_STEP_TIME, CONF_LED_BRIGHTNESS); // Blink all LEDs 4 times
  //performLedAction(ledCurrentState);
  //if (USB_DEBUG)   {    Serial.println("USBDEBUG: All LEDS off");  }
  //setLedState(LED_ACTION_NONE, LED_CLR_NONE, CONF_LED_ALL, 0, 0, CONF_LED_BRIGHTNESS); // Turn all LEDs off
  //performLedAction(ledCurrentState);

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
void setLedState(int ledAction, int ledColorNumber, int ledNumber, int ledBlinkNumber, unsigned long ledBlinkTime, int ledBrightness) {  //Set led state after output action is performed
  if (ledNumber <= OUTPUT_LED_NUM + 1) {

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
void ledIBMEffect(ledStateStruct* args) {
  if (args->ledColorNumber < 7) {
    led.setLedColor(args->ledNumber, args->ledColorNumber, args->ledBrightness);
    setLedState(args->ledAction, (args->ledColorNumber) + 1, args->ledNumber, args->ledBlinkNumber, (args->ledBlinkTime), args->ledBrightness);
    ledTimerId[1] = ledStateTimer.setTimeout(ledCurrentState->ledBlinkTime, ledIBMEffect, ledCurrentState);
  } else if (args->ledColorNumber == 7) {
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
void ledBlinkEffect(ledStateStruct* args) {
  if (ledStateTimer.getNumRuns(0) % 2) {
    led.setLedColor(args->ledNumber, 0, args->ledBrightness);
  } else {
    led.setLedColor(args->ledNumber, args->ledColorNumber, args->ledBrightness);
  }

  if (ledStateTimer.getNumRuns(0) == ((args->ledBlinkNumber) * 2) + 1) {
    //
    setLedDefault();
  }
}

//***LED BLUETOOTH SCAN EFFECT FUNCTION***//
// Function   : ledBtScanEffect
//
// Description: This function performs bluetooth scan blink LED effect.
//
// Return     : void
//****************************************//
void ledBtScanEffect() {
  if (pollTimer.getNumRuns(5) % 2) {
    led.setLedColor(CONF_BT_LED_NUMBER, LED_CLR_NONE, CONF_BT_LED_BRIGHTNESS);
  } else {
    led.setLedColor(CONF_BT_LED_NUMBER, CONF_BT_LED_COLOR, CONF_BT_LED_BRIGHTNESS);
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
void turnLedAllOff() {
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
void turnLedOff(ledStateStruct* args) {
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
//void turnLedOn(ledStateStruct* args)
//{
//  led.setLedColor(args->ledNumber, args->ledColorNumber, args->ledBrightness);
//}


//***BLINK LED FUNCTION***//
// Function   : blinkLed
//
// Description: This function blinks the specified LED based on led number and number of blinks passed by arguments.
//
// Parameters : args : ledStateStruct* : It includes led number and number of blinks.
//
// Return     : void
//****************************************//
void blinkLed(ledStateStruct* args) {
  int blinkOffset = 0;  // number of milliseconds to delay start of blink
  ledTimerId[2] = ledStateTimer.setTimer(args->ledBlinkTime, blinkOffset, ((args->ledBlinkNumber) * 2) + 1, ledBlinkEffect, ledCurrentState);
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
void setLedDefault() {
  //Clear if it's in USB MODE

  led.clearLedAll();

  switch (operatingMode) {
    case CONF_OPERATING_MODE_MOUSE:
      {
        led.setLedColor(CONF_LED_MICRO, LED_CLR_PURPLE, CONF_LED_BRIGHTNESS);
        if (comMode == CONF_COM_MODE_BLE && btmouse.isConnected()) {  //Set micro LED to blue if it's in BLE MODE
          led.setLedColor(CONF_BT_LED_NUMBER, LED_CLR_BLUE, CONF_LED_BRIGHTNESS);
        }
        
        break;
      }
    case CONF_OPERATING_MODE_GAMEPAD:
      {
        led.setLedColor(CONF_LED_MICRO, LED_CLR_YELLOW, CONF_LED_BRIGHTNESS);
        break;
      }
  }
}

//***BLUETOOTH SCAN AND LED FEEDBACK LOOP FUNCTION***//
// Function   : btFeedbackLoop
//
// Description: This function performs the default LED effects to indicate the device is connected.
//
// Parameters : void
//
// Return     : void
//****************************************//
void btFeedbackLoop() {

  if (USB_DEBUG) { Serial.println("USBDEBUG: btFeedbackLoop"); }

  //Get the current bluetooth connection state
  bool tempIsConnected = btmouse.isConnected();

  if (USB_DEBUG) { Serial.println(tempIsConnected); }
  //Perform bluetooth LED blinking if bluetooth is not connected and wasn't connected before
  if (comMode == CONF_COM_MODE_BLE && tempIsConnected == false && tempIsConnected == btIsConnected) {
    btIsConnected = false;
    pollTimerId[5] = pollTimer.setTimer(CONF_BT_SCAN_BLINK_DELAY, 0, ((CONF_BT_SCAN_BLINK_NUMBER*2)+1), ledBtScanEffect); 

  }  //Set the default LED effect if bluetooth connection state is changed
  else if (comMode == CONF_COM_MODE_BLE && tempIsConnected != btIsConnected) {
    btIsConnected = tempIsConnected;
    setLedDefault();
  }

  if (USB_DEBUG) { Serial.println("USBDEBUG: end of btFeedbackLoop"); }
}

//***PERFORM LED ACTION FUNCTION***//
// Function   : performLedAction
//
// Description: This function performs LED actions based on led number, blink number, color number, blink delay, and brightness.
//
// Parameters : args : ledStateStruct* : It includes LED number, blink number, color number, blink delay, and brightness.
//
// Return     : void
//****************************************//
void performLedAction(ledStateStruct* args) {
  switch (args->ledAction)  //Get LED action
  {
    case LED_ACTION_NONE:
      {
        setLedDefault();
        break;
      }
    case LED_ACTION_OFF:
      {
        //turnLedOff(args);
        break;
      }
    case LED_ACTION_ON:
      {
        //turnLedOn(args);
        break;
      }
    case LED_ACTION_BLINK:
      {
        blinkLed(args);
        break;
      }
      /*
    case LED_ACTION_BLINKFAST:
    {
      blinkLedFast(args);
      break;
    }
    */
  }
}


//***SOFTWARE RESET FUNCTION***//
// Function   : softwareReset
//
// Description: This function initiates a software reset.
//
// Parameters :  none
//
// Return     : none
//******************************************//
void softwareReset() {
  releaseOutputAction();
  usbmouse.end();
  gamepad.end();
  btmouse.end();

  NVIC_SystemReset();
  delay(10);
}

void printlnToSerial(String toPrint) {
  Serial.println(toPrint);
}

void printToSerial(String toPrint) {
  Serial.print(toPrint);
}
