

//DO NOT CHANGE 

#define OUTPUT_NOTHING 0                              // No action
#define OUTPUT_LEFT_CLICK 1                           // Generates a short left click
#define OUTPUT_RIGHT_CLICK 2                          // Generates a short right click
#define OUTPUT_DRAG 3                                 // Initiates drag mode, holding down left click until cancelled
#define OUTPUT_SCROLL 4                               // Initiates scroll mode. Vertical motion generates mouse scroll wheel movement.
#define OUTPUT_CURSOR_CALIBRATION 5                   // Initiates the joystick calibration.
#define OUTPUT_MIDDLE_CLICK 6                         // Generates a short middle click

#define INPUT_BUTTON1_PIN A1 // pushbutton S1
#define INPUT_BUTTON2_PIN 11 // pushbutton S2
#define INPUT_BUTTON3_PIN 10 // pushbutton S3
#define INPUT_SWITCH1_PIN A1 // 3.5mm jack SW1
#define INPUT_SWITCH2_PIN 11 // 3.5mm jack SW2
#define INPUT_SWITCH3_PIN 10 // 3.5mm jack SW3

#define INPUT_BUTTON_NUMBER 3
#define INPUT_SWITCH_NUMBER 3

#define SAP_MAIN_STATE_NONE 0
#define SAP_MAIN_STATE_SIP 1
#define SAP_MAIN_STATE_PUFF 2

#define SAP_SEC_STATE_WAITING 0
#define SAP_SEC_STATE_STARTED 1
#define SAP_SEC_STATE_RELEASED 2

#define LED_ACTION_OFF 0
#define LED_ACTION_ON 1
#define LED_ACTION_BLINK 2


#define SETTINGS_FILE    "/settings.txt"
#define SETTINGS_JSON    "{\"CF\":5,\"MN\":0,\"VN\":0.0,\"CM\":0,\"SL\":0,\"ST\":0,\"PT\":0,\"CA0\":[0.0,0.0],\"CA1\":[25.0,25.0],\"CA2\":[-25.0,25.0],\"CA3\":[-25.0,-25.0],\"CA4\":[25.0,-25.0]}"


//CAN BE CHANGED 

#define SOFT_SIP_THRESHOLD -2.5                    //hPa
#define SOFT_PUFF_THRESHOLD 2.5

#define HARD_SIP_THRESHOLD -10.0                    //hPa
#define HARD_PUFF_THRESHOLD 10.0

#define LED_BRIGHTNESS 50 
#define LED_BRIGHTNESS_HIGH 150 

#define COM_METHOD 1        //0 = None , 1 = USB , 2 = Wireless  
