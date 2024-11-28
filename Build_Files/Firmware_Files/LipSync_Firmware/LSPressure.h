/* 
* File: LSPressure.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: v4.0.2 (21 November 2024)
  License: GPL v3.0 or later

  Copyright (C) 2024 Neil Squire Society
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
#ifndef _LSPRESSURE_H
#define _LSPRESSURE_H

#include <Adafruit_LPS35HW.h>   // Tube pressure sensor
#include <Adafruit_LPS2X.h>     // Ambient pressure sensor
#include <Adafruit_Sensor.h>     // Generic sensor

#define LPS22_I2CADDR 0x5C      // Modified LPS22 address

#define PRESS_BUFF_SIZE 5       //The size of pressureBuffer
#define PRESS_SAP_BUFF_SIZE 12  //The size of sip and puff state buffer 

#define PRESS_REF_TOLERANCE 0.1 //The change in reference pressure (hPa) that would initiate reference pressure update 
                                //It's only used in differential mode

#define PRESS_SAP_DEFAULT_THRESHOLD 3.0 // hPa hecto Pascals

#define PRESS_FILTER_NONE 0
#define PRESS_FILTER_AVERAGE 1

#define PRESS_MODE_NONE 0
#define PRESS_MODE_ABS 1              //Absolute pressure mode
#define PRESS_MODE_DIFF 2             //Differential pressure mode

#define PRESS_MODE_MIN 1              //Minimum and maximum mode numbers 
#define PRESS_MODE_MAX 2              //Used for switching pressure mode

//Sip and puff secondary states 
#define PRESS_SAP_SEC_STATE_WAITING 0   //Waiting : No sip or puff
#define PRESS_SAP_SEC_STATE_STARTED 1   //Started : Sip or puff being performed  (Off->On)
#define PRESS_SAP_SEC_STATE_RELEASED 2  //Released : Sip or puff was just released (On->Off)

#define PRESS_SAP_ACTION_TIMEOUT 60000  //Rest timer

//Pressure structure 
typedef struct {
  float sapPressureAbs;                 //Stand-alone I2C Sensor reading [hPa]
  float ambientPressure;                //Ambient Pressure [hPa]
  float sapPressure;                    //Pressure difference used in sip and puff processing [hPa]
} pressureStruct;


class LSPressure {
  private: 
    Adafruit_LPS35HW lps35hw = Adafruit_LPS35HW();      //Create an object of Adafruit_LPS35HW class for Sip and Puff pressure
    Adafruit_LPS22 lps22;                               //Create an object of Adafruit_LPS2X class for ambient pressure
    sensors_event_t lps22_pressure;                     //Ambient temperature event object 
    sensors_event_t lps22_temperature;                  //Ambient pressure event object                           
    LSCircularBuffer <pressureStruct> pressureBuffer;   //Create a buffer of type pressureStruct to push pressure readings 
    LSCircularBuffer <inputStateStruct> sapBuffer;      //Create a buffer of type inputStateStruct to push sap states 
    int filterMode;                                     //Filter Mode : NONE or AVERAGE     
    int pressureMode;                                   //Pressure Mode: DIFF or ABS pressure 
    float sapPressureAbs;                               //Main Pressure reading (Sip and Puff Absolute) [hPa]
    float ambientPressure;                              //Reference Pressure reading (Ambient Absolute) [hPa]
    float offsetPressure;                               //Offset Pressure [hPa] (Difference between two sensors)
    float sapPressure;                                  //Calculated Pressure Difference sapPressure = (sapPressureAbs- ambientPressure- offsetPressure)
    sensors_event_t pressure_event;                     //Onboard pressure event object 
    float refTolVal;                                    //The tolerance value in hPa used to check and update reference pressure 
    LSTimer <void> mainStateTimer;                      //Timer used to measure time for each sip and puff action. 
    int sapStateTimerId;                                //The id for the sap state timer
    inputStateStruct sapCurrState;                      //The current state of sip and puff
    inputStateStruct sapPrevState;                      //The previous state of sip and puff
    float sipThreshold;                                 //Sip Threshold 
    float puffThreshold;                                //Puff Threshold 
    int sapMainState;                                   //The value which represents the current main state (example: PRESS_SAP_MAIN_STATE_PUFF) 
  public:
    LSPressure();                                       //Constructor
    void begin();                                    
    void clear();  
    void setFilterMode(int mode);                       //Set filter Mode: None or Average
    int getPressureMode();                              //Get Sip and Puff Pressure Mode : ABS or DIFF pressure
    void setPressureMode(int mode);                     //Set Sip and Puff Pressure Mode : ABS or DIFF pressure
    void setRefTolerance(float value);                  //Set reference pressure change tolerance value that is used to update the reference pressure 
    float getOffsetPressure();                            //Get Offset pressure (sapPressureAbs- ambientPressure)
    void setOffsetPressure();                             //Set Offset pressure offsetPressure = (sapPressureAbs- ambientPressure)
    void setZeroPressure();                             //Zero the base reference pressure and update Offset pressure value 
    void setThreshold(float s, float p);                //Set sip and puff thresholds
    void setSipThreshold(float s);                      //Set sip threshold
    void setPuffThreshold(float p);                     //Set puff threshold
    void update();                                      //Update the pressure buffer and sip and puff buffer with new readings 
    void updatePressure();                              //Update the pressure buffer with new readings 
    void updateState();                                 //Update the and puff buffer with new states 
    float getSapPressureAbs();                            //Get last main pressure from pressure buffer
    float getAmbientPressure();                             //Get last reference pressure from pressure buffer
    float getSapPressure();                            //Get the Pressure Difference sapPressure = (sapPressureAbs- ambientPressure- offsetPressure)
    pressureStruct getAllPressure();                    //Get the latest pressure values 
    inputStateStruct getState();                        //Get the latest sip and puff state  
};


//*********************************//
// Function   : LSPressure 
// 
// Description: Construct LSPressure
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
LSPressure::LSPressure() {
  pressureBuffer.begin(PRESS_BUFF_SIZE);        //Initialize pressureBuffer
  sapBuffer.begin(PRESS_SAP_BUFF_SIZE);         //Initialize sapBuffer 
}

