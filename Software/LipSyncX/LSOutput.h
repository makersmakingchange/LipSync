
/* 
* File: LSOutput.h
* Firmware: LipSync X
* Developed by: MakersMakingChange
* Version: Beta (23 November 2023)
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

//Header definition
#ifndef _LSOUTPUT_H
#define _LSOUTPUT_H

#include <Adafruit_NeoPixel.h>

//#define OUTPUT_RGB_LED_PIN A1 //Output pin for neopixel
#define OUTPUT_RGB_LED_NUM 0  //Number of leds

//Led color code
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

#define LED_STATE_OFF 0
#define LED_STATE_ON 1

//Adafruit_NeoPixel ledPixels = Adafruit_NeoPixel(OUTPUT_RGB_LED_NUM, OUTPUT_RGB_LED_PIN, NEO_GRB + NEO_KHZ800);


struct rgbStruct {
    int r;    // red value 0 to 255
    int g;   // green value
    int b;   // blue value
};

//Color structure 
typedef struct { 
  uint8_t colorNumber;
  String colorName;
  rgbStruct colorCode;
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
    {LED_CLR_TEAL,"Teal",{0,128,128}},
    {LED_CLR_WHITE,"White",{255,255,255}}
};   


class LSOutput {
  private:
  int ledBrightness;

  public:
    LSOutput();
    void begin();   
    void clearLedAll();                                  
    void clearLed(int ledNumber);
    uint32_t getLedColor(int ledNumber);
    uint8_t getLedBrightness();
    void setLedBrightness(int ledBrightness); 
    void setLedColor(int ledNumber, int ledColorNumber, int ledBrightness); 


};

    

LSOutput::LSOutput() {

}

void LSOutput::begin() {

  // Hub LEDs
  pinMode(CONF_LED_MOUSE_PIN, OUTPUT);
  pinMode(CONF_LED_GAMEPAD_PIN, OUTPUT);

 // Microcontroller LED
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT); 

  //ledPixels.begin();
  //clearLedAll();
}


//***CLEAR ALL RGB LED FUNCTION***//

void LSOutput::clearLedAll() {
  //clearLed(OUTPUT_RGB_LED_NUM+1);
}

//***CLEAR RGB LED FUNCTION***//

void LSOutput::clearLed(int ledNumber) {
  //setLedColor(ledNumber,LED_CLR_NONE,255);
}


//***GET RGB LED COLOR FUNCTION***//

uint32_t LSOutput::getLedColor(int ledNumber) {

  //uint32_t colorValue = ledPixels.getPixelColor(ledNumber-1);
  
  return 0;
  //return colorValue;

}


//***GET RGB LED BRIGHTNESS FUNCTION***//

uint8_t LSOutput::getLedBrightness() {
  return 0;
//  return (ledPixels.getBrightness());
}



//***SET RGB LED BRIGHTNESS FUNCTION***//

void LSOutput::setLedBrightness(int ledBrightness) {
  // ledPixels.setBrightness(ledBrightness);
  // ledPixels.show();
}


//***SET RGB LED COLOR FUNCTION***//

void LSOutput::setLedColor(int ledNumber, int ledColorNumber, int ledBrightness) {
  
    // if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
    //   ledPixels.setPixelColor(ledNumber-1, ledPixels.Color(colorProperty[ledColorNumber].colorCode.g,colorProperty[ledColorNumber].colorCode.r,colorProperty[ledColorNumber].colorCode.b));
    // }
    // else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
    //   for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
    //     ledPixels.setPixelColor(i, ledPixels.Color(colorProperty[ledColorNumber].colorCode.g,colorProperty[ledColorNumber].colorCode.r,colorProperty[ledColorNumber].colorCode.b));            
    //   }
    // }
    // ledPixels.setBrightness(ledBrightness);
    // ledPixels.show();
}




#endif 
