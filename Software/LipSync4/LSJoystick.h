//Header definition
#ifndef _LSJOYSTICK_H
#define _LSJOYSTICK_H

#include <Tlv493d.h>  //Infinion TLV493 magnetic sensor

#define JOY_BUFF_SIZE 5

#define JOY_CALIBR_ARRAY_SIZE 5
#define JOY_RAW_ARRAY_SIZE 5

#define JOY_DIRECTION_DEFAULT 1
#define JOY_DIRECTION_INVERSE -1
#define JOY_DIRECTION_FAULT 0

#define JOY_Z_DIRECTION_THRESHOLD 9

#define JOY_INPUT_XY_MAX 1024 //1024

//#define JOY_OUTPUT_XY_MAX 10 

#define JOY_DEADZONE_STATUS true

#define JOY_DEADZONE_FACTOR 0.12


#define JOY_OUTPUT_RANGE_LEVEL 5 


typedef struct {
  float x;
  float y;
} pointFloatType;

typedef struct {
  int x;
  int y;
} pointIntType;


class LSJoystick {
  private:
    Tlv493d Tlv493dSensor = Tlv493d();
    LSCircularBuffer <pointIntType> joystickInputBuffer;
    int applyDeadzone(int input);
    pointIntType processInputReading(pointFloatType inputPoint);
    pointIntType linearizeOutput(pointIntType inputPoint);
    pointIntType processOutputResponse(pointIntType inputPoint);
    int mapFloatInt(float input, float inputStart, float inputEnd, int outputStart, int outputEnd);
    pointFloatType absPoint(pointFloatType inputPoint);
    float magnitudePoint(pointFloatType inputPoint);
    int sgn(float val);
    pointFloatType magnetInputCalibration[JOY_CALIBR_ARRAY_SIZE];
    pointFloatType inputPoint;
    int _joystickXDirection;
    int _joystickYDirection;
    int _magnetZDirection;
    int _magnetXDirection;
    int _magnetYDirection;
    bool _deadzoneEnabled;
    float _deadzoneFactor;
    int _deadzoneValue;
    int _rangeLevel;
    int _rangeValue;
    float _inputRadius;
  public:
    LSJoystick();
    void begin();
    void clear();   
    int getMagnetXDirection();
    void setMagnetXDirection(int magnetXDirection);
    int getMagnetYDirection();
    void setMagnetYDirection(int magnetYDirection);
    int getMagnetZDirection();
    void setMagnetZDirection();
    void setMagnetDirection(int magnetXDirection, int magnetYDirection);
    void setDeadzone(bool deadzoneEnabled,float deadzoneFactor);
    int getOutputRange();
    void setOutputRange(int rangeLevel);
    void setMinimumRadius();
    pointFloatType getInputComp();
    void updateInputComp();
    pointFloatType getInputMax(int quad);
    void setInputMax(int quad, pointFloatType point);
    void update();
    int getXOut();
    int getYOut();
    pointFloatType getXYIn();
    pointIntType getXYOut();

};

LSJoystick::LSJoystick() {
  joystickInputBuffer.begin(JOY_BUFF_SIZE);
}

void LSJoystick::begin() {

  _inputRadius = 0.0;

  Tlv493dSensor.begin();
  setMagnetDirection(JOY_DIRECTION_DEFAULT,JOY_DIRECTION_DEFAULT);
  setDeadzone(JOY_DEADZONE_STATUS,JOY_DEADZONE_FACTOR);
  setOutputRange(JOY_OUTPUT_RANGE_LEVEL);
  clear();
}

void LSJoystick::clear() {

  magnetInputCalibration[0] = {0.00, 0.00};
  magnetInputCalibration[1] = {0.00, 0.00};
  magnetInputCalibration[2] = {0.00, 0.00};
  magnetInputCalibration[3] = {0.00, 0.00};
  magnetInputCalibration[4] = {0.00, 0.00};


  joystickInputBuffer.pushElement({0, 0});

}

int LSJoystick::getMagnetXDirection() {
  return _magnetXDirection;
}

void LSJoystick::setMagnetXDirection(int magnetXDirection){
   _magnetXDirection = magnetXDirection;
}

int LSJoystick::getMagnetYDirection() {
  return _magnetYDirection;
}

void LSJoystick::setMagnetYDirection(int magnetYDirection){
   _magnetYDirection = magnetYDirection;
}


int LSJoystick::getMagnetZDirection() {
  return _magnetZDirection;
}

