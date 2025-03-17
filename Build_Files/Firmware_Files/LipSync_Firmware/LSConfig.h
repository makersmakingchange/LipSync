/* 
* File: LSConfig.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: v4.1rc (10 March 2025)
  License: GPL v3.0 or later

  Copyright (C) 2024 - 2025 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>

*/

#include "LSUtils.h"

// Debugging
#define USB_DEBUG  0 // Set this to 0 for best performance
#define SHOW_CONNECTION_TIME 0 // Set to 0 for production

#define CONF_ENABLE_WATCHDOG 1 // Set to 1 to enable hardware watchdog

//***DO NOT CHANGE***//
#define CONF_LIPSYNC_MODEL 1                // 1: Lipsync w/ Hub

#define CONF_LIPSYNC_VERSION_MAJOR  4
#define CONF_LIPSYNC_VERSION_MINOR  1
#define CONF_LIPSYNC_VERSION_REV  0

const String lipsyncVersionStr = String(CONF_LIPSYNC_VERSION_MAJOR) + "." + String(CONF_LIPSYNC_VERSION_MINOR) + "." + String(CONF_LIPSYNC_VERSION_REV);

// I2C Devices
#define I2CADDR_DISPLAY 0x3D  // Display address (61)
#define I2CADDR_LPS22   0x5C  // Modified LPS22 address (92)
#define I2CADDR_LPS35HW 0x5D  // LPS35HW address (93)
#define I2CADDR_TLV493D 0x5E  // 3D Hall Effect Sensor (94)


// Input pins
#define CONF_BUTTON1_PIN 9                  // Pushbutton S1
#define CONF_BUTTON2_PIN 3                  // Pushbutton S2
#define CONF_SWITCH1_PIN A0                 // 3.5 mm jack SW1
#define CONF_SWITCH2_PIN A1                 // 3.5 mm jack SW2
#define CONF_SWITCH3_PIN A2                 // 3.5 mm jack SW3

// Input buttons and switch values 
#define CONF_BUTTON_NUMBER 2
#define CONF_SWITCH_NUMBER 3

// Output LEDS
#define CONF_LED_LEFT_PIN 7                 // left
#define CONF_LED_MIDDLE_PIN 8               // middle
#define CONF_LED_RIGHT_PIN 6                // right

// Onboard LEDs
#define CONF_LED_MICRO_RED LED_RED          // Xiao NRF52840 User LED Red
#define CONF_LED_MICRO_GREEN LED_GREEN      // Xiao NRF52840 User LED Green
#define CONF_LED_MICRO_BLUE LED_BLUE        // Xiao NRF52840 User LED Blue

// Output Buzzer
#define CONF_BUZZER_PIN 10

// Operating Mode Values
#define CONF_OPERATING_MODE_MIN 0
#define CONF_OPERATING_MODE_NONE 0
#define CONF_OPERATING_MODE_MOUSE 1
#define CONF_OPERATING_MODE_GAMEPAD 2
#define CONF_OPERATING_MODE_SAFE 3
#define CONF_OPERATING_MODE_MAX 3

#define CONF_OPERATING_MODE_DEFAULT CONF_OPERATING_MODE_MOUSE  // Default mode = Mouse
                                          // 0 = Operating Mode null
                                          // 1 = Mouse
                                          // 2 = Gamepad  

// Communication mode values 
#define CONF_COM_MODE_MIN  0
#define CONF_COM_MODE_NONE 0 
#define CONF_COM_MODE_USB  1
#define CONF_COM_MODE_BLE  2
#define CONF_COM_MODE_MAX  2

#define CONF_COM_MODE_DEFAULT CONF_COM_MODE_USB

#define CONF_LED_NONE 0
#define CONF_LED_LEFT 1
#define CONF_LED_MIDDLE 2
#define CONF_LED_RIGHT 3
#define CONF_LED_MICRO 4
#define CONF_LED_ALL 5

#define CONF_LED_BRIGHTNESS 150