//*********************************//
// Function   : begin 
// 
// Description: Initialize LSPressure with default settings 
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::begin() {

  //LPS35HW Pressure sensor setup
  if (!lps35hw.begin_I2C()) {
    Serial.println(F("Couldn't find LPS35HW chip"));
    //todo this should throw an error
  }
  else{
    if (USB_DEBUG){Serial.println("USBDEBUG: LPS35HW Found.");}
  }

  //LPS22 Pressure sensor setup
  if (!lps22.begin_I2C(LPS22_I2CADDR)) {
    Serial.println(F("Couldn't find LPS22 chip"));
    //todo this should throw an error
  } 
   else{
    if (USB_DEBUG){Serial.println("USBDEBUG: LPS22 Found.");}
  }


  setFilterMode(PRESS_FILTER_NONE);         //Set the default filter mode to none

  setPressureMode(PRESS_MODE_DIFF);         //Set the default pressure mode to differential mode

  setRefTolerance(PRESS_REF_TOLERANCE);     //Set the default tolerance value to update reference pressure 

  setThreshold(PRESS_SAP_DEFAULT_THRESHOLD,PRESS_SAP_DEFAULT_THRESHOLD); //Set the default sip and puff thresholds

  lps35hw.setDataRate(LPS35HW_RATE_25_HZ);  //Options: 1 Hz, 10Hz, 25Hz, 50Hz, 75Hz

  lps22.setDataRate(LPS22_RATE_25_HZ);         // Options: 1-shot, 

  clear();      //Clear buffers and make sure no sip and puff action is set as previous actions

  setZeroPressure();    //Set the zero pressure 
}

