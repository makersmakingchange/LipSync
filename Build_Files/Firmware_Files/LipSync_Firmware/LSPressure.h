/* 
* File: LSPressure.h
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

// Header definition
#ifndef _LSPRESSURE_H
#define _LSPRESSURE_H

#include <Adafruit_LPS35HW.h>   // Tube pressure sensor
#include <Adafruit_LPS2X.h>     // Ambient pressure sensor
#include <Adafruit_Sensor.h>     // Generic sensor

#define LPS22_I2CADDR 0x5C      // Modified LPS22 address

#define PRESS_BUFF_SIZE 5       // The size of pressure Buffer
#define PRESS_SAP_BUFF_SIZE 12  // The size of sip and puff state buffer 

#define PRESS_REF_TOLERANCE 0.1   // The change in reference pressure (hPa) that would initiate reference pressure update 
                                  // It's only used in differential mode

#define PRESS_SAP_DEFAULT_THRESHOLD 3.0 // hPa hecto Pascals

#define PRESS_FILTER_NONE 0
#define PRESS_FILTER_AVERAGE 1

#define PRESS_MODE_NONE 0
#define PRESS_MODE_ABS 1              // Absolute pressure mode
#define PRESS_MODE_DIFF 2             // Differential pressure mode

#define PRESS_MODE_MIN 1              // Minimum and maximum mode numbers 
#define PRESS_MODE_MAX 2              // Used for switching pressure mode

// Sip and puff secondary states 
#define PRESS_SAP_SEC_STATE_WAITING 0   // Waiting : No sip or puff
#define PRESS_SAP_SEC_STATE_STARTED 1   // Started : Sip or puff being performed  (Off->On)
#define PRESS_SAP_SEC_STATE_RELEASED 2  // Released : Sip or puff was just released (On->Off)

#define PRESS_SAP_ACTION_TIMEOUT 60000  // Reset timer
#define PRESS_SAP_SENSOR_TIMEOUT 3000 // Timeout for sensor reading

// Pressure structure 
typedef struct {
  float sapPressureAbs;                 // Stand-alone I2C Sensor reading [hPa]
  float ambientPressure;                // Ambient Pressure [hPa]
  float sapPressure;                    // Pressure difference used in sip and puff processing [hPa]
} pressureStruct;

extern bool g_mouthpiecePressureSensorConnected;  // Mouthpiece pressure sensor connection state
extern bool g_ambientPressureSensorConnected;     // Ambient pressure sensor connection state


class LSPressure {
  public:
    LSPressure();                                       // Constructor
    void begin();                                    
    void clear();  
    void update();                                      // Update the pressure buffer and sip and puff buffer with new readings 
    void setFilterMode(int mode);                       // Set filter Mode: None or Average
    void setPressureMode(int mode);                     // Set Sip and Puff Pressure Mode : ABS or DIFF pressure
    int getPressureMode();                              // Get Sip and Puff Pressure Mode : ABS or DIFF pressure
    void setRefTolerance(float value);                  // Set reference pressure change tolerance value that is used to update the reference pressure
    float getRefTolerance();                            // Get reference pressure change tolerance value that is used to update the reference pressure
    float getOffsetPressure();                          // Get Offset pressure: (sapPressureAbs- ambientPressure)
    void setOffsetPressure(float offsetPressure);       // Set Offset pressure: 
    void updateOffsetPressure();                        // Zero the base reference pressure and update Offset pressure value 
    float measureOffsetPressure();                      // Measure the offset pressure between pressure sensors: offsetPressure = (sapPressureAbs- ambientPressure)
    void setSipThreshold(float s);                      // Set sip threshold
    void setPuffThreshold(float p);                     // Set puff threshold
    void updatePressure();                              // Update the pressure buffer with new readings 
    void updateState();                                 // Update the and puff buffer with new states 
    float getSapPressureAbs();                          // Get last main pressure from pressure buffer
    float getAmbientPressure();                         // Get last reference pressure from pressure buffer
    float getSapPressure();                             // Get the Pressure Difference sapPressure = (sapPressureAbs- ambientPressure- offsetPressure)
    pressureStruct getAllPressure();                    // Get the latest pressure values 
    inputStateStruct getState();                        // Get the latest sip and puff state  

  private: 
      Adafruit_LPS35HW _lps35hw = Adafruit_LPS35HW();     // Create an object of Adafruit_LPS35HW class for Sip and Puff pressure
      Adafruit_LPS22 _lps22;                              // Create an object of Adafruit_LPS2X class for ambient pressure
      sensors_event_t _lps22Pressure;                     // Ambient temperature event object 
      sensors_event_t _lps22Temperature;                  // Ambient pressure event object                           
      LSCircularBuffer <pressureStruct> _pressureBuffer;  // Create a buffer of type pressureStruct to push pressure readings 
      LSCircularBuffer <inputStateStruct> _sapBuffer;     // Create a buffer of type inputStateStruct to push sap states 
      int _filterMode;                                    // Filter Mode : NONE or AVERAGE     
      int _pressureMode;                                  // Pressure Mode: DIFF or ABS pressure 
      float _sapPressureAbs;                              // Main Pressure reading (Sip and Puff Absolute) [hPa]
      float _ambientPressure;                             // Reference Pressure reading (Ambient Absolute) [hPa]
      float _offsetPressure;                              // Offset Pressure [hPa] (Difference between two sensors)
      float _sapPressure;                                 // Calculated Pressure Difference sapPressure = (sapPressureAbs- ambientPressure- offsetPressure)
      float _refTolVal;                                   // The tolerance value in hPa used to check and update reference pressure 
      LSTimer <void> _sapStateTimer;                      // Timer used to measure time for each sip and puff action. 
      int _sapStateTimerId;                                // The id for the sap state timer
      inputStateStruct _sapCurrState;                      // The current state of sip and puff
      inputStateStruct _sapPrevState;                      // The previous state of sip and puff
      float _sipThreshold;                                 // Sip Threshold 
      float _puffThreshold;                                // Puff Threshold 
      int _sapMainState;                                   // The value which represents the current main state (example: PRESS_SAP_MAIN_STATE_PUFF) 
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
LSPressure::LSPressure()
{
  _pressureBuffer.begin(PRESS_BUFF_SIZE);        // Initialize Pressure Buffer
  _sapBuffer.begin(PRESS_SAP_BUFF_SIZE);         // Initialize Sip and Puff Action Buffer 
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
void LSPressure::begin()
{
  // lps35hw Pressure sensor setup
  if (!_lps35hw.begin_I2C())
  {
    Serial.println(F("ERROR: Couldn't find LPS35HW chip"));  // TODO 2025-Feb-21 This should also generate an error
    g_mouthpiecePressureSensorConnected = false;
  }
  else
  {
    if (USB_DEBUG) {Serial.println("USBDEBUG: LSPressure::begin LPS35HW Found."); }
    _lps35hw.setDataRate(LPS35HW_RATE_25_HZ);  // Options: 1 Hz, 10Hz, 25Hz, 50Hz, 75Hz
  }

  // LPS22 Pressure sensor setup
  if (!_lps22.begin_I2C(LPS22_I2CADDR))
  {
    Serial.println(F("ERROR: Couldn't find LPS22 chip"));  // TODO 2025-Feb-21 This should also generate an error
    g_ambientPressureSensorConnected = false;
  } 
  else
  {
    if (USB_DEBUG) {Serial.println("USBDEBUG: LSPressure::begin LPS22 Found.");}
    _lps22.setDataRate(LPS22_RATE_25_HZ);         // Options: 1-shot, 
  }

  //setFilterMode(PRESS_FILTER_NONE);         // Set the default filter mode to none  //  TODO 2025-Feb-22 Currently unused
  setPressureMode(PRESS_MODE_DIFF);         // Set the default pressure mode to differential mode (i.e., mouthpiece and ambient)
  setRefTolerance(PRESS_REF_TOLERANCE);     // Set the default tolerance value to update reference pressure //TODO 2025-Feb-22 No longer used?
  
  // Set default pressure thresholds
  setSipThreshold(PRESS_SAP_DEFAULT_THRESHOLD);
  setPuffThreshold(PRESS_SAP_DEFAULT_THRESHOLD);

  //delay(5);
  //_lps35hw.setDataRate(LPS35HW_RATE_25_HZ);  // Options: 1 Hz, 10Hz, 25Hz, 50Hz, 75Hz
  //_lps22.setDataRate(LPS22_RATE_25_HZ);      // Options: 1-shot, 

  clear();      // Clear buffers and make sure no sip and puff action is set as previous actions

  delay(20);
  updateOffsetPressure();    // Update the offset pressure between pressure sensors
}

//*********************************//
// Function   : clear 
// 
// Description: Clear pressure buffer and restart state timer
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::clear()
{
  while(_pressureBuffer.getLength() < PRESS_BUFF_SIZE)
  {    
    _pressureBuffer.pushElement({0.0, 0.0, 0.0});  // Clear Pressure Buffer by pushing zero readings to buffers 
  }

  // Push initial state to state Queue
  _sapCurrState = _sapPrevState = {PRESS_SAP_MAIN_STATE_NONE, PRESS_SAP_SEC_STATE_WAITING, 0};
  _sapBuffer.pushElement(_sapCurrState);

  // Reset and start the timer   
  _sapStateTimerId =  _sapStateTimer.startTimer();

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
void LSPressure::setFilterMode(int mode)
{
  _filterMode = mode;
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
int LSPressure::getPressureMode()
{
  return _pressureMode;
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
void LSPressure::setPressureMode(int mode)
{
  _pressureMode = mode;
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
void LSPressure::setRefTolerance(float inputrefTolVal)
{
  _refTolVal = inputrefTolVal;
}

//*********************************//
// Function   : getRefTolerance 
// 
// Description: Get the reference pressure change tolerance (hPa)
// 
// Arguments :  void
// 
// Return     : value : float : The pressure change in hPa
//*********************************//
float LSPressure::getRefTolerance()
{
  return _refTolVal;
}

//*********************************//
// Function   : measureOffsetPressure 
// 
// Description: Measure the offset pressure (hPa) between the mouthpiece sensor and ambient sensor
//              
// Arguments :  void
// 
// Return     : tempOffsetPressure : float : The offset pressure value in hPa
//*********************************//
float LSPressure::measureOffsetPressure()  
{
  // Initialize variables used to calculate offset value  
  float tempSapPressureAbs = 0.00;
  float tempAmbientPressure = 0.00;
  float tempOffsetPressure = 0.00;

  unsigned long pressureReadingStartTime = millis();
  unsigned long pressureReadingTime = 0;

  // If pressure mode is differential  
  if(_pressureMode == PRESS_MODE_DIFF) {
    
    do  // Keep reading until we have a valid main and reference pressure values > 0.0 
    {     
      tempSapPressureAbs = _lps35hw.readPressure();  //TODO replace with generic get function
      _lps22.getEvent(&_lps22Pressure, &_lps22Temperature);
      tempAmbientPressure = _lps22Pressure.pressure;
      pressureReadingTime = millis();
      if (pressureReadingTime - pressureReadingStartTime > PRESS_SAP_SENSOR_TIMEOUT){
        Serial.println("ERROR: Mouthpiece pressure sensor timeout");
        // TODO 2025-Feb-24 Throw error
        break;
      }
    } while (tempSapPressureAbs <= 0.00 || tempAmbientPressure <= 0.00 );
    
    tempOffsetPressure = tempSapPressureAbs - tempAmbientPressure;    // Calculate offset value which is the difference between main and reference pressure sensors
  } else if(_pressureMode == PRESS_MODE_ABS){// If pressure mode is absolute 
    
    // Keep reading until we have a valid main pressure > 0.00
    pressureReadingStartTime = millis();
    do 
    {
      tempSapPressureAbs = _lps35hw.readPressure();
      pressureReadingTime = millis();
      if (pressureReadingTime - pressureReadingStartTime > PRESS_SAP_SENSOR_TIMEOUT){
        Serial.println("ERROR: Ambient pressure sensor timeout");
        // TODO 2025-Feb-24 Throw error
        break;
      }
    } while (tempSapPressureAbs <= 0.00);

    tempOffsetPressure = 0.00;  // Set offset value to zero
    _ambientPressure = tempSapPressureAbs;  // Set the reference value which is the main pressure reading when no sip or puff is performed 
  }
  else
  {
    
  }

  return tempOffsetPressure;                         // Return offset value
}

//*********************************//
// Function   : getOffsetPressure 
// 
// Description: Return the offset pressure (hPa)

// Arguments :  void
// 
// Return     : tempOffsetPressure : float : The offset pressure value in hPa
//*********************************//
float LSPressure::getOffsetPressure()  
{
  return _offsetPressure;  // Return offset value
}

//*********************************//
// Function   : setOffsetPressure 
// 
// Description: Set the offset pressure value (hPa)
//              
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::setOffsetPressure(float offsetPressure)
{
  _offsetPressure = offsetPressure;
}

//*********************************//
// Function   : updateOffsetPressure 
// 
// Description: Measure and set the offset pressure value (hPa) 
//
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::updateOffsetPressure()
{
  float tempOffsetPressure = 0.00;

  // Measure multiple readings
  for (int i = 0 ; i < PRESS_BUFF_SIZE ; i++)
  {        
    tempOffsetPressure += measureOffsetPressure();  
  }

  // Set the offsetPressure equal to average offset values in pressure buffer
  tempOffsetPressure = (tempOffsetPressure / PRESS_BUFF_SIZE);    

  if (USB_DEBUG) {
    Serial.print("updateOffsetPressure(): Offset Pressure: ");
    Serial.print(tempOffsetPressure);
    Serial.println(" hPa");
  }

  setOffsetPressure(tempOffsetPressure);
}


//*********************************//
// Function   : setSipThreshold 
// 
// Description: Set sip pressure threshold in hPa
// Arguments :  s : float : Sip pressure threshold
// 
// Return     : void
//*********************************//
void LSPressure::setSipThreshold(float s)
{
  _sipThreshold = s;
}

//*********************************//
// Function   : setPuffThreshold 
// 
// Description: Set puff pressure threshold in hPa
//
// Arguments :  p : float : Puff pressure threshold
// 
// Return     : void
//*********************************//
void LSPressure::setPuffThreshold(float p)
{
  _puffThreshold = p;
}

//*********************************//
// Function   : update 
// 
// Description: Update pressure buffer , and sip&puff buffer
//
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::update()
{
  updatePressure();
  updateState();
}

//*********************************//
// Function   : updatePressure 
// 
// Description: Update pressure buffer
//
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::updatePressure()
{
  
  _sapPressureAbs = _lps35hw.readPressure();        // Update mouthpiece pressure value 

  // If pressure mode is differential  
  if(_pressureMode == PRESS_MODE_DIFF) {
    _lps22.getEvent(&_lps22Pressure, &_lps22Temperature); 
    float tempAmbientPressure = _lps22Pressure.pressure;  // Set a temporary reference value to new reference pressure reading 
    
    // Update offset pressure value if reference pressure is changed using tolerance value 
    /*  // TODO 2025-Feb-22 If not used, delete
    if(abs(ambientPressure - tempAmbientPressure) >= refTolVal && tempAmbientPressure > 0.00){ 
        //offsetPressure += ambientPressure - tempAmbientPressure;                // Add the reference pressure change to the offset value 
        //ambientPressure = tempAmbientPressure; 
      }  
      */  
    if(tempAmbientPressure > 0.00) { // If ambient pressure is valid, update the ambient pressure value 
      _ambientPressure = tempAmbientPressure; 
    } 
  }

    // Make sure pressure readings are valid 
  if(_sapPressureAbs > 0.00 && _ambientPressure > 0.00) {
    _sapPressure = _sapPressureAbs - _ambientPressure - _offsetPressure;              // Calculate the pressure difference 
    _pressureBuffer.pushElement({_sapPressureAbs, _ambientPressure, _sapPressure});   // Push new pressure values to pressure buffer 

  }
 
}

//*********************************//
// Function   : updateState 
// 
// Description: Update the sip and puff state buffer
//
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::updateState()  //  TODO 2025-Feb-22 This code should be abstracted out as inputHandler as it is repeated for buttons and switches
{
  _sapStateTimer.run();                       // Update main state timer
  _sapPrevState = _sapBuffer.getLastElement();  // Get the previous state
  
  float pressureValue = getSapPressure();    // Get latest pressure value

  // check for sip and puff conditions
  if (pressureValue > _puffThreshold) {
    _sapMainState = PRESS_SAP_MAIN_STATE_PUFF; // Puff detected
  }
  else if (pressureValue < -1 * _sipThreshold) {

    _sapMainState = PRESS_SAP_MAIN_STATE_SIP; // Sip detected
  }
  else {                                      // Neither sip nor puff detected
    _sapMainState = PRESS_SAP_MAIN_STATE_NONE;
  }

  // Update the state using logic
  // None:None, Sip:Sip, Puff:Puff
  if(_sapPrevState.mainState == _sapMainState){ // Current state is same as previous
    _sapCurrState = {_sapMainState, _sapPrevState.secondaryState, _sapStateTimer.elapsedTime(_sapStateTimerId)};
    _sapBuffer.updateLastElement(_sapCurrState);
  } 
  else {  // None:Sip , None:Puff , Sip:None, Puff:None 
      // State: Sip or puff
      // Previous state: {none, waiting, time} Note: There can't be sip or puff and waiting 
      // New state: {Sip or puff, started, 0}
      if(_sapPrevState.secondaryState == PRESS_SAP_SEC_STATE_WAITING) {
        _sapCurrState = {_sapMainState, PRESS_SAP_SEC_STATE_STARTED, 0};
      } 
      // State: none
      // Previous state: {Sip or puff, started, time} Note: There can't be none and started 
      // New state: {Sip or puff, released, time}      
      else if(_sapPrevState.secondaryState == PRESS_SAP_SEC_STATE_STARTED){
        _sapCurrState = {_sapPrevState.mainState, PRESS_SAP_SEC_STATE_RELEASED, _sapPrevState.elapsedTime};
      }
      // State: None
      // Previous state: {Sip or puff, released, time}
      // New state: {none, waiting, 0}
      else if(_sapPrevState.secondaryState == PRESS_SAP_SEC_STATE_RELEASED && _sapMainState == PRESS_SAP_MAIN_STATE_NONE) {
        _sapCurrState = {_sapMainState, PRESS_SAP_SEC_STATE_WAITING, 0};
      }
      // State: Sip or puff
      // Previous state: {none, released, time}
      // New state: {Sip or puff, started, 0}
      else if(_sapPrevState.secondaryState == PRESS_SAP_SEC_STATE_RELEASED && _sapMainState != PRESS_SAP_MAIN_STATE_NONE) {
        _sapCurrState = {_sapMainState, PRESS_SAP_SEC_STATE_STARTED, 0};
      }      
      // Push the new state   
      _sapBuffer.pushElement(_sapCurrState);
      // Reset and start the timer
      _sapStateTimer.restartTimer(_sapStateTimerId);  
  }

  // No action in 1 minute : reset timer
  //if(_sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_WAITING && _sapStateTimer.elapsedTime(_sapStateTimerId)>PRESS_SAP_ACTION_TIMEOUT){  //TODO 2025-Feb-22 Remove?
  if(_sapStateTimer.elapsedTime(_sapStateTimerId) > PRESS_SAP_ACTION_TIMEOUT) {
      updateOffsetPressure();   // Update pressure offset value   
      _sapStateTimer.restartTimer(_sapStateTimerId);    // Reset and start the timer      
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
float LSPressure::getSapPressureAbs()
{
  return _pressureBuffer.getLastElement().sapPressureAbs;
}

//*********************************//
// Function   : getAmbientPressure 
// 
// Description: Get the last reference pressure reading from the pressure buffer 
// Arguments :  void
// 
// Return     : pressure : float : Last reference pressure reading
//*********************************//
float LSPressure::getAmbientPressure()
{
  return _pressureBuffer.getLastElement().ambientPressure;
}

//*********************************//
// Function   : getAmbientPressure 
// 
// Description: Get the last pressure difference from the pressure buffer 
// Arguments :  void
// 
// Return     : pressure : float : Last pressure difference from pressure buffer 
//*********************************//
float LSPressure::getSapPressure()
{
  return _pressureBuffer.getLastElement().sapPressure;
}


//*********************************//
// Function   : getAllPressureure 
// 
// Description: Get the last pressure readings including the main ,reference, and difference pressure values from the pressure buffer 
// Arguments :  void
// 
// Return     : pressure : pressureStruct : Last pressure values from pressure buffer 
//*********************************//
pressureStruct LSPressure::getAllPressure()
{
  return _pressureBuffer.getLastElement();
}


//*********************************//
// Function   : getState 
// 
// Description: Get the last sip and puff state from sap buffer 
// Arguments :  void
// 
// Return     : state : inputStateStruct : Last sip and puff state
//*********************************//
inputStateStruct LSPressure::getState()
{
  return _sapBuffer.getLastElement();
}


#endif 