// LED color codes
#define LED_CLR_NONE 0
#define LED_CLR_BLUE 1
#define LED_CLR_PURPLE 2
#define LED_CLR_MAGENTA 3
#define LED_CLR_PINK 4
#define LED_CLR_RED 5
#define LED_CLR_ORANGE 6
#define LED_CLR_YELLOW 7
#define LED_CLR_GREEN 8
#define LED_CLR_TEAL 9
#define LED_CLR_WHITE 10

#define LED_ACTION_NONE 0
#define LED_ACTION_OFF 1
#define LED_ACTION_ON 2
#define LED_ACTION_BLINK 3
#define LED_ACTION_BLINKFAST 4

// Display
#define CONF_DISPLAY_ENABLED false

// Button Input States
#define INPUT_MAIN_STATE_NONE           0
#define INPUT_MAIN_STATE_S1_PRESSED     1
#define INPUT_MAIN_STATE_S2_PRESSED     2
#define INPUT_MAIN_STATE_S12_PRESSED    3
#define INPUT_MAIN_STATE_S3_PRESSED     4
#define INPUT_MAIN_STATE_S13_PRESSED    5
#define INPUT_MAIN_STATE_S23_PRESSED    6
#define INPUT_MAIN_STATE_S123_PRESSED   7


// Output action numbers
#define CONF_ACTION_NOTHING 0              // No action
#define CONF_ACTION_LEFT_CLICK 1           // Generates a short left click
#define CONF_ACTION_RIGHT_CLICK 2          // Generates a short right click
#define CONF_ACTION_MIDDLE_CLICK 3         // Generates a short middle click
#define CONF_ACTION_DRAG 4                 // Initiates drag mode, holding down left click until cancelled
#define CONF_ACTION_SCROLL 5               // Initiates scroll mode. Vertical motion generates mouse scroll wheel movement.
#define CONF_ACTION_B1_PRESS 6             // Generates a Button 1 press or button X1(Left USB)/View(Right USB) press in XAC  
#define CONF_ACTION_B2_PRESS 7             // Generates a Button 2 press or button X2(Left USB)/Menu(Right USB) press in XAC    
#define CONF_ACTION_B3_PRESS 8             // Generates a Button 3 press or button LS(Left USB)/RS(Right USB) press in XAC
#define CONF_ACTION_B4_PRESS 9             // Generates a Button 4 press or button LB(Left USB)/RB(Right USB) press in XAC 
#define CONF_ACTION_B5_PRESS 10            // Generates a Button 5 press or button A(Left USB)/X(Right USB) press in XAC
#define CONF_ACTION_B6_PRESS 11            // Generates a Button 6 press or button B(Left USB)/Y(Right USB) press in XAC
#define CONF_ACTION_B7_PRESS 12            // Generates a Button 7 press or button View(Left USB)/X1(Right USB) press in XAC  
#define CONF_ACTION_B8_PRESS 13            // Generates a Button 8 press or button Menu(Left USB)/X2(Right USB) press in XAC 
#define CONF_ACTION_CURSOR_CENTER 14       // Center the joystick 
#define CONF_ACTION_CURSOR_CALIBRATION 15  // Initiates the joystick calibration.
#define CONF_ACTION_DEC_SPEED 16           // Decrease cursor movement speed
#define CONF_ACTION_INC_SPEED 17           // Increase cursor movement speed
#define CONF_ACTION_CHANGE_MODE 18         // Change communication mode
#define CONF_ACTION_START_MENU 19          // Start menu on display
#define CONF_ACTION_STOP_MENU 20           // Close menu on display
#define CONF_ACTION_NEXT_MENU_ITEM 21      // Move to next item in menu
#define CONF_ACTION_SELECT_MENU_ITEM 22    // Select current item in menu 
#define CONF_ACTION_RESET 23               // Software Reset
#define CONF_ACTION_FACTORY_RESET 24       // Factory Reset


// Flash Memory settings - Don't change  
#define CONF_SETTINGS_FILE    "/settings.txt"
#define CONF_SETTINGS_JSON    "{\"MN\":0,\"VN1\":4,\"VN2\":1,\"VN3\":0,\"ID\":0,\"OM\":1,\"CM\":1,\"SS\":5,\"SL\":5,\"PM\":2,\"ST\":3.0,\"PT\":3.0,\"AV\":0,\"IZ\":0.05,\"OZ\":0.95,\"CA0\":[0.0,0.0],\"CA1\":[-13.0,13.0],\"CA2\":[13.0,13.0],\"CA3\":[13.0,-13.0],\"CA4\":[-13.0,-13.0],\"SM\":1,\"LM\":1,\"LL\":5,\"DM\":0}"

