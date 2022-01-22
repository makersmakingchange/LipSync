#include <Tlv493d.h>  //Infinion TLV493 magnetic sensor

#define JOY_CALIBR_ARRAY_SIZE 5
#define JOY_RAW_ARRAY_SIZE 5

#define JOY_RAW_X_COMP 0.0
#define JOY_RAW_Y_COMP 0.0

#define JOY_RAW_X1_MAX 40.0
#define JOY_RAW_Y1_MAX 40.0

#define JOY_RAW_X2_MAX -40.0
#define JOY_RAW_Y2_MAX 40.0

#define JOY_RAW_X3_MAX -40.0
#define JOY_RAW_Y3_MAX -40.0

#define JOY_RAW_X4_MAX 40.0
#define JOY_RAW_Y4_MAX -40.0

#define JOY_MAG_DIRECTION_DEFAULT 1
#define JOY_MAG_DIRECTION_INVERSE -1
#define JOY_MAG_DIRECTION_FAULT 0

#define JOY_MAG_DIRECTION_THRESHOLD 9

#define JOY_INPUT_XY_MAX 1024 //1024

#define JOY_OUTPUT_XY_MAX 12 

#define JOY_DEADZONE_STATUS true

#define JOY_DEADZONE_FACTOR 0.12


#define JOY_OUTPUT_SCALE 5 

Tlv493d Tlv493dSensor = Tlv493d();


typedef struct {
  float x;
  float y;
} pointFloatType;

typedef struct {
  int x;
  int y;
} pointIntType;

LSCircularBuffer <pointIntType> joystickInputBuffer(5);
class LSJoystick {
  private:
    int applyDeadzone(int input);
    pointIntType linearizeOutput(pointIntType inputPoint);
    pointIntType processInputReading(pointFloatType inputPoint);
    pointIntType processOutputResponse(pointIntType inputPoint);
    int mapFloatInt(float input, float inputStart, float inputEnd, int outputStart, int outputEnd);
    pointFloatType absPoint(pointFloatType inputPoint);
    int sgn(float val);
    //pointFloatType magnetInputComp;
    pointFloatType magnetInputCalibration[JOY_CALIBR_ARRAY_SIZE];
    int _magnetDirection;
    bool _deadzoneEnabled;
    float _deadzoneFactor;
    int _deadzoneValue;
    int _scaleLevel;
    float _inputRadius;
  public:
    LSJoystick();
    void begin();
    void clear();   
    int getMagnetDirection();
    void setMagnetDirection();
    void setDeadzone(bool deadzoneEnabled,float deadzoneFactor);
    void setOutputScale(int scaleLevel);
    void setMinimumRadius();
    pointFloatType readInputComp();
    void getInputComp();
    pointFloatType getInputMax(int quad);
    void setInputMax(int quad, pointFloatType point);
    void update();
    int getXVal();
    int getYVal();
    pointIntType getXYVal();

};

LSJoystick::LSJoystick() {
}

void LSJoystick::begin() {

  _inputRadius = 0.0;
  Tlv493dSensor.begin();
  setMagnetDirection();
  setDeadzone(JOY_DEADZONE_STATUS,JOY_DEADZONE_FACTOR);
  setOutputScale(JOY_OUTPUT_SCALE);
  clear();
}

void LSJoystick::clear() {

  //magnetInputComp.x = 0.00;
  //magnetInputComp.y = 0.00;
  magnetInputCalibration[0] = {0.00, 0.00};
  magnetInputCalibration[1] = {0.00, 0.00};
  magnetInputCalibration[2] = {0.00, 0.00};
  magnetInputCalibration[3] = {0.00, 0.00};
  magnetInputCalibration[4] = {0.00, 0.00};


  joystickInputBuffer.pushElement({0, 0});

  /*
  for (int i = 0; i < JOY_RAW_ARRAY_SIZE; i++) {
    joystickInputBuffer.pushElement({0, 0});

  }
  */
}


int LSJoystick::getMagnetDirection() {
  return _magnetDirection;
}