//*********************************//
// Function   : clear 
// 
// Description: Clear LSPressure class
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::clear() {

  while(pressureBuffer.getLength()<PRESS_BUFF_SIZE){    //Clear pressureBuffer by pushing zero readings to buffers 
    pressureBuffer.pushElement({0.0, 0.0, 0.0});   
  }

    //Push initial state to state Queue
  sapCurrState = sapPrevState = {PRESS_SAP_MAIN_STATE_NONE, PRESS_SAP_SEC_STATE_WAITING, 0};
  sapBuffer.pushElement(sapCurrState);

  //Reset and start the timer   
  sapStateTimerId =  mainStateTimer.startTimer();

}

//*********************************//
// Function   : setFilterMode 
// 
// Description: Set the filter mode ( Currently not used )
// 
// Arguments :  mode : int : Each filter mode will have a number 
// 
// Return     : void
//*********************************//
void LSPressure::setFilterMode(int mode) {
  filterMode = mode;
}

//*********************************//
// Function   : getPressureMode 
// 
// Description: Get the pressure mode ( ABS = 1 or DIFF = 2)
// 
// Arguments :  void
// 
// Return     : mode : int : Each pressure mode has a number 
//*********************************//
int LSPressure::getPressureMode() {
  return pressureMode;
}

//*********************************//
// Function   : setPressureMode 
// 
// Description: Set the pressure mode ( ABS = 1 or DIFF = 2)
// 
// Arguments :  mode : int : Each pressure mode has a number 
// 
// Return     : void
//*********************************//
void LSPressure::setPressureMode(int mode) {
  pressureMode = mode;
}

//*********************************//
// Function   : setRefTolerance 
// 
// Description: Set the reference pressure change tolerance (hPa)
// 
// Arguments :  value : float : The pressure change in hPa
// 
// Return     : void
//*********************************//
void LSPressure::setRefTolerance(float value) {
  refTolVal = value;
}

//*********************************//
// Function   : getOffsetPressure 
// 
// Description: Get the offset pressure value (hPa)
//              It doesn't set offsetPressure
// Arguments :  void
// 
// Return     : tempOffsetPressure : float : The offset pressure value in hPa
//*********************************//
float LSPressure::getOffsetPressure() {

  //Initialize variables used to calculate offset value  
  float tempSapPressureAbs = 0.00;
  float tempAmbientPressure = 0.00;
  float tempOffsetPressure = 0.00;

  

  //If pressure mode is differential  
  if(pressureMode==PRESS_MODE_DIFF){
    //Keep reading until we have a valid main and reference pressure values > 0.0
    do{     
      tempSapPressureAbs = lps35hw.readPressure();
      lps22.getEvent(&lps22_pressure,&lps22_temperature);
      tempAmbientPressure=lps22_pressure.pressure;
    } while (tempSapPressureAbs <= 0.00 || tempAmbientPressure <= 0.00);
    
    tempOffsetPressure = tempSapPressureAbs - tempAmbientPressure;    //Calculate offset value which is the difference between main and reference pressure 
    ambientPressure=tempAmbientPressure;                         //Set the reference value 
  }  //If pressure mode is absolute  
  else if(pressureMode==PRESS_MODE_ABS){
    //Keep reading until we have a valid main pressure > 0.00
    do{
      tempSapPressureAbs = lps35hw.readPressure();
    } while (tempSapPressureAbs <= 0.00);

    tempOffsetPressure=0.00;                         //Set offset value to zero
    ambientPressure=tempSapPressureAbs;                       //Set the reference value which is the main pressure reading when no sip or puff is performed 
  }
   else{
    
  }
  if (USB_DEBUG) {Serial.print("USBDEBUG: tempOffsetPresssure: ");
    Serial.println(tempOffsetPressure);  }
  return tempOffsetPressure;                         //Return offset value
}

//*********************************//
// Function   : setOffsetPressure 
// 
// Description: Set the offset pressure value (hPa)
//              It's same as getOffsetPressure, but it sets offsetPressure as well
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::setOffsetPressure() {
  offsetPressure = getOffsetPressure();
}

