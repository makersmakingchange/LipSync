//Header definition
#ifndef _LSOUTPUT_H
#define _LSOUTPUT_H


#include <Adafruit_NeoPixel.h>

#define OUTPUT_RGB_LED_PIN A3 //output pin for neopixel
#define OUTPUT_RGB_LED_NUM 3

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


#define LED_ACTION_OFF 0
#define LED_ACTION_ON 1
#define LED_ACTION_BLINK 2

#define LED_STATE_OFF 0
#define LED_STATE_ON 1

Adafruit_NeoPixel ledPixels = Adafruit_NeoPixel(OUTPUT_RGB_LED_NUM, OUTPUT_RGB_LED_PIN, NEO_GRB + NEO_KHZ800);


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
    {LED_CLR_BLUE,"Blue",{0,0,60}},
    {LED_CLR_PURPLE,"Purple",{50,0,128}},
    {LED_CLR_MAGENTA,"Magenta",{255,0,255}},       
    {LED_CLR_PINK,"Pink",{60,0,50}},   
    {LED_CLR_RED,"Red",{60,0,0}},
    {LED_CLR_ORANGE,"Orange",{60,20,0}},
    {LED_CLR_YELLOW,"Yellow",{50,60,0}}, 
    {LED_CLR_GREEN,"Green",{0,60,0}},
    {LED_CLR_TEAL,"Teal",{0,128,128}}
};


class LSOutput {
  public:
    LSOutput();
    void begin();  
    void clearLedAll();                                  
    void clearLed(int ledNumber);
    void setLedState(int ledState, int ledNumber);
    uint32_t getLedColor(int ledNumber);
    void setLedColorById(int ledNumber, int ledColorNumber, uint8_t ledBrightness); 
    uint8_t getLedBrightness();
    void setLedBrightness(uint8_t ledBrightness); 
    void setLedColor(int ledNumber, int ledColorNumber, uint8_t ledBrightness);
   private:
     int ledAction;                               //off = 0, on = 1, blink = 2
     boolean ledStateArray[OUTPUT_RGB_LED_NUM];        //off = 0, on = 1
     LSTimer ledStateTimer;

};

LSOutput::LSOutput() {

}

void LSOutput::begin() {

  pinMode(OUTPUT_RGB_LED_PIN, OUTPUT);

  ledPixels.begin();
  //clearLedAll();
}



//***CLEAR ALL RGB LED FUNCTION***//

void LSOutput::clearLedAll() {
  clearLed(OUTPUT_RGB_LED_NUM+1);
}

//***CLEAR RGB LED FUNCTION***//

void LSOutput::clearLed(int ledNumber) {

  setLedColor(ledNumber,LED_CLR_NONE,0);
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
  setLedColor(ledNumber,ledColorNumber,ledBrightness);
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

void LSOutput::setLedColor(int ledNumber, int ledColorNumber, uint8_t ledBrightness) {
    if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
      ledPixels.setPixelColor(ledNumber-1, ledPixels.Color(colorProperty[ledColorNumber].colorCode.g,colorProperty[ledColorNumber].colorCode.r,colorProperty[ledColorNumber].colorCode.b));
    }
    else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
      for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
        ledPixels.setPixelColor(i, ledPixels.Color(colorProperty[ledColorNumber].colorCode.g,colorProperty[ledColorNumber].colorCode.r,colorProperty[ledColorNumber].colorCode.b));            
      }
    }
    ledPixels.setBrightness(ledBrightness);
    ledPixels.show();
}


#endif 
