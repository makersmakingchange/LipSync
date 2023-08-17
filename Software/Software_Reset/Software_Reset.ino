#include "XACGamepad.h"
#include "USBKeyboard.h"
#include "USBMouse.h"
#include "USBMouse.h"
#include "LSMemory.h"
#include <StopWatch.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>

//Flash Memory settings - Don't change  
#define CONF_SETTINGS_FILE    "/settings2.txt"
#define CONF_SETTINGS_JSON    "{\"MD\":0,\"CF\":0}"



#define SWITCH_MODE_CHANGE_TIME 2000                                  //How long to hold switch 4 to change mode 
#define LED_BRIGHTNESS 150                                             //The mode led color brightness which is always on ( Use a low value to decrease power usage )
#define LED_ACTION_BRIGHTNESS 150                                      //The action led color brightness which can be a higher value than LED_BRIGHTNESS


//Define Switch pins
#define LED_PIN A1

#define SWITCH_PIN 2


// Variable Declaration

//Declare switch state variables for each switch
int switchDState;

//Previous status of switches
int switchDPrevState = HIGH;


//Stopwatches array used to time switch presses
StopWatch timeWatcher[3];
StopWatch switchDTimeWatcher[1];


//Declare Switch variables for settings 
int switchConfigured;
int switchMode;


//RGB LED Color code structure 

struct rgbColorCode {
    int r;    // red value 0 to 255
    int g;   // green value
    int b;   // blue value
 };

//Color structure 
typedef struct { 
  uint8_t colorNumber;
  String colorName;
  rgbColorCode colorCode;
} colorStruct;

 //Mode structure 
typedef struct { 
  uint8_t modeNumber;
  String modeName;
  uint8_t modeColorNumber;
} modeStruct;



//Color properties 
const colorStruct colorProperty[] {
    {1,"Green",{60,0,0}},
    {2,"Pink",{0,50,60}},
    {3,"Yellow",{60,50,0}},    
    {4,"Orange",{20,60,0}},
    {5,"Blue",{0,0,60}},
    {6,"Red",{0,60,0}},
    {7,"Purple",{0,50,128}},
    {8,"Teal",{128,0,128}}       
};


//Mode properties 
const modeStruct modeProperty[] {
    {1,"Keyboard",8},
    {2,"Mouse",7},
    {3,"Joystick",1},
    {4,"None",4}
};


//Setup NeoPixel LED
Adafruit_NeoPixel ledPixels = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);

//Starts an instance of the keyboard and mouse objects
USBKeyboard keyboard;
USBMouse mouse;
XACGamepad gamepad;
LSMemory mem;

void setup() {
  ledPixels.begin();                                                           //Start NeoPixel
  Serial.begin(115200);                                                        //Start Serial
  delay(15);
  initMemory();                                                 //Initialize Memory 
  switchSetup();
  delay(5);
  if(switchMode==1) {
    keyboard.begin();
    mouse.end();
    gamepad.end();
  }
  else if(switchMode==2) {
    keyboard.end();
    mouse.begin();
    gamepad.end();
  }
  else if(switchMode==3) {
    keyboard.end();
    mouse.end();
    gamepad.begin();
  }
  else if(switchMode==4) {
    keyboard.end();
    mouse.end();
    gamepad.end();    
  }
  initLedFeedback();                                                          //Led will blink in a color to show the current mode 
  delay(5);

  //Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);                                                      

};

void loop() {
 
  delay(7000);                                                    
  if(switchMode==1) {
    keyboard.write('a');
    delay(100);                                                    
    keyboard.releaseAll();
  }
  else if(switchMode==2) {
    if (!mouse.isPressed(MOUSE_RIGHT)) {
      mouse.press(MOUSE_RIGHT);
      delay(50);
      mouse.release(MOUSE_RIGHT);
    } 
    else if (mouse.isPressed(MOUSE_RIGHT)) {
      mouse.release(MOUSE_RIGHT);
    } 
  }
  else if(switchMode==3) {
    gamepad.press(2);
    gamepad.send();
    delay(200);                                                    
    gamepad.releaseAll();
    gamepad.send();
    delay(2000);                                                    
  }

  changeSwitchMode();

  switchMode++;
  
  if (switchMode==3) { switchMode=0; }
  ledPixels.show(); 
  delay(5);
}


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

//***RGB LED FUNCTION***//aa