// Polling rates for each module
#define CONF_JOYSTICK_POLL_RATE 20          // 20 ms 
#define CONF_SCROLL_POLL_RATE 30            // 30 ms
#define CONF_PRESSURE_POLL_RATE 50          // 50 ms
#define CONF_INPUT_POLL_RATE 20             // 20 ms
#define CONF_BT_FEEDBACK_POLL_RATE 1000     // 1s         
#define CONF_DEBUG_POLL_RATE 100            // 100 ms
#define CONF_SCREEN_POLL_RATE 20            // 20 ms
#define CONF_USB_POLL_RATE 1000             // Check USB connection every 1 second
#define CONF_WATCHDOG_POLL_RATE 5000        // Reset watchdog timer every 5 seconds

#define CONF_BUTTON_PRESS_DELAY 150         // 150 ms - Duration of single button press in gamepad mode

#define CONF_WATCHDOG_TIMEOUT_SEC 30        // 30 second hardware watchdog


// Safe Boot Mode
#define CONF_SAFE_MODE_REASON_WATCHDOG 1
#define CONF_SAFE_MODE_REASON_INPUT 2
#define CONF_SAFE_MODE_REASON_HARDWARE 3

// Hub Menu
#define CONF_SPLASH_SCREEN_DURATION 10000   // 10 seconds - how long the splash screen stays on on startup
#define CONF_SAFEMODE_MENU_TIMEOUT 3000 // 3 seconds between screens
#define CONF_MENU_TIMEOUT  300000           // 300 seconds (5 minutes) - duration of inactivity after which the screen turns off 
#define CONF_MENU_CONTROL_MIN 0
#define CONF_MENU_CONTROL_OPEN 1
#define CONF_MENU_CONTROL_SELECT 2
#define CONF_MENU_CONTROL_NEXT 3
#define CONF_MENU_CONTROL_CLOSE 4
#define CONF_MENU_CONTROL_MAX 4


#define CONF_USB_HID_TIMEOUT  5000           // 5 seconds - timeout for connecting to USB and continuing with the program
#define CONF_USB_HID_INIT_DELAY 2000         // initial delay before attempting to reconnect to usb again

// Polling Timer IDs for each module
#define CONF_TIMER_JOYSTICK 0
#define CONF_TIMER_PRESSURE 1
#define CONF_TIMER_INPUT 2
#define CONF_TIMER_BLUETOOTH 3
#define CONF_TIMER_DEBUG 4
#define CONF_TIMER_SCROLL 5
#define CONF_TIMER_SCREEN 6
#define CONF_TIMER_USB 7
#define CONF_TIMER_WATCHDOG 8

#define CONF_TIMER_LED_STARTUP 0
#define CONF_TIMER_LED_IBM 1
#define CONF_TIMER_LED_BLINK 2
#define CONF_TIMER_LED_BT 3
#define CONF_TIMER_LED_ERROR 4


// Joystick values 
#define CONF_JOY_CURSOR_SPEED_LEVEL_MIN 1
#define CONF_JOY_CURSOR_SPEED_LEVEL_MAX 10



// Debug mode values 
#define CONF_DEBUG_MODE_NONE      0 
#define CONF_DEBUG_MODE_JOYSTICK  1
#define CONF_DEBUG_MODE_PRESSURE  2
#define CONF_DEBUG_MODE_BUTTON    3
#define CONF_DEBUG_MODE_SWITCH    4
#define CONF_DEBUG_MODE_SAP       5

#define CONF_DEBUG_MODE_MIN 0
#define CONF_DEBUG_MODE_MAX 5

// Debug Default settings
#define CONF_DEBUG_MODE_DEFAULT  0        // Default debug mode state = Off 
                                          // 0 = Debug mode is Off
                                          // 1 = Joystick debug mode is On
                                          // 2 = Pressure debug mode is On
                                          // 3 = Buttons debug mode is On
                                          // 4 = Switch debug mode is On
                                          // 5 = Sip & Puff state debug mode is On


