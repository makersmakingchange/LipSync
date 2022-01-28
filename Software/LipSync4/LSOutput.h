//Header definition
#ifndef _LSOUTPUT_H
#define _LSOUTPUT_H

#include <Adafruit_NeoPixel.h>

#define OUTPUT_RGB_LED_PIN A3 //output pin for neopixel
#define OUTPUT_RGB_LED_NUM 3

#define LED_CLR_NONE 0
#define LED_CLR_RED 1
#define LED_CLR_BLUE 2
#define LED_CLR_GREEN 3
#define LED_CLR_YELLOW 4
#define LED_CLR_PINK 5
#define LED_CLR_ORANGE 6
#define LED_CLR_PURPLE 7
#define LED_CLR_TEAL 8
#define LED_CLR_MAGENTA 9

#define LED_ACTION_OFF 0
#define LED_ACTION_ON 1
#define LED_ACTION_BLINK 2

#define LED_STATE_OFF 0
#define LED_STATE_ON 1

Adafruit_NeoPixel ledPixels = Adafruit_NeoPixel(OUTPUT_RGB_LED_NUM, OUTPUT_RGB_LED_PIN, NEO_GRB + NEO_KHZ800);


int ledStateTimerId;

struct rgb_t {
    int r;    // red value 0 to 255
    int g;   // green value
    int b;   // blue value
 };

//Color structure 
typedef struct { 
  uint8_t colorNumber;
  String colorName;
  rgb_t colorCode;
} colorStruct;

//Color properties 
const colorStruct colorProperty[] {
    {LED_CLR_NONE,"None",{0,0,0}},
    {LED_CLR_RED,"Red",{60,0,0}},
    {LED_CLR_BLUE,"Blue",{0,0,60}},
    {LED_CLR_GREEN,"Green",{0,60,0}},
    {LED_CLR_YELLOW,"Yellow",{50,60,0}}, 
    {LED_CLR_PINK,"Pink",{60,0,50}},   
    {LED_CLR_ORANGE,"Orange",{60,20,0}},
    {LED_CLR_PURPLE,"Purple",{50,0,128}},
    {LED_CLR_TEAL,"Teal",{0,128,128}},
    {LED_CLR_MAGENTA,"Magenta",{255,0,255}}       
};

//led structure 
/* typedef struct { 
  uint8_t ledMainState;            //off = 0, on = 1, blink = 2
  bool ledBlinkState;              //off = 0, on = 1
  uint8_t ledColorNumber;
  uint8_t ledNumber;
  uint8_t ledBlinkNumber;
  unsigned long ledBlinkTime;
  uint8_t ledBrightness;
} ledStateType; */



class LSOutput {
  public:
    LSOutput();
    void begin();  
    void run();    
    void clearLedAll();                                  
    void clearLed(int ledNumber);
    void setLedState(int ledState, int ledNumber);
    uint32_t getLedColor(int ledNumber);
    void setLedColorById(int ledNumber, int ledColorNumber, uint8_t ledBrightness); 
    void setLedBlinkById(int ledNumber, int numBlinks, int delayBlinks, int ledColorNumber,uint8_t ledBrightness);
    void setLedActionById(int ledNumber, int numBlinks, int delayBlinks, int ledColorNumber,uint8_t ledBrightness, int ledAction);
    uint8_t getLedBrightness();
    void setLedBrightness(uint8_t ledBrightness); 
   private:
     int ledAction;                               //off = 0, on = 1, blink = 2
     boolean ledStateArray[OUTPUT_RGB_LED_NUM];        //off = 0, on = 1
     void performLedColor(int ledNumber, int ledColorNumber, uint8_t ledBrightness, boolean ledState);
     void performLedToggle(int ledNumber, int ledColorNumber,uint8_t ledBrightness);
     LSTimer ledStateTimer;

};

LSOutput::LSOutput() {

}

void LSOutput::begin() {

  pinMode(OUTPUT_RGB_LED_PIN, OUTPUT);

  ledPixels.begin();
  //clearLedAll();
}
void LSOutput::run() {

  //Update the timer
  ledStateTimer.run();

  //Set the action and state once the blinking timer has finished it's job 
  if(!ledStateTimer.isEnabled(ledStateTimerId) && ledAction == LED_ACTION_BLINK){
    //clearLedAll();
  }
}


//***CLEAR ALL RGB LED FUNCTION***//

void LSOutput::clearLedAll() {
  clearLed(OUTPUT_RGB_LED_NUM+1);
}

//***CLEAR RGB LED FUNCTION***//

void LSOutput::clearLed(int ledNumber) {
  if(ledStateTimer.isEnabled(ledStateTimerId)){
    ledStateTimer.deleteTimer(ledStateTimerId);
  }
  performLedColor(ledNumber,LED_CLR_NONE,0, LED_STATE_OFF);
  ledAction = LED_ACTION_OFF;
}