void setLedBlink(int numBlinks, int delayBlinks, int ledColor,uint8_t ledBrightness) {
  if (numBlinks < 0) numBlinks *= -1;

      for (int i = 0; i < numBlinks; i++) {
        ledPixels.setPixelColor(0, ledPixels.Color(colorProperty[ledColor-1].colorCode.g,colorProperty[ledColor-1].colorCode.r,colorProperty[ledColor-1].colorCode.b));
        ledPixels.setBrightness(ledBrightness);
        ledPixels.show(); 
        delay(delayBlinks);
        ledPixels.setPixelColor(0, ledPixels.Color(0,0,0));
        ledPixels.setBrightness(ledBrightness);
        ledPixels.show(); 
        delay(delayBlinks);
      }
}

//***UPDATE RGB LED COLOR FUNCTION***//

void updateLedColor(int ledColor, uint8_t ledBrightness) {
    ledPixels.setPixelColor(0, ledPixels.Color(colorProperty[ledColor-1].colorCode.g,colorProperty[ledColor-1].colorCode.r,colorProperty[ledColor-1].colorCode.b));
    ledPixels.setBrightness(ledBrightness);
    ledPixels.show();
}

//***GET RGB LED COLOR FUNCTION***//

uint32_t getLedColor(int ledModeNumber) {

  int colorNumber= modeProperty[ledModeNumber-1].modeColorNumber-1;
  
  return (ledPixels.Color(colorProperty[colorNumber].colorCode.g,colorProperty[colorNumber].colorCode.r,colorProperty[colorNumber].colorCode.b));
}

//***GET RGB LED BRIGHTNESS FUNCTION***//

uint8_t getLedBrightness() {
  return (ledPixels.getBrightness());
}

//***SET RGB LED COLOR FUNCTION***//

void setLedColor (uint32_t ledColor, uint8_t ledBrightness){
  ledPixels.setPixelColor(0, ledColor);
  ledPixels.setBrightness(ledBrightness);
  ledPixels.show(); 

}

//***SET RGB LED BRIGHTNESS FUNCTION***//

void setLedBrightness(uint8_t ledBrightness) {
  ledPixels.setBrightness(ledBrightness);
  ledPixels.show();
}

//***CLEAR RGB LED FUNCTION***//

void ledClear() {
  ledPixels.setPixelColor(0, ledPixels.Color(0,0,0));
  ledPixels.show(); 
}



void modeFeedback(int modeNumber,int delayTime, int blinkNumber =1)
{

   //Get new led color and brightness 
  uint32_t newColor = getLedColor(modeNumber);
  uint8_t newBrightness = getLedBrightness();
  
  setLedBlink(blinkNumber,delayTime,modeProperty[modeNumber-1].modeColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);

  //Set new led color and brightness 
  setLedColor(newColor,newBrightness);
  
}

//***SETUP SWITCH MODE FUNCTION***//

void switchSetup() {
  //Check if it's first time running the code
  switchConfigured = mem.readInt(CONF_SETTINGS_FILE, "CF");

  delay(5);
  if (switchConfigured==0) {
    //Define default settings if it's first time running the code
    switchMode=1;
    switchConfigured=1;

    //Write default settings to flash storage 
    mem.writeInt(CONF_SETTINGS_FILE, "MD", switchMode); 
    mem.writeInt(CONF_SETTINGS_FILE, "CF", switchConfigured); 
    delay(5);
      
  } else {
    //Load settings from flash storage if it's not the first time running the code
    switchMode=mem.readInt(CONF_SETTINGS_FILE, "MD");
    delay(5);
  }  

}


void initLedFeedback(){
  setLedBlink(2,500,modeProperty[switchMode-1].modeColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);
  updateLedColor(modeProperty[switchMode-1].modeColorNumber,LED_BRIGHTNESS);
  delay(5);
}





//***CHANGE SWITCH MODE FUNCTION***//

void changeSwitchMode(){
    //Update switch mode varia
    switchMode++;
    if (switchMode == (sizeof (modeProperty) / sizeof (modeProperty[0]))+1) {
      switchMode=1;
    } 
    else {
    }
    
    //Blink 2 times in modes color 
    //setLedBlink(2,500,modeProperty[switchMode].modeColorNumber,LED_ACTION_BRIGHTNESS);
    modeFeedback(switchMode,500,2);

    //Serial print switch mode
    Serial.print("Switch Mode: ");
    Serial.println(switchMode);
    
    //Save switch mode
    mem.writeInt(CONF_SETTINGS_FILE, "MD", switchMode);
    delay(25);
    keyboard.end();
    mouse.end();
    gamepad.end();
    NVIC_SystemReset();
    delay(10);
}