//*********************************//
// Function   : setZeroPressure 
// 
// Description: Set the offset pressure value (hPa) and the reference pressure 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::setZeroPressure() {
  offsetPressure = 0.00;
  for (int i = 0 ; i < PRESS_BUFF_SIZE ; i++){        //Set the offsetPressure equal to average offset values in pressure buffer  
    offsetPressure += getOffsetPressure();
  }
  offsetPressure = (offsetPressure / PRESS_BUFF_SIZE);
}


//*********************************//
// Function   : setThreshold 
// 
// Description: Set the sip and puff pressure thresholds in hPa
// Arguments :  s : float : Sip pressure threshold
//              p : float : Puff pressure threshold
// 
// Return     : void
//*********************************//
void LSPressure::setThreshold(float s, float p){
  sipThreshold = s;
  puffThreshold = p;
}

//*********************************//
// Function   : setSipThreshold 
// 
// Description: Set sip pressure threshold in hPa
// Arguments :  s : float : Sip pressure threshold
// 
// Return     : void
//*********************************//
void LSPressure::setSipThreshold(float s){
  sipThreshold = s;
}

//*********************************//
// Function   : setPuffThreshold 
// 
// Description: Set puff pressure threshold in hPa
// Arguments :  p : float : Puff pressure threshold
// 
// Return     : void
//*********************************//
void LSPressure::setPuffThreshold(float p){
  puffThreshold = p;
}

//*********************************//
// Function   : update 
// 
// Description: Update pressure buffer , and sip&puff buffer
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::update() {
  updatePressure();
  updateState();
}

//*********************************//
// Function   : updatePressure 
// 
// Description: Update pressure buffer
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::updatePressure() {
  
  sapPressureAbs = lps35hw.readPressure();        //Update main pressure value 

  //If pressure mode is differential  
  if(pressureMode==PRESS_MODE_DIFF) {
    lps22.getEvent(&lps22_pressure, &lps22_temperature); 
    float tempAmbientPressure = lps22_pressure.pressure;  //Set a temporary reference value to new reference pressure reading 
    //Update offset pressure value if reference pressure is changed using tolerance value 
    if(abs(ambientPressure-tempAmbientPressure)>=refTolVal && tempAmbientPressure > 0.00){ 
        offsetPressure+=ambientPressure-tempAmbientPressure;                //Add the reference pressure change to the offset value 

        ambientPressure=tempAmbientPressure; 
      }    
      //if(tempAmbientPressure > 0.00) { ambientPressure=tempAmbientPressure; } // Update the reference pressure value 
   };

  
  //Make sure pressure readings are valid 
  if(sapPressureAbs > 0.00 && ambientPressure > 0.00){
    sapPressure = sapPressureAbs - ambientPressure - offsetPressure;                    //Calculate the pressure difference 
    pressureBuffer.pushElement({sapPressureAbs, ambientPressure, sapPressure});  //Push new pressure values to pressure buffer 
  }
 
}