void LSJoystick::setMagnetZDirection() {

  float zReading = 0.0;
  for (int i = 0 ; i < JOY_RAW_ARRAY_SIZE ; i++){
    Tlv493dSensor.updateData();
    zReading += Tlv493dSensor.getZ();
  }
  zReading = ((float) zReading) / JOY_RAW_ARRAY_SIZE;
   
  if (zReading < -1 * JOY_Z_DIRECTION_THRESHOLD)
  {
    _magnetZDirection = JOY_DIRECTION_INVERSE;
  }
  else if (zReading > JOY_Z_DIRECTION_THRESHOLD){
    _magnetZDirection = JOY_DIRECTION_DEFAULT;
  }
  else {
    _magnetZDirection = JOY_DIRECTION_FAULT;
  }

}

void LSJoystick::setMagnetDirection(int magnetXDirection,int magnetYDirection) {

  setMagnetXDirection(magnetXDirection);
  setMagnetYDirection(magnetYDirection);
  setMagnetZDirection();
  
  _joystickXDirection = _magnetZDirection * _magnetYDirection;
  _joystickYDirection = _magnetZDirection * _magnetXDirection;
}

void LSJoystick::setDeadzone(bool deadzoneEnabled,float deadzoneFactor){
  _deadzoneEnabled = deadzoneEnabled;
  
  _deadzoneFactor = deadzoneFactor;
  (_deadzoneEnabled) ? _deadzoneValue = round(JOY_INPUT_XY_MAX*_deadzoneFactor):_deadzoneValue=0;   
}

int LSJoystick::getOutputRange(){
  return _rangeLevel;
}


void LSJoystick::setOutputRange(int rangeLevel){
  _rangeValue = (int)((0.125 * sq(rangeLevel)) + ( 0.3 * rangeLevel ) + 2);       //Polynomial 
  //_rangeValue = (int)((1.05 * exp(( 0.175 * rangeLevel) + 1.1)) - 1);                   //Exponential   
   Serial.print("_rangeValue:");
   Serial.println(_rangeValue);
  _rangeLevel = rangeLevel;
}

void LSJoystick::setMinimumRadius(){
  float tempRadius = 0.0;
  for (int i = 1; i < JOY_CALIBR_ARRAY_SIZE; i++) {
    tempRadius = (sqrt(sq(magnetInputCalibration[i].x) + sq(magnetInputCalibration[i].y))/sqrt(2.0));
    if(_inputRadius==0.0 || tempRadius<_inputRadius) { _inputRadius = tempRadius; }
  }
}



pointFloatType LSJoystick::getInputComp() {
  return magnetInputCalibration[0];
}

void LSJoystick::updateInputComp() {
  Tlv493dSensor.updateData();
  magnetInputCalibration[0] = {Tlv493dSensor.getY()*_joystickXDirection, Tlv493dSensor.getX()*_joystickYDirection};
  setMinimumRadius();
}



pointFloatType LSJoystick::getInputMax(int quad) {
  Tlv493dSensor.updateData();
  //Apply compensation point
  pointFloatType tempCalibrationPoint = {Tlv493dSensor.getY()*_joystickXDirection - magnetInputCalibration[0].x, 
                                        Tlv493dSensor.getX()*_joystickYDirection - magnetInputCalibration[0].y};
//  Serial.print("x:");
//  Serial.print(tempCalibrationPoint.x);
//  Serial.print("y:");
//  Serial.print(tempCalibrationPoint.y);
//  Serial.print(",magnitude:");
//  Serial.println(magnitudePoint(tempCalibrationPoint));
  if((quad >= 0) && 
  (quad < JOY_CALIBR_ARRAY_SIZE) && 
  magnitudePoint(tempCalibrationPoint)>magnitudePoint(magnetInputCalibration[quad])){           //The point with larger magnitude is sent as output 
    magnetInputCalibration[quad] = tempCalibrationPoint;
    setMinimumRadius();
  }
  
  return magnetInputCalibration[quad];
}

void LSJoystick::setInputMax(int quad,pointFloatType inputPoint) {

  magnetInputCalibration[quad] = inputPoint;
  setMinimumRadius();
}



void LSJoystick::update() {

  Tlv493dSensor.updateData();
  inputPoint = {Tlv493dSensor.getY(), Tlv493dSensor.getX()};
  pointFloatType centeredPoint = {(inputPoint.x)*_joystickXDirection, (inputPoint.y)*_joystickYDirection};
  pointIntType outputPoint = processInputReading(centeredPoint);
  //pointIntType outputPoint = processInputReading({Tlv493dSensor.getY()*_magnetDirection, Tlv493dSensor.getX()*_magnetDirection});
  outputPoint = processOutputResponse(outputPoint);
  joystickInputBuffer.pushElement(outputPoint);
//  
//  Serial.print(Tlv493dSensor.getY());  
//  Serial.print(",");  
//  Serial.println(Tlv493dSensor.getX()); 
  
}


