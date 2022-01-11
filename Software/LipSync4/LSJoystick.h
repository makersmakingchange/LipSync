#include <Tlv493d.h>  //Infinion TLV493 magnetic sensor

#define CALIBRATION_ARRAY_SIZE 5
#define RAW_ARRAY_SIZE 5

#define RAW_X_COMP 0.0
#define RAW_Y_COMP 0.0

#define RAW_X1_MAX 40.0
#define RAW_Y1_MAX 40.0

#define RAW_X2_MAX -40.0
#define RAW_Y2_MAX 40.0

#define RAW_X3_MAX -40.0
#define RAW_Y3_MAX -40.0

#define RAW_X4_MAX 40.0
#define RAW_Y4_MAX -40.0

#define MAG_DIRECTION_DEFAULT 1
#define MAG_DIRECTION_INVERSE -1

#define INPUT_MAX 20

Tlv493d Tlv493dSensor = Tlv493d();

typedef struct {
  float x;
  float y;
} magnetInputType;

typedef struct {
  int x;
  int y;
} joystickInputType;

LSCircularBuffer <joystickInputType> joystickInputBuffer(5);
class LSJoystick {
  private:
    magnetInputType magnetInputComp;
    magnetInputType magnetInputCalibration[CALIBRATION_ARRAY_SIZE];
    joystickInputType limitCircle(magnetInputType point);
    int mapFloatInt(float input, float inputStart, float inputEnd, int outputStart, int outputEnd);
    int sgn(float val);
    int _direction;
  public:
    LSJoystick();
    void begin();
    void clear();                                                  //Clear the stack
    void setMagDirection(int direction);
    void setInputComp();
    void setInputMax(int quad);
    void update();
    int getXVal();
    int getYVal();
    joystickInputType getAllVal();

};

LSJoystick::LSJoystick() {
  _direction = MAG_DIRECTION_DEFAULT;
}

void LSJoystick::begin() {
  Tlv493dSensor.begin();
  clear();
}

void LSJoystick::clear() {

  _direction = MAG_DIRECTION_DEFAULT;

  magnetInputComp.x = 0.00;
  magnetInputComp.y = 0.00;
  magnetInputCalibration[0] = {magnetInputComp.x, magnetInputComp.y};
  magnetInputCalibration[1] = {0.00, 0.00};
  magnetInputCalibration[2] = {0.00, 0.00};
  magnetInputCalibration[3] = {0.00, 0.00};
  magnetInputCalibration[4] = {0.00, 0.00};

  for (int i = 0; i < RAW_ARRAY_SIZE; i++) {
    joystickInputBuffer.pushElement({0, 0});


  }
}

void LSJoystick::setMagDirection(int direction) {
  if (direction == MAG_DIRECTION_DEFAULT || direction == MAG_DIRECTION_INVERSE) {
    _direction = direction;
  } else {
    _direction = MAG_DIRECTION_DEFAULT;
  }
}

void LSJoystick::setInputComp() {
  Tlv493dSensor.updateData();
  magnetInputComp.x = Tlv493dSensor.getY();
  magnetInputComp.y = Tlv493dSensor.getX();
  magnetInputCalibration[0] = {magnetInputComp.x, magnetInputComp.y};

}



void LSJoystick::setInputMax(int quad) {
  /*
    if((quad >= 0) && (quad < CALIBRATION_ARRAY_SIZE)){
    Tlv493dSensor.updateData();
    magnetInputCalibration[quad] = {Tlv493dSensor.getY(), Tlv493dSensor.getX()};
    }*/
  magnetInputCalibration[1] = {RAW_X1_MAX, RAW_Y1_MAX};
  magnetInputCalibration[2] = {RAW_X2_MAX, RAW_Y2_MAX};
  magnetInputCalibration[3] = {RAW_X3_MAX, RAW_Y3_MAX};
  magnetInputCalibration[4] = {RAW_X4_MAX, RAW_Y4_MAX};

}



void LSJoystick::update() {

  Tlv493dSensor.updateData();
  joystickInputType outputPoint = limitCircle({Tlv493dSensor.getY(), Tlv493dSensor.getX()});
  joystickInputBuffer.pushElement(outputPoint);

}

int LSJoystick::getXVal() {
  return joystickInputBuffer.getLastElement().x;
}

int LSJoystick::getYVal() {
  return joystickInputBuffer.getLastElement().y;
}


joystickInputType LSJoystick::getAllVal() {
  return joystickInputBuffer.getLastElement();
}

joystickInputType LSJoystick::limitCircle(magnetInputType point) {
  magnetInputType rawPoint = {point.x - magnetInputComp.x, point.y - magnetInputComp.y};
  magnetInputType maxPoint, limitPoint;
  joystickInputType limitOutputPoint;

  float thetaVal = atan2(rawPoint.y, rawPoint.x);         // Get the angel of the point

  if ( rawPoint.x > 0 && rawPoint.y >= 0) {               // Find the max point at the corner depending on the quadrant
    maxPoint = magnetInputCalibration[1];
  }
  else if ( rawPoint.x <= 0 && rawPoint.y > 0) {
    maxPoint = magnetInputCalibration[2];
  }
  else if ( rawPoint.x < 0 && rawPoint.y <= 0) {
    maxPoint = magnetInputCalibration[3];
  }
  else if ( rawPoint.x >= 0 && rawPoint.y < 0) {
    maxPoint = magnetInputCalibration[4];
  }
  else {
    maxPoint = {0.00, 0.00};
  }

  //Find the limiting point on perimeter of ellipse
  limitPoint.x = (maxPoint.x * maxPoint.y) / sqrt(sq(maxPoint.y) + sq(maxPoint.x) * sq(tan(thetaVal)));
  limitPoint.y = (maxPoint.x * maxPoint.y) / sqrt(sq(maxPoint.x) + sq(maxPoint.y) / sq(tan(thetaVal)));


  //Compare the magnitude of two points from center
  //Output point on perimeter of ellipse if it's outside
  if ((sq(rawPoint.y) + sq(rawPoint.x)) >= (sq(limitPoint.y) + sq(limitPoint.x))) {
    limitOutputPoint.x = mapFloatInt(limitPoint.x, -maxPoint.x, maxPoint.x, -INPUT_MAX, INPUT_MAX);
    limitOutputPoint.y = mapFloatInt(limitPoint.y, -maxPoint.y, maxPoint.y, -INPUT_MAX, INPUT_MAX);

  } else {
    limitOutputPoint.x = mapFloatInt(rawPoint.x, -maxPoint.x, maxPoint.x, -INPUT_MAX, INPUT_MAX);
    limitOutputPoint.y = mapFloatInt(rawPoint.y, -maxPoint.y, maxPoint.y, -INPUT_MAX, INPUT_MAX);
  }

  limitOutputPoint.x = _direction * sgn(rawPoint.x) * limitOutputPoint.x;
  limitOutputPoint.y = _direction * sgn(rawPoint.y) * limitOutputPoint.y;

  return limitOutputPoint;
}



int LSJoystick::mapFloatInt(float input, float inputStart, float inputEnd, int outputStart, int outputEnd) {

  float inputRange = inputEnd - inputStart;
  int outputRange = outputEnd - outputStart;

  int output = (input - inputStart) * outputRange / inputRange + outputStart;

  return output;
}


int LSJoystick::sgn(float val) {
  return (0.0 < val) - (val < 0.0);
}