// Internal Test
#define CONF_TEST_MODE_MIN 1
#define CONF_TEST_MODE_LED 1
#define CONF_TEST_MODE_BUZZER 2
#define CONF_TEST_MODE_WATCHDOG 3
#define CONF_TEST_MODE_MAX 3


// Sound Modes                                                                                              
#define CONF_SOUND_MODE_OFF     0   // Sound off
#define CONF_SOUND_MODE_BASIC   1   // Minimal sounds
#define CONF_SOUND_MODE_ALL     2   // All sounds

#define CONF_SOUND_MODE_MIN     0
#define CONF_SOUND_MODE_MAX     2
#define CONF_SOUND_MODE_DEFAULT CONF_SOUND_MODE_BASIC


// Light Modes
#define CONF_LIGHT_MODE_OFF     0   // LEDs off
#define CONF_LIGHT_MODE_BASIC   1   // Minimal LEDs
#define CONF_LIGHT_MODE_ALL     2   // All LEDs

#define CONF_LIGHT_MODE_MIN     0
#define CONF_LIGHT_MODE_MAX     2
#define CONF_LIGHT_MODE_DEFAULT CONF_LIGHT_MODE_BASIC

// Light Brightness Levels
#define CONF_LED_BRIGHTNESS_LEVEL_DEFAULT   5
#define CONF_LED_BRIGHTNESS_LEVEL_MIN       0
#define CONF_LED_BRIGHTNESS_LEVEL_MAX       10

#define CONF_LED_BRIGHTNESS_MAX 255

//***CAN BE CHANGED***//
// API
#define CONF_API_ENABLED true               // Enable or Disable API

// Startup Default settings
#define CONF_STARTUP_LED_STEP_TIME 500      // Time for each color

// Joystick Deadzone Default settings
#define CONF_JOY_DEADZONE_MIN 0.0
#define CONF_JOY_DEADZONE_INNER_DEFAULT 0.05
#define CONF_JOY_DEADZONE_OUTER_DEFAULT 0.95
#define CONF_JOY_DEADZONE_MAX 1.0

// Joystick full calibration points and related LED feedback settings
#define CONF_JOY_CALIB_CORNER_DEFAULT 13.0
#define CONF_JOY_CALIB_START_DELAY 1000              // Number of milliseconds to delay full joystick calibration once triggered
#define CONF_JOY_CALIB_START_LED_COLOR LED_CLR_RED   // Joystick Calibration process start and end color
#define CONF_JOY_CALIB_STEP_DELAY 1500               // Number of milliseconds to delay between corner measurements
#define CONF_JOY_CALIB_LED_NUMBER CONF_LED_ALL
#define CONF_JOY_CALIB_LED_COLOR LED_CLR_RED         // LED color for joystick calibration process 
#define CONF_JOY_CALIB_STEP_BLINK 1
#define CONF_JOY_CALIB_STEP_BLINK_DELAY 150
#define CONF_JOY_CALIB_READING_DELAY 200
#define CONF_JOY_CALIB_STEP_BLINK_COLOR LED_CLR_RED  // The color indicates the joystick Calibration about to start
#define CONF_JOY_CALIB_READING_NUMBER 10             // Number of readings to measure (and then average) for each calibration point  

#define CONF_JOY_CALIB_ERROR 10  // flag to display message stating there was an error with one or more corner calibrations
#define CONF_JOY_CALIB_CORNER_MIN 3  // Minimum value for a corner coordinate when completing full calibration. Less than this will be set to default.

// Joystick center initialization and related LED feedback settings 
#define CONF_JOY_INIT_START_DELAY 1000  // Number of milliseconds to delay joystick neutral calibration once triggered
#define CONF_JOY_INIT_LED_NUMBER CONF_LED_ALL
#define CONF_JOY_INIT_LED_COLOR LED_CLR_RED  // LED color for center initialization step
#define CONF_JOY_INIT_STEP_BLINK 1
#define CONF_JOY_INIT_STEP_BLINK_DELAY 150
#define CONF_JOY_INIT_STEP_BLINK_COLOR LED_CLR_RED // LED color for Center initialization start blink color 
#define CONF_JOY_INIT_READING_DELAY 100
#define CONF_JOY_INIT_READING_NUMBER 5

