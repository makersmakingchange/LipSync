//Header definition
#ifndef _LSPRESSURE_H
#define _LSPRESSURE_H


#include <Adafruit_LPS35HW.h>
#include <Adafruit_BMP280.h>   //onboard pressure sensor on Adafruit Sense Micro 

#define PRESS_BUFF_SIZE 5
#define PRESS_SAP_BUFF_SIZE 12

#define PRESS_REF_TOLERANCE 0.1

#define PRESS_SAP_DEFAULT_THRESHOLD 2.5 

#define PRESS_FILTER_NONE 0
#define PRESS_FILTER_AVERAGE 1

#define PRESS_TYPE_ABS 0
#define PRESS_TYPE_DIFF 1

#define PRESS_SAP_MAIN_STATE_NONE 0
#define PRESS_SAP_MAIN_STATE_SIP 1
#define PRESS_SAP_MAIN_STATE_PUFF 2

#define PRESS_SAP_SEC_STATE_WAITING 0
#define PRESS_SAP_SEC_STATE_STARTED 1
#define PRESS_SAP_SEC_STATE_RELEASED 2

typedef struct {
  float mainPressure;
  float refPressure;
  float diffPressure;
} pressureStruct;


class LSPressure {
  private: 
    Adafruit_LPS35HW lps35hw = Adafruit_LPS35HW();
    Adafruit_BMP280 bmp; // use I2C interface
    Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
    LSCircularBuffer <pressureStruct> pressureBuffer;
    LSCircularBuffer <inputStateStruct> sapBuffer;   //Create a buffer of type inputStateStruct
    int filterMode;
    int pressureType;
    float mainVal;
    float refVal;
    float compVal;
    float diffVal;
    sensors_event_t pressure_event;
    float refTolVal;
    LSTimer <void> mainStateTimer;
    int sapStateTimerId;
    inputStateStruct sapCurrState;
    inputStateStruct sapPrevState;
    float sipThreshold;
    float puffThreshold;
    int sapMainState;
  public:
    LSPressure();
    void begin(int type);                                    
    void clear();  
    void setFilterMode(int mode); 
    void setRefTolerance(float value); 
    float getCompPressure();
    void setCompPressure();
    void setZeroPressure();
    void setStateThreshold(float s, float p);
    void update();    
    void updatePressure();
    void updateState();
    float getMainPressure();
    float getRefPressure();
    float getDiffPressure();
    pressureStruct getAllPressure();
    inputStateStruct getState();
};

LSPressure::LSPressure() {
  pressureBuffer.begin(PRESS_BUFF_SIZE);
  sapBuffer.begin(PRESS_SAP_BUFF_SIZE);  
}

void LSPressure::begin(int type) {

  //BMP280 Pressure sensor setups
  if (!lps35hw.begin_I2C()) {
    Serial.println(F("Couldn't find LPS35HW chip"));
  }
  //BMP280 Onboard Pressure sensor setups
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor"));
    while (1) delay(10);
  } 

  pressureType = type;

  filterMode = PRESS_FILTER_NONE;

  refTolVal = PRESS_REF_TOLERANCE;

  sipThreshold = -PRESS_SAP_DEFAULT_THRESHOLD;

  puffThreshold = PRESS_SAP_DEFAULT_THRESHOLD;

  lps35hw.setDataRate(LPS35HW_RATE_25_HZ);  // 1,10,25,50,75

  if(pressureType==PRESS_TYPE_DIFF){
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode. 
                  Adafruit_BMP280::SAMPLING_NONE,     // Temp. oversampling
                  Adafruit_BMP280::SAMPLING_X4,    // Pressure oversampling 
                  Adafruit_BMP280::FILTER_X16,      // Filtering. 
                  Adafruit_BMP280::STANDBY_MS_1000); // Standby time.       
  }
  clear();

  setZeroPressure();
}

void LSPressure::clear() {

  while(pressureBuffer.getLength()<PRESS_BUFF_SIZE){
    pressureBuffer.pushElement({0.0, 0.0, 0.0});   
  }

    //Push initial state to state Queue
  
  sapCurrState = sapPrevState = {PRESS_SAP_MAIN_STATE_NONE, PRESS_SAP_SEC_STATE_WAITING, 0};
  sapBuffer.pushElement(sapCurrState);

  //Reset and start the timer   
  sapStateTimerId =  mainStateTimer.startTimer();

}

