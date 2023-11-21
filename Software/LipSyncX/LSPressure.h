/* 
* File: LSPressure.h
* Firmware: LipSync4
* Developed by: MakersMakingChange
* Version: Alpha 2 (14 April 2022) 
* Copyright Neil Squire Society 2022. 
* License: This work is licensed under the CC BY SA 4.0 License: http://creativecommons.org/licenses/by-sa/4.0 .
*/

//Header definition
#ifndef _LSPRESSURE_H
#define _LSPRESSURE_H

#include <Adafruit_LPS35HW.h>   // Stand-alone i2C Pressure Sensor
#include <Adafruit_BMP280.h>    // Pressure Sensor onboard Feather nRF5285280 Sense

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

//Sip and puff main states 
#define PRESS_SAP_MAIN_STATE_NONE 0   //No action 
#define PRESS_SAP_MAIN_STATE_SIP 1    //Sip action 
#define PRESS_SAP_MAIN_STATE_PUFF 2   //Puff action 

//Sip and puff secondary states 
#define PRESS_SAP_SEC_STATE_WAITING 0   //Waiting : No sip or puff
#define PRESS_SAP_SEC_STATE_STARTED 1   //Started : Sip or puff being performed 
#define PRESS_SAP_SEC_STATE_RELEASED 2  //Released : Sip or puff was just released 

#define PRESS_SAP_ACTION_TIMEOUT 60000  //Rest timer

//Pressure structure 
typedef struct {
  float mainPressure;     //Stand-alone I2C Sensor reading 
  float refPressure;      //Onboard Sensor reading in differential mode
  float diffPressure;     //Pressure difference used in sip and puff processing 
} pressureStruct;


class LSPressure {
  private: 
    Adafruit_LPS35HW lps35hw = Adafruit_LPS35HW();        //Create an object of Adafruit_LPS35HW class
    Adafruit_LPS22 lps22;                                 //Create an object of Adafruit_LPS2X class
    Adafruit_Sensor *lps22_pressure = lps22.getPressureSensor(); // Retrieve pressure sensor from LPS22
    LSCircularBuffer <pressureStruct> pressureBuffer;   //Create a buffer of type pressureStruct to push pressure readings 
    LSCircularBuffer <inputStateStruct> sapBuffer;      //Create a buffer of type inputStateStruct to push sap states 
    int filterMode;                                     //Filter Mode : NONE or AVERAGE     
    int pressureMode;                                   //Pressure Mode: DIFF or ABS pressure 
    float mainVal;                                      //Main Pressure reading
    float refVal;                                       //Reference Pressure reading
    float compVal;                                      //Compensation Pressure
    float diffVal;                                      //Calculated Pressure Difference diffVal = (mainVal- refVal- compVal)
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
    float getCompPressure();                            //Get Compensation pressure (mainVal- refVal)
    void setCompPressure();                             //Set Compensation pressure compVal = (mainVal- refVal)
    void setZeroPressure();                             //Zero the base reference pressure and update Compensation pressure value 
    void setThreshold(float s, float p);                //Set sip and puff thresholds
    void setSipThreshold(float s);                      //Set sip threshold
    void setPuffThreshold(float p);                     //Set puff threshold
    void update();                                      //Update the pressure buffer and sip and puff buffer with new readings 
    void updatePressure();                              //Update the pressure buffer with new readings 
    void updateState();                                 //Update the and puff buffer with new states 
    float getMainPressure();                            //Get last main pressure from pressure buffer
    float getRefPressure();                             //Get last reference pressure from pressure buffer
    float getDiffPressure();                            //Get the Pressure Difference diffVal = (mainVal- refVal- compVal)
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

  //BMP280 Pressure sensor setups
  if (!lps35hw.begin_I2C()) {
    Serial.println(F("Couldn't find LPS35HW chip"));
  }
  //BMP280 Onboard Pressure sensor setups
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor"));
    while (1) delay(10);
  } 

  setFilterMode(PRESS_FILTER_NONE);         //Set the default filter mode to none

  setPressureMode(PRESS_MODE_DIFF);         //Set the default pressure mode to differential mode

  setRefTolerance(PRESS_REF_TOLERANCE);     //Set the default tolerance value to update reference pressure 

  setThreshold(PRESS_SAP_DEFAULT_THRESHOLD,PRESS_SAP_DEFAULT_THRESHOLD); //Set the default sip and puff thresholds

  lps35hw.setDataRate(LPS35HW_RATE_25_HZ);          //Options: 1 Hz, 10Hz, 25Hz, 50Hz, 75Hz

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     //Operating Mode
                Adafruit_BMP280::SAMPLING_NONE,     //Temperature oversampling
                Adafruit_BMP280::SAMPLING_X4,       //Pressure oversampling 
                Adafruit_BMP280::FILTER_X16,        //Filtering. 
                Adafruit_BMP280::STANDBY_MS_1000);  //Standby time in ms.       

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
// Function   : getCompPressure 
// 
// Description: Get the compensation pressure value (hPa)
//              It doesn't set compVal
// Arguments :  void
// 
// Return     : tempCompVal : float : The compensation pressure value in hPa
//*********************************//
float LSPressure::getCompPressure() {

  //Initialize variables used to calculate compensation value  
  float tempMainVal = 0.00;
  float tempRefVal = 0.00;
  float tempCompVal = 0.00;

  //If pressure mode is differential  
  if(pressureMode==PRESS_MODE_DIFF){
    //Keep reading until we have a valid main and reference pressure values > 0.0
    do{     
      tempMainVal = lps35hw.readPressure();
      bmp_pressure->getEvent(&pressure_event);
      tempRefVal=pressure_event.pressure;
    } while (tempMainVal <= 0.00 || tempRefVal <= 0.00);
    
    tempCompVal = tempMainVal - tempRefVal;    //Calculate compensation value which is the difference between main and reference pressure 
    refVal=tempRefVal;                         //Set the reference value 
  }  //If pressure mode is absolute  
  else if(pressureMode==PRESS_MODE_ABS){
    //Keep reading until we have a valid main pressure > 0.00
    do{
      tempMainVal = lps35hw.readPressure();
    } while (tempMainVal <= 0.00);

    tempCompVal=0.00;                         //Set compensation value to zero
    refVal=tempMainVal;                       //Set the reference value which is the main pressure reading when no sip or puff is performed 
  }
  return tempCompVal;                         //Return compensation value
}

