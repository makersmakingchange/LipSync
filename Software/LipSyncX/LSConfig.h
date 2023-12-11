/* 
* File: LipSyncX.ino
* Firmware: LipSync Hub
* Developed by: MakersMakingChange
* Version: Beta (11 December 2023) 
* Copyright Neil Squire Society 2022. 
* License: This work is licensed under the CC BY SA 4.0 License: http://creativecommons.org/licenses/by-sa/4.0 .
*/

//***DO NOT CHANGE***//
#define CONF_LIPSYNC_MODEL 1                              //Mouse = 1 , Gamepad = 2
#define CONF_LIPSYNC_VERSION 1.0

//Output pins
#define CONF_LED1_PIN 7                                 // Pushbutton S1
#define CONF_LED2_PIN 8                                 // Pushbutton S2
#define CONF_LED3_PIN 6                                 // Pushbutton S3

//Input pins
#define CONF_BUTTON1_PIN 9                                 // Pushbutton S1
#define CONF_BUTTON2_PIN 3                                 // Pushbutton S2

#define CONF_SWITCH1_PIN A0                                // 3.5mm jack SW1
#define CONF_SWITCH2_PIN A1                                // 3.5mm jack SW2
#define CONF_SWITCH3_PIN A2                                // 3.5mm jack SW3


//Input buttons and switch values 
#define CONF_BUTTON_NUMBER 2
#define CONF_SWITCH_NUMBER 3

//Output action numbers
#define CONF_ACTION_NOTHING 0                              // No action
#define CONF_ACTION_LEFT_CLICK 1                           // Generates a short left click
#define CONF_ACTION_RIGHT_CLICK 2                          // Generates a short right click
#define CONF_ACTION_DRAG 3                                 // Initiates drag mode, holding down left click until cancelled
#define CONF_ACTION_SCROLL 4                               // Initiates scroll mode. Vertical motion generates mouse scroll wheel movement.
#define CONF_ACTION_CURSOR_CENTER 5                        // Center the joystick 
#define CONF_ACTION_CURSOR_CALIBRATION 6                   // Initiates the joystick calibration.
#define CONF_ACTION_MIDDLE_CLICK 7                         // Generates a short middle click
#define CONF_ACTION_DEC_SPEED 8                            // Decrease cursor movement speed
#define CONF_ACTION_INC_SPEED 9                            // Increase cursor movement speed
#define CONF_ACTION_CHANGE_MODE 10                         // Change communication mode
#define CONF_ACTION_FACTORY_RESET 11                       // Factory Reset

//Flash Memory settings - Don't change  
#define CONF_SETTINGS_FILE    "/settings.txt"
#define CONF_SETTINGS_JSON    "{\"MN\":0,\"VN\":0.0,\"CM\":0,\"SS\":5,\"SL\":5,\"PM\":0,\"ST\":0.0,\"PT\":0.0,\"AV\":0,\"DZ\":0.0,\"CA0\":[0.0,0.0],\"CA1\":[15.0,15.0],\"CA2\":[-15.0,15.0],\"CA3\":[-15.0,-15.0],\"CA4\":[15.0,-15.0],\"DM\":0}"

//Polling rates for each module
#define CONF_JOYSTICK_POLL_RATE 50          //50ms
#define CONF_SCROLL_POLL_RATE 150           //150ms
#define CONF_PRESSURE_POLL_RATE 50          //50ms
#define CONF_INPUT_POLL_RATE 50             //50ms
#define CONF_BT_FEEDBACK_POLL_RATE 1000     //1s
#define CONF_DEBUG_POLL_RATE 100            //100ms

//Joystick values 
#define CONF_JOY_SPEED_LEVEL_MIN 0
#define CONF_JOY_SPEED_LEVEL_MAX 10

//Communication mode values 
#define CONF_COM_MODE_NONE 0 
#define CONF_COM_MODE_USB  1
#define CONF_COM_MODE_BLE  2

#define CONF_COM_MODE_MIN 1
#define CONF_COM_MODE_MAX 2

//Debug mode values 
#define CONF_DEBUG_MODE_NONE      0 
#define CONF_DEBUG_MODE_JOYSTICK  1
#define CONF_DEBUG_MODE_PRESSURE  2
#define CONF_DEBUG_MODE_BUTTON    3
#define CONF_DEBUG_MODE_SWITCH    4
#define CONF_DEBUG_MODE_SAP       5

#define CONF_DEBUG_MODE_MIN 0
#define CONF_DEBUG_MODE_MAX 5

//***CAN BE CHANGED***//
#define CONF_API_ENABLED true               //Enable or Disable API

//Communication Method Default settings
#define CONF_COM_MODE_DEFAULT 1             //0 = None , 1 = USB , 2 = Wireless  

//Led Default settings
#define CONF_LED_BRIGHTNESS 150

//Startup Default settings
#define CONF_STARTUP_LED_STEP_TIME 500      //Time for each color

//Joystick Deadzone Default settings
#define CONF_JOY_DEADZONE_DEFAULT 0.12
#define CONF_JOY_MIN_DEADZONE 0.01
#define CONF_JOY_MAX_DEADZONE 1.0