// Joystick cursor speed change and related LED feedback settings 
#define CONF_JOY_CURSOR_SPEED_LEVEL_DEFAULT 5  // Default cursor speed level
#define CONF_JOY_SPEED_CHANGE_LED_DELAY 150
#define CONF_JOY_SPEED_CHANGE_LED_BLINK 1
#define CONF_JOY_SPEED_DEC_LED_NUMBER CONF_LED_LEFT // Which LED indicates decrease in cursor speed level
#define CONF_JOY_SPEED_DEC_LED_COLOR LED_CLR_RED // Decrease speed color
#define CONF_JOY_SPEED_INC_LED_NUMBER CONF_LED_RIGHT // Which LED indicates increase in cursor speed level
#define CONF_JOY_SPEED_INC_LED_COLOR LED_CLR_RED  // Increase speed color
#define CONF_JOY_SPEED_LIMIT_LED_DELAY 50
#define CONF_JOY_SPEED_LIMIT_LED_BLINK 3
#define CONF_JOY_SPEED_LIMIT_LED_COLOR LED_CLR_RED // Out of range limit speed color

#define CONF_JOY_OUTPUT_XY_MAX  1024
#define CONF_JOY_OUTPUT_XY_MAX_GAMEPAD  127

// Scroll level change and related LED feedback settings 
#define CONF_SCROLL_CHANGE_LED_DELAY  150
#define CONF_SCROLL_CHANGE_LED_BLINK  1
#define CONF_SCROLL_DEC_LED_NUMBER    CONF_LED_LEFT
#define CONF_SCROLL_DEC_LED_COLOR     LED_CLR_PURPLE  // Decrease scroll level color
#define CONF_SCROLL_INC_LED_NUMBER    CONF_LED_RIGHT
#define CONF_SCROLL_INC_LED_COLOR     LED_CLR_PURPLE  // Increase scroll level color
#define CONF_SCROLL_LIMIT_LED_DELAY   50
#define CONF_SCROLL_LIMIT_LED_BLINK   3
#define CONF_SCROLL_LIMIT_LED_COLOR   LED_CLR_RED // Out of range limit scroll level color

// Scroll level values 
#define CONF_SCROLL_LEVEL_DEFAULT 5
#define CONF_SCROLL_LEVEL_MIN 1
#define CONF_SCROLL_LEVEL_MAX 10
#define CONF_SCROLL_MOVE_MIN  1
#define CONF_SCROLL_MOVE_MAX  16
#define CONF_SCROLL_MOVE_BASE 1

// Joystick cursor acceleration change 
#define CONF_JOY_ACCELERATION_LEVEL_MAX 10
#define CONF_JOY_ACCELERATION_LEVEL_MIN -10
#define CONF_JOY_ACCELERATION_LEVEL_DEFAULT 0


// Sip and Puff Default settings
#define CONF_SIP_THRESHOLD 3.0                    // hPa
#define CONF_PUFF_THRESHOLD 3.0                   // hPa
#define CONF_PRESS_MIN_THRESHOLD 2.0              // hPa
#define CONF_PRESS_MAX_THRESHOLD 150.0            // hPa
#define CONF_PRESS_MODE_DEFAULT 1                 // Default pressure mode state = 2 
                                                  //  0 = None or PRESS_MODE_NONE
                                                  //  1 = Absolute or PRESS_MODE_ABS
                                                  //  2 = Differential or PRESS_MODE_DIFF
                                                  
                                                  
// Sip and puff main states 
#define PRESS_SAP_MAIN_STATE_NONE 0   // No action 
#define PRESS_SAP_MAIN_STATE_SIP 1    // Sip action sapPressure < -sip threshold
#define PRESS_SAP_MAIN_STATE_PUFF 2   // Puff action sapPressure > puff threshold

// Inputs and related LED feedback settings
#define CONF_INPUT_LED_DELAY 150          // Led blink time for input actions 
#define CONF_INPUT_LED_BLINK 1            // Led blink number  for input actions 

