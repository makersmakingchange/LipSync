#include <Adafruit_LPS35HW.h>
#include <Adafruit_BMP280.h>   //onboard pressure sensor on Adafruit Sense Micro 

#define PRESS_ARRAY_SIZE 5

#define FILTER_NONE 0
#define FILTER_AVERAGE 1

#define PRESS_TYPE_ABS 0
#define PRESS_TYPE_DIFF 1

Adafruit_LPS35HW lps35hw = Adafruit_LPS35HW();

Adafruit_BMP280 bmp; // use I2C interface

Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
StopWatch pressureTimer[1];

typedef struct {
  float mainPressure;
  float refPressure;
  float rawPressure;
} pressureStruct;

LSQueue <pressureStruct> pressureQueue;

class LSPressure {
  private: 
    float calculateAverage(pressureStruct * array, int len);  
    void resetTimer();
    unsigned long getTime();
    int pressureType;
    float pressureComp;
    float pressureZero;
  public:
    LSPressure();
    pressureStruct pressureArray[PRESS_ARRAY_SIZE];
    int filterMode;
    void begin(int type);                                    
    void clear();  
    void zero();
    void update();    
    void setFilterMode(int mode); 
    float getMainPressure();
    float getRefPressure();
    float getRawPressure();
    float getFltPressure();
    pressureStruct getAllPressure();
};

LSPressure::LSPressure() {

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

  filterMode = FILTER_NONE;

  lps35hw.setDataRate(LPS35HW_RATE_75_HZ);  // 1,10,25,50,75

  if(pressureType==PRESS_TYPE_DIFF){
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode. 
                  Adafruit_BMP280::SAMPLING_NONE,     // Temp. oversampling
                  Adafruit_BMP280::SAMPLING_X4,    // Pressure oversampling 
                  Adafruit_BMP280::FILTER_X16,      // Filtering. 
                  Adafruit_BMP280::STANDBY_MS_1000); // Standby time.       
  } else {
    zero();
  }

  clear();
}

void LSPressure::clear() {
      
  sensors_event_t pressure_event;
  bmp_pressure->getEvent(&pressure_event);

  float mainVal = lps35hw.readPressure();
  float refVal;
  if(pressureType==PRESS_TYPE_DIFF){
    refVal=pressure_event.pressure;
  }
  else{
    refVal = pressureZero;
  }
  //pressureComp = mainVal - refVal;
  float rawVal = 0.0;  //mainVal - refVal;
  
  for (int i=0; i < PRESS_ARRAY_SIZE; i++) {
    //pressureArray[i] = {mainVal, refVal, rawVal};
    pressureQueue.push({mainVal, refVal, rawVal});
    
  }
  
}

void LSPressure::zero() {
  //lps35hw.zeroPressure();
  pressureZero = lps35hw.readPressure();
}

void LSPressure::update() {
  //resetTimer();
  sensors_event_t pressure_event;
  bmp_pressure->getEvent(&pressure_event);
  
  float mainVal = lps35hw.readPressure();
  float refVal;
  if(pressureType==PRESS_TYPE_DIFF){
    refVal=pressure_event.pressure;
  }
  else{
    refVal= pressureZero;
  }

  if(mainVal > 0.00 && refVal > 0.00){
    float rawVal = mainVal - refVal; // - pressureComp;
    /*
    memmove( pressureArray, &pressureArray[1], (PRESS_ARRAY_SIZE-1) * sizeof(pressureArray[0]));
    pressureArray[PRESS_ARRAY_SIZE-1] = {mainVal, refVal, rawVal};
    */
    pressureQueue.push({mainVal, refVal, rawVal});
    pressureQueue.pop();    
  }
  
  //Serial.println(getTime());  
}

void LSPressure::setFilterMode(int mode) {
  filterMode = mode;
}

float LSPressure::getMainPressure() {
  //return pressureArray[0].mainPressure;
  return pressureQueue.end().mainPressure;
}

float LSPressure::getRefPressure() {
  //return pressureArray[0].refPressure;
  return pressureQueue.end().refPressure;
}


float LSPressure::getRawPressure() {
  //return pressureArray[0].rawPressure;
  return pressureQueue.end().rawPressure;
}


float LSPressure::getFltPressure() {
  if(filterMode == FILTER_NONE){
    return pressureArray[0].rawPressure;
  } else if (filterMode == FILTER_AVERAGE) {
    return (calculateAverage(pressureArray,PRESS_ARRAY_SIZE));
  } else {
    return pressureArray[0].rawPressure;
  }
}

pressureStruct LSPressure::getAllPressure() {
  //return pressureArray[0];
  return pressureQueue.front();
}


float LSPressure::calculateAverage(pressureStruct * array, int len)  
{
  float sum = 0.0;
  for (int i = 0 ; i < len ; i++){
    sum += array [i].rawPressure;
  }
  return(((float) sum) / len);
}


//***RESET TIMER FUNCTION***//

void LSPressure::resetTimer() {
  pressureTimer[0].stop();                                //Reset and start the timer         
  pressureTimer[0].reset();                                                                        
  pressureTimer[0].start(); 
}

//***GET TIME FUNCTION***//

unsigned long LSPressure::getTime() {
  unsigned long finalTime = pressureTimer[0].elapsed(); 
  pressureTimer[0].stop();                                //Reset and start the timer         
  pressureTimer[0].reset(); 
  return finalTime;
}