//Joystick calibration points and related LED feedback settings
#define CONF_JOY_CALIB_START_DELAY 1000
#define CONF_JOY_CALIB_START_LED_COLOR LED_CLR_PURPLE                                           //Joystick Calibration process start and end color
#define CONF_JOY_CALIB_STEP_DELAY 1500
#define CONF_JOY_CALIB_LED_NUMBER 4
#define CONF_JOY_CALIB_LED_COLOR LED_CLR_RED                                                    //The color indicates the joystick Calibration process 
#define CONF_JOY_CALIB_STEP_BLINK 1
#define CONF_JOY_CALIB_STEP_BLINK_DELAY 150
#define CONF_JOY_CALIB_READING_DELAY 200
#define CONF_JOY_CALIB_STEP_BLINK_COLOR LED_CLR_YELLOW                                          //The color indicates the joystick Calibration about to start
#define CONF_JOY_CALIB_READING_NUMBER 10

//Joystick center initialization and related LED feedback settings 
#define CONF_JOY_INIT_START_DELAY 1000
#define CONF_JOY_INIT_LED_NUMBER 2
#define CONF_JOY_INIT_LED_COLOR LED_CLR_RED                                                     //Center initialization actual step color 
#define CONF_JOY_INIT_STEP_BLINK 1
#define CONF_JOY_INIT_STEP_BLINK_DELAY 150
#define CONF_JOY_INIT_STEP_BLINK_COLOR LED_CLR_YELLOW                                           //Center initialization start blink color 
#define CONF_JOY_INIT_READING_DELAY 100
#define CONF_JOY_INIT_READING_NUMBER 5

//Joystick cursor speed change and related LED feedback settings 
#define CONF_JOY_SPEED_LEVEL_DEFAULT 5
#define CONF_JOY_SPEED_CHANGE_LED_DELAY 150
#define CONF_JOY_SPEED_CHANGE_LED_BLINK 1
#define CONF_JOY_SPEED_DEC_LED_NUMBER 1
#define CONF_JOY_SPEED_DEC_LED_COLOR LED_CLR_PURPLE                                            //Decrease speed color
#define CONF_JOY_SPEED_INC_LED_NUMBER 3
#define CONF_JOY_SPEED_INC_LED_COLOR LED_CLR_PURPLE                                            //Increase speed color
#define CONF_JOY_SPEED_LIMIT_LED_DELAY 50
#define CONF_JOY_SPEED_LIMIT_LED_BLINK 3
#define CONF_JOY_SPEED_LIMIT_LED_COLOR LED_CLR_RED                                             //Out of range limit speed color

//Joystick cursor acceletaion change 
#define CONF_JOY_ACCELERATION_LEVEL_MAX 10
#define CONF_JOY_ACCELERATION_LEVEL_MIN -10
#define CONF_JOY_ACCELERATION_LEVEL_DEFAULT 0


//Scroll level change and related LED feedback settings 
#define CONF_SCROLL_CHANGE_LED_DELAY 150
#define CONF_SCROLL_CHANGE_LED_BLINK 1
#define CONF_SCROLL_DEC_LED_NUMBER 1
#define CONF_SCROLL_DEC_LED_COLOR LED_CLR_PURPLE                                            //Decrease scroll level color
#define CONF_SCROLL_INC_LED_NUMBER 3
#define CONF_SCROLL_INC_LED_COLOR LED_CLR_PURPLE                                            //Increase scroll level color
#define CONF_SCROLL_LIMIT_LED_DELAY 50
#define CONF_SCROLL_LIMIT_LED_BLINK 3
#define CONF_SCROLL_LIMIT_LED_COLOR LED_CLR_RED                                             //Out of range limit scroll level color

//Scroll level values 
#define CONF_SCROLL_LEVEL_DEFAULT 5
#define CONF_SCROLL_LEVEL_MIN 1
#define CONF_SCROLL_LEVEL_MAX 10
#define CONF_SCROLL_MOVE_MIN  1
#define CONF_SCROLL_MOVE_MAX  10
#define CONF_SCROLL_MOVE_BASE 1


//Sip and Puff Default settings
#define CONF_SIP_THRESHOLD 3.0                    //hPa
#define CONF_PUFF_THRESHOLD 3.0                   //hPa
#define CONF_PRESS_MIN_THRESHOLD 1.0              //hPa
#define CONF_PRESS_MAX_THRESHOLD 100.0            //hPa
#define CONF_PRESS_MODE_DEFAULT 1                 //Default pressure mode state = 2 
                                                  // 0 = None or PRESS_MODE_NONE
                                                  // 1 = Absolute or PRESS_MODE_ABS
                                                  // 2 = Differential or PRESS_MODE_DIFF

//Inputs and related LED feedback settings
#define CONF_INPUT_LED_DELAY 150          //Led blink time for input actions 
#define CONF_INPUT_LED_BLINK 1            //Led blink number  for input actions 

//Communication Mode Default settings
#define CONF_COM_MODE_LED_NUMBER  2
#define CONF_COM_MODE_LED_BLINK   1
#define CONF_COM_MODE_LED_BLINK_DELAY   1000


//Bluetooth connection and related LED feedback settings 
#define CONF_BT_SCAN_BLINK_DELAY 500
#define CONF_BT_SCAN_BLINK_NUMBER 1
#define CONF_BT_LED_NUMBER 2
#define CONF_BT_LED_COLOR LED_CLR_BLUE 
#define CONF_BT_LED_BRIGHTNESS CONF_LED_BRIGHTNESS

//Debug Default settings
#define CONF_DEBUG_MODE_DEFAULT  0        // Default debug mode state = Off 
                                          // 0 = Debug mode is Off
                                          // 1 = Joystick debug mode is On
                                          // 2 = Pressure debug mode is On
                                          // 3 = Buttons debug mode is On
                                          // 4 = Switch debug mode is On
                                          // 5 = Sip & Puff state debug mode is On