// Communication Mode Default settings
#define CONF_COM_MODE_LED_NUMBER  2
#define CONF_COM_MODE_LED_BLINK   1
#define CONF_COM_MODE_LED_BLINK_DELAY   1000


// Bluetooth connection and related LED feedback settings 
#define CONF_BT_SCAN_BLINK_DELAY 500
#define CONF_BT_SCAN_BLINK_NUMBER 1
#define CONF_BT_LED_NUMBER CONF_LED_MICRO // Which LED indicate Bluetooth connection status
#define CONF_BT_LED_COLOR LED_CLR_BLUE 
#define CONF_BT_LED_BRIGHTNESS CONF_LED_BRIGHTNESS

// Error codes
#define CONF_ERROR_NONE 0
#define CONF_ERROR_USB  1
#define CONF_ERROR_I2C  2

#define CONF_ERROR_LED_BLINK_DELAY 500
#define CONF_ERROR_LED_NUMBER CONF_LED_ALL // All LEDs
#define CONF_ERROR_LED_COLOR LED_CLR_RED   // red Color
#define CONF_ERROR_LED_BRIGHTNESS 255      // Full brightness

// Acceleration
// Cursor acceleration structure
const accStruct accProperty[]{
  { 0, 1.0, 0, 0 },
  { 1, 1.0, 0, 0 },
  { 2, 1.0, 0, 0 },
  { 3, 1.0, 0, 0 },
  { 4, 1.0, 0, 0 },
  { 5, 1.0, 0, 0 },
  { 6, 1.0, 0, 0 },
  { 7, 1.0, 0, 0 },
  { 8, 1.0, 0, 0 },
  { 9, 1.0, 0, 0 }
};

/* LIPSYNC INPUT AND OUTPUT MAPPING */

// Sip and Puff Action Mapping
//  {INPUT ACTION, MOUSE MODE ACTION, GAMEPAD MODE ACTION, MENU MODE ACTION, minTime, maxTime}
const inputActionStruct sapActionProperty[]{
  { PRESS_SAP_MAIN_STATE_NONE,        CONF_ACTION_NOTHING,      CONF_ACTION_NOTHING,    CONF_ACTION_NOTHING, CONF_ACTION_NOTHING,                      0,     0 },
  
  // Puff actions
  { PRESS_SAP_MAIN_STATE_PUFF,        CONF_ACTION_LEFT_CLICK,   CONF_ACTION_B1_PRESS,   CONF_ACTION_SELECT_MENU_ITEM, CONF_ACTION_SELECT_MENU_ITEM,    0,  1000 },
  { PRESS_SAP_MAIN_STATE_PUFF,        CONF_ACTION_DRAG,         CONF_ACTION_B3_PRESS,   CONF_ACTION_NOTHING, CONF_ACTION_NOTHING,                   1000,  3000 },
  { PRESS_SAP_MAIN_STATE_PUFF,        CONF_ACTION_START_MENU,   CONF_ACTION_START_MENU, CONF_ACTION_STOP_MENU, CONF_ACTION_NOTHING,                 3000,  8000 },

  // Sip Actions
  { PRESS_SAP_MAIN_STATE_SIP,         CONF_ACTION_RIGHT_CLICK,  CONF_ACTION_B2_PRESS,   CONF_ACTION_NEXT_MENU_ITEM, CONF_ACTION_NEXT_MENU_ITEM,       0, 1000 },
  { PRESS_SAP_MAIN_STATE_SIP,         CONF_ACTION_SCROLL,       CONF_ACTION_B4_PRESS,   CONF_ACTION_NOTHING, CONF_ACTION_NOTHING,                  1000, 3000 },  
  { PRESS_SAP_MAIN_STATE_SIP,         CONF_ACTION_MIDDLE_CLICK, CONF_ACTION_B5_PRESS,   CONF_ACTION_NOTHING, CONF_ACTION_NOTHING,                  3000, 8000 }
};