//*********************************//
// Function   : updateState 
// 
// Description: Update the sip and puff state buffer
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::updateState() {
  mainStateTimer.run();                       //Update main state timer
  sapPrevState = sapBuffer.getLastElement();  //Get the previous state
  
  float pressureValue = getSapPressure();    //Get the current pressure difference 
  //check for sip and puff conditions
  if (pressureValue > puffThreshold)  { 
    sapMainState = PRESS_SAP_MAIN_STATE_PUFF;
  } else if (pressureValue < -1*sipThreshold)  { 
    sapMainState = PRESS_SAP_MAIN_STATE_SIP;
  } else {
    sapMainState = PRESS_SAP_MAIN_STATE_NONE;
  }

  //Update the state using logic
  //None:None, Sip:Sip, Puff:Puff
  if(sapPrevState.mainState == sapMainState){ // Current state is same as previous
    sapCurrState = {sapMainState, sapPrevState.secondaryState, mainStateTimer.elapsedTime(sapStateTimerId)};
    //if (USB_DEBUG) { Serial.println("a");}
    sapBuffer.updateLastElement(sapCurrState);
  } else {  //None:Sip , None:Puff , Sip:None, Puff:None 
      //State: Sip or puff
      //Previous state: {none, waiting, time} Note: There can't be sip or puff and waiting 
      //New state: {Sip or puff, started, 0}
      if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_WAITING){
        sapCurrState = {sapMainState, PRESS_SAP_SEC_STATE_STARTED, 0};
        //if (USB_DEBUG) { Serial.println("b");}
      } 
      //State: none
      //Previous state: {Sip or puff, started, time} Note: There can't be none and started 
      //New state: {Sip or puff, released, time}      
      else if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_STARTED){
        sapCurrState = {sapPrevState.mainState, PRESS_SAP_SEC_STATE_RELEASED, sapPrevState.elapsedTime};
        //if (USB_DEBUG) { Serial.println("c");}
        //Serial.println(ambientPressure);
      }
      //State: None
      //Previous state: {Sip or puff, released, time}
      //New state: {none, waiting, 0}
      else if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_RELEASED && sapMainState==PRESS_SAP_MAIN_STATE_NONE){
        sapCurrState = {sapMainState, PRESS_SAP_SEC_STATE_WAITING, 0};
        //if (USB_DEBUG) { Serial.println("d");}
      }
      //State: Sip or puff
      //Previous state: {none, released, time}
      //New state: {Sip or puff, started, 0}
      else if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_RELEASED && sapMainState!=PRESS_SAP_MAIN_STATE_NONE){
        sapCurrState = {sapMainState, PRESS_SAP_SEC_STATE_STARTED, 0};
        //if (USB_DEBUG) { Serial.println("e");}
      }      
      //Push the new state   
      sapBuffer.pushElement(sapCurrState);
      //Reset and start the timer
      mainStateTimer.restartTimer(sapStateTimerId);  
  }

  //No action in 1 minute : reset timer
  if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_WAITING && mainStateTimer.elapsedTime(sapStateTimerId)>PRESS_SAP_ACTION_TIMEOUT){
      setZeroPressure();                                   //Update pressure offset value 
      //Reset and start the timer    
      mainStateTimer.restartTimer(sapStateTimerId);   
  }
}

//*********************************//
// Function   : getSapPressureAbs 
// 
// Description: Get the last main pressure reading from the pressure buffer 
// Arguments :  void
// 
// Return     : pressure : float : Last main pressure reading
//*********************************//
float LSPressure::getSapPressureAbs() {
  return pressureBuffer.getLastElement().sapPressureAbs;
}

//*********************************//
// Function   : getAmbientPressure 
// 
// Description: Get the last reference pressure reading from the pressure buffer 
// Arguments :  void
// 
// Return     : pressure : float : Last reference pressure reading
//*********************************//
float LSPressure::getAmbientPressure() {
  return pressureBuffer.getLastElement().ambientPressure;
}

//*********************************//
// Function   : getAmbientPressure 
// 
// Description: Get the last pressure difference from the pressure buffer 
// Arguments :  void
// 
// Return     : pressure : float : Last pressure difference from pressure buffer 
//*********************************//
float LSPressure::getSapPressure() {
  return pressureBuffer.getLastElement().sapPressure;
}


//*********************************//
// Function   : getAllPressureure 
// 
// Description: Get the last pressure readings including the main ,reference, and difference pressure values from the pressure buffer 
// Arguments :  void
// 
// Return     : pressure : pressureStruct : Last pressure values from pressure buffer 
//*********************************//
pressureStruct LSPressure::getAllPressure() {
  return pressureBuffer.getLastElement();
}


//*********************************//
// Function   : getState 
// 
// Description: Get the last sip and puff state from sap buffer 
// Arguments :  void
// 
// Return     : state : inputStateStruct : Last sip and puff state
//*********************************//
inputStateStruct LSPressure::getState(){
  return sapBuffer.getLastElement();
}


#endif 