int LSJoystick::getXOut() {
  return joystickInputBuffer.getLastElement().x;
}

int LSJoystick::getYOut() {
  return joystickInputBuffer.getLastElement().y;
}


pointIntType LSJoystick::getXYOut() {
  return joystickInputBuffer.getLastElement();
}

pointFloatType LSJoystick::getXYIn() {
  return inputPoint;
}



//Private

int LSJoystick::applyDeadzone(int input){

  int output = 0;
  //Deadzone
  if(_deadzoneEnabled) { 
    if(abs(input)<_deadzoneValue){
      output=0;
    }
    else if(abs(input)>JOY_INPUT_XY_MAX-_deadzoneValue){
      output=sgn(input) * JOY_INPUT_XY_MAX;
    } else{
      output=input;
    }
  }
  return output;
}

pointIntType LSJoystick::linearizeOutput(pointIntType inputPoint){
  pointIntType outputPoint = {0,0};
  outputPoint.x = map(inputPoint.x,-1*JOY_INPUT_XY_MAX,JOY_INPUT_XY_MAX,-1*_rangeValue,_rangeValue);
  outputPoint.y = map(inputPoint.y,-1*JOY_INPUT_XY_MAX,JOY_INPUT_XY_MAX,-1*_rangeValue,_rangeValue);
  return outputPoint;  
}

pointIntType LSJoystick::processInputReading(pointFloatType inputPoint) {
  pointFloatType centeredPoint = {inputPoint.x - magnetInputCalibration[0].x, inputPoint.y - magnetInputCalibration[0].y};
  pointFloatType limitPoint = {0.00, 0.00};
  pointIntType outputPoint = {0,0};
  
  float thetaVal = atan2(centeredPoint.y, centeredPoint.x);         // Get the angel of the point

  //Find the limiting point on perimeter of circle
  limitPoint.x = sgn(centeredPoint.x) * abs(cos(thetaVal)*_inputRadius);
  limitPoint.y = sgn(centeredPoint.y) * abs(sin(thetaVal)*_inputRadius);

  //Compare the magnitude of two points from center
  //Output point on perimeter of circle if it's outside
  if ((sq(centeredPoint.y) + sq(centeredPoint.x)) >= sq(_inputRadius)) {
    centeredPoint.x = limitPoint.x; 
    centeredPoint.y = limitPoint.y; 
//    Serial.print(centeredPoint.x);  
//    Serial.print(",");  
//    Serial.println(centeredPoint.y); 
  }
  
    outputPoint.x = mapFloatInt(centeredPoint.x, -1*_inputRadius, _inputRadius, -1*JOY_INPUT_XY_MAX, JOY_INPUT_XY_MAX);
    outputPoint.y = mapFloatInt(centeredPoint.y, -1*_inputRadius, _inputRadius, -1*JOY_INPUT_XY_MAX, JOY_INPUT_XY_MAX);
 
//    Serial.print(outputPoint.x);  
//    Serial.print(",");  
//    Serial.println(outputPoint.y); 
 
  return outputPoint;
}



pointIntType LSJoystick::processOutputResponse(pointIntType inputPoint){
  pointIntType outputPoint,deadzonedPoint = {0,0};
  //Deadzone
  deadzonedPoint = {applyDeadzone(inputPoint.x),applyDeadzone(inputPoint.y)};
  //Linearize
  outputPoint = linearizeOutput(deadzonedPoint);
  return outputPoint;
}

int LSJoystick::mapFloatInt(float input, float inputStart, float inputEnd, int outputStart, int outputEnd) {

  float inputRange = inputEnd - inputStart;
  int outputRange = outputEnd - outputStart;

  int output = (int)((input - inputStart) * outputRange / inputRange + outputStart);
  
  return output;
}

pointFloatType LSJoystick::absPoint(pointFloatType inputPoint){
  return {abs(inputPoint.x), abs(inputPoint.y)};
}

float LSJoystick::magnitudePoint(pointFloatType inputPoint){
  return (sqrt(sq(inputPoint.x) + sq(inputPoint.y)));
}



int LSJoystick::sgn(float val) {
  return (0.0 < val) - (val < 0.0);
}


#endif 