//*********************************//
// Function   : setCompPressure 
// 
// Description: Set the compensation pressure value (hPa)
//              It's same as getCompPressure, but it sets compVal as well
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::setCompPressure() {
  compVal = getCompPressure();
}

//*********************************//
// Function   : setZeroPressure 
// 
// Description: Set the compensation pressure value (hPa) and the reference pressure 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSPressure::setZeroPressure() {
  compVal = 0.00;
  for (int i = 0 ; i < PRESS_BUFF_SIZE ; i++){        //Set the compVal equal to average compensation values in pressure buffer  
    compVal += getCompPressure();
  }
  compVal = (compVal / PRESS_BUFF_SIZE);
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
  
  mainVal = lps35hw.readPressure();        //Update main pressure value 

  //If pressure mode is differential  
  if(pressureMode==PRESS_MODE_DIFF) {
    bmp_pressure->getEvent(&pressure_event); 
    float tempRefVal = pressure_event.pressure;  //Set a temporary reference value to new reference pressure reading 
    //Update compensation pressure value if reference pressure is changed using tolerance value 
    if(abs(refVal-tempRefVal)>=refTolVal && tempRefVal > 0.00){ 
        //compVal+=refVal-tempRefVal;                //Add the reference pressure change to the compensation value 
        compVal=mainVal-tempRefVal;               //Update the compensation value 
      }    
      if(tempRefVal > 0.00) { refVal=tempRefVal; } // Update the reference pressure value 
   };

  
  //Make sure pressure readings are valid 
  if(mainVal > 0.00 && refVal > 0.00){
    diffVal = mainVal - refVal - compVal;                    //Calculate the pressure difference 
    pressureBuffer.pushElement({mainVal, refVal, diffVal});  //Push new pressure values to pressure buffer 
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
  float pressureValue = getDiffPressure();    //Get the current pressure difference 
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
  if(sapPrevState.mainState == sapMainState){
    sapCurrState = {sapMainState, sapPrevState.secondaryState, mainStateTimer.elapsedTime(sapStateTimerId)};
    //Serial.println("a");
    sapBuffer.updateLastElement(sapCurrState);
  } else {  //None:Sip , None:Puff , Sip:None, Puff:None
      //State: Sip or puff
      //Previous state: {none, waiting, time} Note: There can't be sip or puff and waiting 
      //New state: {Sip or puff, started, 0}
      if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_WAITING){
        sapCurrState = {sapMainState, PRESS_SAP_SEC_STATE_STARTED, 0};
        //Serial.println("b");
      } 
      //State: none
      //Previous state: {Sip or puff, started, time} Note: There can't be none and started 
      //New state: {Sip or puff, released, time}      
      else if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_STARTED){
        sapCurrState = {sapPrevState.mainState, PRESS_SAP_SEC_STATE_RELEASED, sapPrevState.elapsedTime};
        //Serial.println("c");
        //Serial.println(refVal);
      }
      //State: None
      //Previous state: {Sip or puff, released, time}
      //New state: {none, waiting, 0}
      else if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_RELEASED && sapMainState==PRESS_SAP_MAIN_STATE_NONE){
        sapCurrState = {sapMainState, PRESS_SAP_SEC_STATE_WAITING, 0};
        //Serial.println("d");
      }
      //State: Sip or puff
      //Previous state: {none, released, time}
      //New state: {Sip or puff, started, 0}
      else if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_RELEASED && sapMainState!=PRESS_SAP_MAIN_STATE_NONE){
        sapCurrState = {sapMainState, PRESS_SAP_SEC_STATE_STARTED, 0};
        //Serial.println("e");
      }      
      //Push the new state   
      sapBuffer.pushElement(sapCurrState);
      //Reset and start the timer
      mainStateTimer.restartTimer(sapStateTimerId);  
  }

  //No action in 1 minute : reset timer
  if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_WAITING && mainStateTimer.elapsedTime(sapStateTimerId)>PRESS_SAP_ACTION_TIMEOUT){
      setZeroPressure();                                   //Update pressure compensation value 
      //Reset and start the timer    
      mainStateTimer.restartTimer(sapStateTimerId);   
  }
}

//*********************************//
// Function   : getMainPressure 
// 
// Description: Get the last main pressure reading from the pressure buffer 
// Arguments :  void
// 
// Return     : pressure : float : Last main pressure reading
//*********************************//
float LSPressure::getMainPressure() {
  return pressureBuffer.getLastElement().mainPressure;
}

//*********************************//
// Function   : getRefPressure 
// 
// Description: Get the last reference pressure reading from the pressure buffer 
// Arguments :  void
// 
// Return     : pressure : float : Last reference pressure reading
//*********************************//
float LSPressure::getRefPressure() {
  return pressureBuffer.getLastElement().refPressure;
}

//*********************************//
// Function   : getRefPressure 
// 
// Description: Get the last pressure difference from the pressure buffer 
// Arguments :  void
// 
// Return     : pressure : float : Last pressure difference from pressure buffer 
//*********************************//
float LSPressure::getDiffPressure() {
  return pressureBuffer.getLastElement().diffPressure;
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
