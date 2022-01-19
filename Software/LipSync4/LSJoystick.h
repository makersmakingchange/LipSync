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

#define JOY_INPUT_XY_MAX 12 //1024

#define JOY_DEADZONE_FACTOR 0.05


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
    pointIntType processInputReading(pointFloatType point);
    pointIntType processOutputResponse(pointIntType point);
    int mapFloatInt(float input, float inputStart, float inputEnd, int outputStart, int outputEnd);
    pointFloatType absPoint(pointFloatType point);
    int sgn(float val);
    pointFloatType magnetInputComp;
    pointFloatType magnetInputCalibration[JOY_CALIBR_ARRAY_SIZE];
    int _magnetDirection;
    bool _deadzoneEnabled;
    float _deadzoneFactor;
    int _scaleLevel;
  public:
    LSJoystick();
    void begin();
    void clear();                                                  //Clear the stack
    void setMagnetDirection(int magnetDirection);
    void setDeadzone(bool deadzoneEnabled,float deadzoneFactor);
    void setOutputScale(int scaleLevel);
    void setInputComp();
    pointFloatType getInputMax(int quad);
    void setInputMax(int quad, pointFloatType point);
    void update();
    int getXVal();
    int getYVal();
    pointIntType getAllVal();

};

LSJoystick::LSJoystick() {
  //_magnetDirection = JOY_MAG_DIRECTION_DEFAULT;
}

void LSJoystick::begin() {
  setMagnetDirection(JOY_MAG_DIRECTION_DEFAULT);
  setDeadzone(true,JOY_DEADZONE_FACTOR);
  setOutputScale(JOY_OUTPUT_SCALE);
  Tlv493dSensor.begin();
  clear();
}