void LSJoystick::setMagnetDirection() {

  float zReading = 0.0;
  for (int i = 0 ; i < JOY_RAW_ARRAY_SIZE ; i++){
    Tlv493dSensor.updateData();
    zReading += Tlv493dSensor.getZ();
  }
  zReading = ((float) zReading) / JOY_RAW_ARRAY_SIZE;
   
  if (zReading < -1 * JOY_MAG_DIRECTION_THRESHOLD)
  {
    _magnetDirection = JOY_MAG_DIRECTION_DEFAULT;
  }
  else if (zReading > JOY_MAG_DIRECTION_THRESHOLD){
    _magnetDirection = JOY_MAG_DIRECTION_INVERSE;
  }
  else {
    _magnetDirection = JOY_MAG_DIRECTION_FAULT;
  }

}

void LSJoystick::setDeadzone(bool deadzoneEnabled,float deadzoneFactor){
  _deadzoneEnabled = deadzoneEnabled;
  
  _deadzoneFactor = deadzoneFactor;
  (_deadzoneEnabled) ? _deadzoneValue = round(JOY_INPUT_XY_MAX*_deadzoneFactor):_deadzoneValue=0;   
}

void LSJoystick::setOutputScale(int scaleLevel){
  _scaleLevel=scaleLevel;
}

void LSJoystick::setMinimumRadius(){
  float tempRadius = 0.0;
  for (int i = 1; i < JOY_CALIBR_ARRAY_SIZE; i++) {
    tempRadius = (sqrt(sq(magnetInputCalibration[i].x) + sq(magnetInputCalibration[i].y))/sqrt(2.0));
    if(_inputRadius==0.0 || tempRadius<_inputRadius) { _inputRadius = tempRadius; }
  }
}



pointFloatType LSJoystick::readInputComp() {
  return magnetInputCalibration[0];
}

void LSJoystick::getInputComp() {
  Tlv493dSensor.updateData();
  //magnetInputComp.x = Tlv493dSensor.getY()*_magnetDirection;
  //magnetInputComp.y = Tlv493dSensor.getX()*_magnetDirection;
  magnetInputCalibration[0] = {Tlv493dSensor.getY()*_magnetDirection, Tlv493dSensor.getX()*_magnetDirection};
  setMinimumRadius();
}



pointFloatType LSJoystick::getInputMax(int quad) {
  
  if((quad >= 0) && (quad < JOY_CALIBR_ARRAY_SIZE)){
    Tlv493dSensor.updateData();
    magnetInputCalibration[quad] = {Tlv493dSensor.getY()*_magnetDirection, Tlv493dSensor.getX()*_magnetDirection};
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
  pointFloatType inputPoint = {Tlv493dSensor.getY()*_magnetDirection, Tlv493dSensor.getX()*_magnetDirection};
  pointIntType outputPoint = processInputReading(inputPoint);
  //pointIntType outputPoint = processInputReading({Tlv493dSensor.getY()*_magnetDirection, Tlv493dSensor.getX()*_magnetDirection});
  outputPoint = processOutputResponse(outputPoint);
  joystickInputBuffer.pushElement(outputPoint);
  
//  Serial.print(outputPoint.x);  
//  Serial.print(",");  
//  Serial.println(outputPoint.y); 
  
}


int LSJoystick::getXVal() {
  return joystickInputBuffer.getLastElement().x;
}

int LSJoystick::getYVal() {
  return joystickInputBuffer.getLastElement().y;
}


pointIntType LSJoystick::getXYVal() {
  return joystickInputBuffer.getLastElement();
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
  outputPoint.x = map(inputPoint.x,-1*JOY_INPUT_XY_MAX,JOY_INPUT_XY_MAX,-1*JOY_OUTPUT_XY_MAX,JOY_OUTPUT_XY_MAX);
  outputPoint.y = map(inputPoint.y,-1*JOY_INPUT_XY_MAX,JOY_INPUT_XY_MAX,-1*JOY_OUTPUT_XY_MAX,JOY_OUTPUT_XY_MAX);
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


int LSJoystick::sgn(float val) {
  return (0.0 < val) - (val < 0.0);
}