void LSPressure::setFilterMode(int mode) {
  filterMode = mode;
}

void LSPressure::setRefTolerance(float value) {
  refTolVal = value;
}

float LSPressure::getCompPressure() {

  float tempMainVal = 0.00;
  float tempRefVal = 0.00;
  float tempCompVal = 0.00;

  if(pressureType==PRESS_TYPE_DIFF){
    //Keep reading until we have a valid pressure > 0.0
    do{     
      tempMainVal = lps35hw.readPressure();
      bmp_pressure->getEvent(&pressure_event);
      tempRefVal=pressure_event.pressure;
    } while (tempMainVal <= 0.00 || tempRefVal <= 0.00);
    
    tempCompVal = tempMainVal - tempRefVal; 
    refVal=tempRefVal;
  } 
  else if(pressureType==PRESS_TYPE_ABS){
    //Keep reading until we have a valid pressure > 0.00
    do{
      tempMainVal = lps35hw.readPressure();
    } while (tempMainVal <= 0.00);

    tempCompVal=0.00;
    refVal=tempMainVal;
  }
  return tempCompVal;
}

void LSPressure::setCompPressure() {
  compVal = getCompPressure();
}

void LSPressure::setZeroPressure() {
  compVal = 0.00;
  for (int i = 0 ; i < PRESS_BUFF_SIZE ; i++){
    compVal += getCompPressure();
  }
  compVal = (compVal / PRESS_BUFF_SIZE);
}

void LSPressure::setStateThreshold(float s, float p){
  sipThreshold = s;
  puffThreshold = p;
}

void LSPressure::update() {
  updatePressure();
  updateState();
}


void LSPressure::updatePressure() {
  //resetTimer();

  mainVal = lps35hw.readPressure();
  
  if(pressureType==PRESS_TYPE_DIFF) {
    
    bmp_pressure->getEvent(&pressure_event); 
    float tempRefVal = pressure_event.pressure;
    //Update compensation pressure value if reference pressure is changed 
    if(abs(refVal-tempRefVal)>=refTolVal && tempRefVal > 0.00){ 
        compVal+=refVal-tempRefVal;
      }    
      if(tempRefVal > 0.00) { refVal=tempRefVal; }
   };

  
  //Make sure pressure readings are valid 
  if(mainVal > 0.00 && refVal > 0.00){
    diffVal = mainVal - refVal - compVal;

    pressureBuffer.pushElement({mainVal, refVal, diffVal});
  }
  
  //Serial.println(getTime());  
}

void LSPressure::updateState() {
  mainStateTimer.run();
  sapPrevState = sapBuffer.getLastElement();  //Get the previous state
  float pressureValue = getDiffPressure();
  //check for sip and puff conditions
  if (pressureValue > puffThreshold)  { 
    sapMainState = PRESS_SAP_MAIN_STATE_PUFF;
  } else if (pressureValue < sipThreshold)  { 
    sapMainState = PRESS_SAP_MAIN_STATE_SIP;
  } else {
    sapMainState = PRESS_SAP_MAIN_STATE_NONE;
  }

  //None:None, Sip:Sip, Puff:Puff
  //Update time
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
  if(sapPrevState.secondaryState==PRESS_SAP_SEC_STATE_WAITING && mainStateTimer.elapsedTime(sapStateTimerId)>CONF_ACTION_TIMEOUT){
      setZeroPressure();                                   //Update pressure compensation value 
      //Reset and start the timer    
      mainStateTimer.restartTimer(sapStateTimerId);   
  }
}

float LSPressure::getMainPressure() {
  return pressureBuffer.getLastElement().mainPressure;
}

float LSPressure::getRefPressure() {
  return pressureBuffer.getLastElement().refPressure;
}


float LSPressure::getDiffPressure() {
  return pressureBuffer.getLastElement().diffPressure;
}



pressureStruct LSPressure::getAllPressure() {
  return pressureBuffer.getLastElement();
}

inputStateStruct LSPressure::getState(){
  return sapBuffer.getLastElement();
}


#endif 