void LSJoystick::clear() {

  _magnetDirection = JOY_MAG_DIRECTION_DEFAULT;

  magnetInputComp.x = 0.00;
  magnetInputComp.y = 0.00;
  magnetInputCalibration[0] = {magnetInputComp.x, magnetInputComp.y};
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

void LSJoystick::setMagnetDirection(int magnetDirection) {
  if (magnetDirection == JOY_MAG_DIRECTION_DEFAULT || magnetDirection == JOY_MAG_DIRECTION_INVERSE) {
    _magnetDirection = magnetDirection;
  } else {
    _magnetDirection = JOY_MAG_DIRECTION_DEFAULT;
  }
}

void LSJoystick::setDeadzone(bool deadzoneEnabled,float deadzoneFactor){
  _deadzoneEnabled = deadzoneEnabled;
  _deadzoneFactor = deadzoneFactor;
}

void LSJoystick::setOutputScale(int scaleLevel){
  _scaleLevel=scaleLevel;
}

void LSJoystick::setInputComp() {
  Tlv493dSensor.updateData();
  magnetInputComp.x = Tlv493dSensor.getY()*_magnetDirection;
  magnetInputComp.y = Tlv493dSensor.getX()*_magnetDirection;
  magnetInputCalibration[0] = {magnetInputComp.x, magnetInputComp.y};
    Serial.print(magnetInputComp.x);  
    Serial.print(",");  
    Serial.println(magnetInputComp.y); 
}



pointFloatType LSJoystick::getInputMax(int quad) {
  
  if((quad >= 0) && (quad < JOY_CALIBR_ARRAY_SIZE)){
    Tlv493dSensor.updateData();
    magnetInputCalibration[quad] = {Tlv493dSensor.getY()*_magnetDirection, Tlv493dSensor.getX()*_magnetDirection};
  }
  return magnetInputCalibration[quad];
}

void LSJoystick::setInputMax(int quad,pointFloatType point) {
  /*
  magnetInputCalibration[1] = {JOY_RAW_X1_MAX, JOY_RAW_Y1_MAX};
  magnetInputCalibration[2] = {JOY_RAW_X2_MAX, JOY_RAW_Y2_MAX};
  magnetInputCalibration[3] = {JOY_RAW_X3_MAX, JOY_RAW_Y3_MAX};
  magnetInputCalibration[4] = {JOY_RAW_X4_MAX, JOY_RAW_Y4_MAX};
  */
  magnetInputCalibration[quad] = point;
}



void LSJoystick::update() {

  Tlv493dSensor.updateData();
  pointIntType outputPoint = processInputReading({Tlv493dSensor.getY()*_magnetDirection, Tlv493dSensor.getX()*_magnetDirection});
  outputPoint = processOutputResponse(outputPoint);
  joystickInputBuffer.pushElement(outputPoint);

}


int LSJoystick::getXVal() {
  return joystickInputBuffer.getLastElement().x;
}

int LSJoystick::getYVal() {
  return joystickInputBuffer.getLastElement().y;
}


pointIntType LSJoystick::getAllVal() {
  return joystickInputBuffer.getLastElement();
}

//Private

pointIntType LSJoystick::processInputReading(pointFloatType point) {
  pointFloatType rawPoint = {point.x - magnetInputComp.x, point.y - magnetInputComp.y};
  pointFloatType maxPoint, limitPoint = {0.00, 0.00};
  pointIntType outputPoint;
  bool canSkip=false;
/*
      Serial.print(point.x);  
    Serial.print(",");  
    Serial.println(point.y); 
*/


  float thetaVal = atan2(rawPoint.y, rawPoint.x);         // Get the angel of the point

  if ( rawPoint.x > 0 && rawPoint.y >= 0) {               // Find the max point at the corner depending on the quadrant
    maxPoint = absPoint(magnetInputCalibration[1]);
    //Serial.println("1"); 
  }
  else if ( rawPoint.x <= 0 && rawPoint.y > 0) {
    maxPoint = absPoint(magnetInputCalibration[2]);
    //Serial.println("2"); 
  }
  else if ( rawPoint.x < 0 && rawPoint.y <= 0) {
    maxPoint = absPoint(magnetInputCalibration[3]);
    //Serial.println("3"); 
  }
  else if ( rawPoint.x >= 0 && rawPoint.y < 0) {
    maxPoint = absPoint(magnetInputCalibration[4]);
    //Serial.println("4"); 
  }
  else {
    maxPoint = {0.00, 0.00};
    //Serial.println("0"); 
    canSkip=true;
  }

  //Find the limiting point on perimeter of ellipse
  if(!canSkip){
    limitPoint.x = abs(maxPoint.x * maxPoint.y) / sqrt(sq(maxPoint.y) + sq(maxPoint.x) * sq(tan(thetaVal)));
    limitPoint.y = abs(maxPoint.x * maxPoint.y) / sqrt(sq(maxPoint.x) + sq(maxPoint.y) / sq(tan(thetaVal)));
  }

  //Compare the magnitude of two points from center
  //Output point on perimeter of ellipse if it's outside
  if ((sq(rawPoint.y) + sq(rawPoint.x)) >= (sq(limitPoint.y) + sq(limitPoint.x))) {
    outputPoint.x = sgn(rawPoint.x) * limitPoint.x;
    outputPoint.y = sgn(rawPoint.y) * limitPoint.y;

  } else {
    outputPoint.x = mapFloatInt(rawPoint.x, -maxPoint.x, maxPoint.x, -JOY_INPUT_XY_MAX, JOY_INPUT_XY_MAX);
    outputPoint.y = mapFloatInt(rawPoint.y, -maxPoint.y, maxPoint.y, -JOY_INPUT_XY_MAX, JOY_INPUT_XY_MAX);
  }

  outputPoint.x = outputPoint.x;
  outputPoint.y = outputPoint.y;
 
  return outputPoint;
}

pointIntType LSJoystick::processOutputResponse(pointIntType point){
  return point;
}


int LSJoystick::mapFloatInt(float input, float inputStart, float inputEnd, int outputStart, int outputEnd) {

  float inputRange = inputEnd - inputStart;
  int outputRange = outputEnd - outputStart;

  int output = (input - inputStart) * outputRange / inputRange + outputStart;

  return output;
}

pointFloatType LSJoystick::absPoint(pointFloatType point){
  return {abs(point.x), abs(point.y)};
}


int LSJoystick::sgn(float val) {
  return (0.0 < val) - (val < 0.0);
}
