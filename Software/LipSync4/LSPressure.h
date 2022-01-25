#include <Adafruit_LPS35HW.h>
#include <Adafruit_BMP280.h>   //onboard pressure sensor on Adafruit Sense Micro 

#define PRESS_BUFF_SIZE 5
#define PRESS_REF_TOLERANCE 0.1

#define PRESS_FILTER_NONE 0
#define PRESS_FILTER_AVERAGE 1

#define PRESS_TYPE_ABS 0
#define PRESS_TYPE_DIFF 1

Adafruit_LPS35HW lps35hw = Adafruit_LPS35HW();

Adafruit_BMP280 bmp; // use I2C interface

Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

typedef struct {
  float mainPressure;
  float refPressure;
  float diffPressure;
} pressureStruct;

LSCircularBuffer <pressureStruct> pressureBuffer(PRESS_BUFF_SIZE);

class LSPressure {
  private: 
    int filterMode;
    int pressureType;
    float mainVal;
    float refVal;
    float compVal;
    float diffVal;
    sensors_event_t pressure_event;
    float refTolVal;
  public:
    LSPressure();
    void begin(int type);                                    
    void clear();  
    void setFilterMode(int mode); 
    void setRefTolerance(float value); 
    float getCompPressure();
    void setCompPressure();
    void setZeroPressure();
    void update();    
    float getMainPressure();
    float getRefPressure();
    float getDiffPressure();
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

  filterMode = PRESS_FILTER_NONE;

  refTolVal = PRESS_REF_TOLERANCE;

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


void LSPressure::update() {
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