// Buttons built in to hub: S1 = Next, S2 = Select
const inputActionStruct buttonActionProperty[]{
  { INPUT_MAIN_STATE_NONE,            CONF_ACTION_NOTHING,      CONF_ACTION_NOTHING,    CONF_ACTION_NOTHING, CONF_ACTION_NOTHING,                     0,     0 },

  { INPUT_MAIN_STATE_S2_PRESSED,      CONF_ACTION_LEFT_CLICK,   CONF_ACTION_B1_PRESS,   CONF_ACTION_SELECT_MENU_ITEM,  CONF_ACTION_SELECT_MENU_ITEM,     0,  1000 },
  { INPUT_MAIN_STATE_S2_PRESSED,      CONF_ACTION_DRAG,         CONF_ACTION_B3_PRESS,   CONF_ACTION_NOTHING,  CONF_ACTION_NOTHING,                  1000,  3000 },  
  { INPUT_MAIN_STATE_S2_PRESSED,      CONF_ACTION_START_MENU,   CONF_ACTION_START_MENU, CONF_ACTION_STOP_MENU,  CONF_ACTION_NOTHING,                3000,  8000 },

  { INPUT_MAIN_STATE_S1_PRESSED,      CONF_ACTION_RIGHT_CLICK,  CONF_ACTION_B2_PRESS,   CONF_ACTION_NEXT_MENU_ITEM,  CONF_ACTION_NEXT_MENU_ITEM,       0,  1000 },
  { INPUT_MAIN_STATE_S1_PRESSED,      CONF_ACTION_SCROLL,       CONF_ACTION_B4_PRESS,   CONF_ACTION_NOTHING,  CONF_ACTION_NOTHING,                  1000,  3000 }, 
  { INPUT_MAIN_STATE_S1_PRESSED,      CONF_ACTION_MIDDLE_CLICK, CONF_ACTION_B5_PRESS,   CONF_ACTION_NOTHING,  CONF_ACTION_NOTHING,                  3000,  8000 },

  { INPUT_MAIN_STATE_S12_PRESSED,     CONF_ACTION_START_MENU,   CONF_ACTION_START_MENU, CONF_ACTION_STOP_MENU,  CONF_ACTION_NOTHING,                   0,  1000 },
};

// External Assistive Switch Jacks
const inputActionStruct switchActionProperty[]{
  { INPUT_MAIN_STATE_NONE,            CONF_ACTION_NOTHING,      CONF_ACTION_NOTHING,        CONF_ACTION_NOTHING,  CONF_ACTION_NOTHING,           0,    0 },
  
  { INPUT_MAIN_STATE_S1_PRESSED,      CONF_ACTION_LEFT_CLICK,   CONF_ACTION_B1_PRESS,       CONF_ACTION_SELECT_MENU_ITEM,  CONF_ACTION_SELECT_MENU_ITEM,  0,  1000 },
  { INPUT_MAIN_STATE_S1_PRESSED,      CONF_ACTION_DRAG,         CONF_ACTION_B3_PRESS,       CONF_ACTION_NOTHING,  CONF_ACTION_NOTHING,        1000,  3000 },
  { INPUT_MAIN_STATE_S1_PRESSED,      CONF_ACTION_START_MENU,   CONF_ACTION_START_MENU,     CONF_ACTION_STOP_MENU,  CONF_ACTION_NOTHING,      3000,  8000 },
  
  { INPUT_MAIN_STATE_S2_PRESSED,      CONF_ACTION_MIDDLE_CLICK, CONF_ACTION_B5_PRESS,       CONF_ACTION_NOTHING,  CONF_ACTION_NOTHING,           0,  1000 },
  { INPUT_MAIN_STATE_S2_PRESSED,      CONF_ACTION_NOTHING,      CONF_ACTION_B6_PRESS,       CONF_ACTION_NOTHING,  CONF_ACTION_NOTHING,        1000,  3000 },
  { INPUT_MAIN_STATE_S2_PRESSED,      CONF_ACTION_CURSOR_CENTER, CONF_ACTION_CURSOR_CENTER, CONF_ACTION_NOTHING,  CONF_ACTION_NOTHING,        3000,  8000 },
  
  { INPUT_MAIN_STATE_S3_PRESSED,      CONF_ACTION_RIGHT_CLICK,  CONF_ACTION_B2_PRESS,       CONF_ACTION_NEXT_MENU_ITEM,  CONF_ACTION_NEXT_MENU_ITEM,    0,  1000 },
  { INPUT_MAIN_STATE_S3_PRESSED,      CONF_ACTION_SCROLL,       CONF_ACTION_B4_PRESS,       CONF_ACTION_NOTHING,   CONF_ACTION_NOTHING,       1000,  3000 },
  { INPUT_MAIN_STATE_S3_PRESSED,      CONF_ACTION_MIDDLE_CLICK, CONF_ACTION_NOTHING,        CONF_ACTION_NOTHING,  CONF_ACTION_NOTHING,        3000,  8000 },
  
  { INPUT_MAIN_STATE_S13_PRESSED,      CONF_ACTION_START_MENU,  CONF_ACTION_START_MENU,     CONF_ACTION_STOP_MENU,  CONF_ACTION_NOTHING,          0, 3000 },
};