//***SET RGB LED STATE FUNCTION***//

void LSOutput::setLedState(int ledState, int ledNumber){ //Set led state after output action is performed 
  if(ledNumber<=OUTPUT_RGB_LED_NUM){
    ledStateArray[ledNumber-1]=ledState;
  }
  else if(ledNumber==OUTPUT_RGB_LED_NUM+1){
      for (int i=0; i < OUTPUT_RGB_LED_NUM; i++) {
        ledStateArray[i]=ledState;
      }
  }
}

//***GET RGB LED COLOR FUNCTION***//

uint32_t LSOutput::getLedColor(int ledNumber) {

  uint32_t colorValue = ledPixels.getPixelColor(ledNumber-1);
  
  return colorValue;
}


//***SET RGB LED COLOR BY ID FUNCTION***//

void LSOutput::setLedColorById(int ledNumber, int ledColorNumber, uint8_t ledBrightness) {
  if(ledAction == LED_ACTION_BLINK){
    return;
  }
  performLedColor(ledNumber,ledColorNumber,ledBrightness, LED_STATE_ON);
  ledAction = LED_ACTION_ON;
}



//***SET RGB LED BLINK BY ID FUNCTION***//

void LSOutput::setLedBlinkById(int ledNumber, int numBlinks, int delayBlinks, int ledColorNumber,uint8_t ledBrightness) {

  if(ledAction == LED_ACTION_BLINK){        //Don't start another blinking action if it's already blinking 
    return;
  }
  performLedColor(ledNumber, ledColorNumber, ledBrightness, LED_STATE_ON);
  if(numBlinks==1){
    //ledStateTimerId = ledStateTimer.setTimeout(delayBlinks,performLedToggle(ledNumber, ledColorNumber,ledBrightness));    
    //ledStateTimerId = ledStateTimer.setTimeout(delayBlinks,clearLedAll); 
  } else{
    //ledStateTimerId = ledStateTimer.setTimer(delayBlinks,performLedToggle(ledNumber, ledColorNumber,ledBrightness),numBlinks*2);    
  }
  ledAction = LED_ACTION_BLINK;

}

void LSOutput::setLedActionById(int ledNumber, int numBlinks, int delayBlinks, int ledColorNumber,uint8_t ledBrightness, int ledAction){
      switch (ledAction) {
        case LED_ACTION_OFF: {
          clearLed(ledNumber);
          break;
        }
        case LED_ACTION_ON: {
          setLedColorById(ledNumber, ledColorNumber, ledBrightness);
          break;
        }
        case LED_ACTION_BLINK: {
          setLedBlinkById(ledNumber, numBlinks, delayBlinks, ledColorNumber,ledBrightness);
          break;
        }
      }
}


//***GET RGB LED BRIGHTNESS FUNCTION***//

uint8_t LSOutput::getLedBrightness() {
  return (ledPixels.getBrightness());
}



//***SET RGB LED BRIGHTNESS FUNCTION***//

void LSOutput::setLedBrightness(uint8_t ledBrightness) {
  ledPixels.setBrightness(ledBrightness);
  ledPixels.show();
}


//***PERFORM RGB LED COLOR FUNCTION***//

void LSOutput::performLedColor(int ledNumber, int ledColorNumber, uint8_t ledBrightness, boolean ledState) {
    if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
      ledPixels.setPixelColor(ledNumber-1, ledPixels.Color(colorProperty[ledColorNumber].colorCode.g,colorProperty[ledColorNumber].colorCode.r,colorProperty[ledColorNumber].colorCode.b));
      ledStateArray[ledNumber-1] = ledState;
    }
    else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
      for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
        ledPixels.setPixelColor(i, ledPixels.Color(colorProperty[ledColorNumber].colorCode.g,colorProperty[ledColorNumber].colorCode.r,colorProperty[ledColorNumber].colorCode.b));            
      }
      ledStateArray[OUTPUT_RGB_LED_NUM-1] = ledState;
    }
    ledPixels.setBrightness(ledBrightness);
    ledPixels.show();
}


//***PERFORM RGB LED BLINK FUNCTION***//

void LSOutput::performLedToggle(int ledNumber, int ledColorNumber,uint8_t ledBrightness) {

  int tempColorNumber = ledColorNumber;
  boolean tempState = LED_STATE_OFF;

  //Single led or all leds
    if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
      ledStateArray[ledNumber-1] = !ledStateArray[ledNumber-1];
      tempState = ledStateArray[ledNumber-1];
    }
    else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
      for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
        ledStateArray[i] = !ledStateArray[i];
      }
      tempState = ledStateArray[OUTPUT_RGB_LED_NUM-1];
    }

    (tempState) ? tempColorNumber = ledColorNumber : tempColorNumber = LED_CLR_NONE;

  
  performLedColor(ledNumber,tempColorNumber,ledBrightness, tempState);

}


#endif 