// LED Action for all available output actions. This maps what happens with the lights when different actions are triggered.
// ledOutputActionNumber, ledNumber, ledStartColor, ledEndColor, ledEndAction
const ledActionStruct ledActionProperty[]{
  { CONF_ACTION_NOTHING,            CONF_LED_NONE,    LED_CLR_NONE,   LED_CLR_NONE, LED_ACTION_NONE },
  { CONF_ACTION_LEFT_CLICK,         CONF_LED_LEFT,    LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_RIGHT_CLICK,        CONF_LED_RIGHT,   LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_MIDDLE_CLICK,       CONF_LED_MIDDLE,  LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_DRAG,               CONF_LED_LEFT,    LED_CLR_RED,    LED_CLR_RED,  LED_ACTION_ON },
  { CONF_ACTION_SCROLL,             CONF_LED_RIGHT,   LED_CLR_RED,    LED_CLR_RED,  LED_ACTION_ON },
  { CONF_ACTION_B1_PRESS,           CONF_LED_LEFT,    LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_B2_PRESS,           CONF_LED_RIGHT,   LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_B3_PRESS,           CONF_LED_LEFT,    LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_B4_PRESS,           CONF_LED_RIGHT,   LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_B5_PRESS,           CONF_LED_MIDDLE,  LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_B6_PRESS,           CONF_LED_MIDDLE,  LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_B7_PRESS,           CONF_LED_NONE,    LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_B8_PRESS,           CONF_LED_NONE,    LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_CURSOR_CENTER,      CONF_LED_MICRO,   LED_CLR_PURPLE, LED_CLR_NONE, LED_ACTION_NONE },
  { CONF_ACTION_CURSOR_CALIBRATION, CONF_LED_MICRO,   LED_CLR_PURPLE, LED_CLR_NONE, LED_ACTION_ON },
  { CONF_ACTION_DEC_SPEED,          CONF_LED_LEFT,    LED_CLR_RED,    LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_INC_SPEED,          CONF_LED_RIGHT,   LED_CLR_RED,    LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_CHANGE_MODE,        CONF_LED_NONE,    LED_CLR_NONE,   LED_CLR_NONE, LED_ACTION_NONE },
  { CONF_ACTION_START_MENU,         CONF_LED_ALL,     LED_CLR_NONE,   LED_CLR_RED,  LED_ACTION_BLINK },
  { CONF_ACTION_STOP_MENU,          CONF_LED_ALL,     LED_CLR_NONE,   LED_CLR_NONE, LED_ACTION_BLINK },
  { CONF_ACTION_NEXT_MENU_ITEM,     CONF_LED_LEFT,    LED_CLR_RED,    LED_CLR_NONE, LED_ACTION_BLINK },
  { CONF_ACTION_SELECT_MENU_ITEM,   CONF_LED_RIGHT,   LED_CLR_RED,    LED_CLR_NONE, LED_ACTION_BLINK },
  { CONF_ACTION_RESET,              CONF_LED_MICRO,   LED_CLR_RED,    LED_CLR_RED,  LED_ACTION_NONE },
  { CONF_ACTION_FACTORY_RESET,      CONF_LED_MICRO,   LED_CLR_RED,    LED_CLR_RED,  LED_ACTION_NONE }
};
