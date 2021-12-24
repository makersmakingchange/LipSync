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
} joystickRawStruct;

typedef struct {
  int x;
  int y;
} joystickInputStruct;

LSQueue <joystickInputStruct> joystickInputQueue(5);
class LSJoystick {
  private: 
    joystickRawStruct joystickRawComp;    
    joystickRawStruct joystickRawCalibration[CALIBRATION_ARRAY_SIZE]; 
    joystickInputStruct limitCircle(joystickRawStruct point);      
    int mapFloatInt(float input,float inputStart,float inputEnd, int outputStart, int outputEnd);
    int sgn(float val);
    int _direction;
  public:
    LSJoystick();   
    void begin(); 
    void clear();                                                  //Clear the stack
    void setMagDirection(int direction);  
    void setRawComp();   
    void setRawMax(int quad);                                     
    void update();    
    int getXVal();
    int getYVal();
    joystickInputStruct getAllVal();

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
  
  joystickRawComp.x = 0.00;
  joystickRawComp.y = 0.00;
  joystickRawCalibration[0] = {joystickRawComp.x, joystickRawComp.y};
  joystickRawCalibration[1] = {0.00, 0.00};
  joystickRawCalibration[2] = {0.00, 0.00};
  joystickRawCalibration[3] = {0.00, 0.00};
  joystickRawCalibration[4] = {0.00, 0.00};

  for (int i=0; i < RAW_ARRAY_SIZE; i++) {
      joystickInputQueue.push({0, 0});
      
    
  }
}

void LSJoystick::setMagDirection(int direction){
  if(direction == MAG_DIRECTION_DEFAULT || direction == MAG_DIRECTION_INVERSE){
    _direction = direction;
  } else {
    _direction = MAG_DIRECTION_DEFAULT;
  }
}

void LSJoystick::setRawComp(){
  Tlv493dSensor.updateData();
  joystickRawComp.x = Tlv493dSensor.getY();
  joystickRawComp.y = Tlv493dSensor.getX();
  joystickRawCalibration[0] = {joystickRawComp.x, joystickRawComp.y};
  
}

void LSJoystick::setRawMax(int quad){
  /*
  if((quad >= 0) && (quad < CALIBRATION_ARRAY_SIZE)){
    Tlv493dSensor.updateData();    
    joystickRawCalibration[quad] = {Tlv493dSensor.getY(), Tlv493dSensor.getX()};  
  }*/
  joystickRawCalibration[1] = {RAW_X1_MAX, RAW_Y1_MAX};
  joystickRawCalibration[2] = {RAW_X2_MAX, RAW_Y2_MAX};
  joystickRawCalibration[3] = {RAW_X3_MAX, RAW_Y3_MAX};
  joystickRawCalibration[4] = {RAW_X4_MAX, RAW_Y4_MAX};

}



void LSJoystick::update() {
  
  Tlv493dSensor.updateData();
  joystickInputStruct outputPoint = limitCircle({Tlv493dSensor.getY(),Tlv493dSensor.getX()});

  /*
  joystickRawStruct rawPoint = {Tlv493dSensor.getY()-joystickRawComp.x,Tlv493dSensor.getX()-joystickRawComp.y};
  joystickRawStruct maxPoint, limitPoint;
  joystickInputStruct limitOutputPoint;
  
  float thetaVal = atan2(rawPoint.y, rawPoint.x);         // Get the angel of the point
  
  if( rawPoint.x > 0 && rawPoint.y >= 0){                 // Find the max point at the corner depending on the quadrant 
    maxPoint = joystickRawCalibration[1];
  }
  else if( rawPoint.x <= 0 && rawPoint.y > 0){
    maxPoint = joystickRawCalibration[2];
  }
  else if( rawPoint.x < 0 && rawPoint.y <= 0){
    maxPoint = joystickRawCalibration[3];
  }
  else if( rawPoint.x >= 0 && rawPoint.y < 0){
    maxPoint = joystickRawCalibration[4];
  }
  else {
    maxPoint = {0.00,0.00};
  }

  //Find the limiting point on perimeter of ellipse
  limitPoint.x = (maxPoint.x * maxPoint.y)/sqrt(sq(maxPoint.y)+sq(maxPoint.x)*sq(tan(thetaVal)));
  limitPoint.y = (maxPoint.x * maxPoint.y)/sqrt(sq(maxPoint.x)+sq(maxPoint.y)/sq(tan(thetaVal)));


  //Compare the magnitude of two points from center 
  //Output point on perimeter of ellipse if it's outside 
  if((sq(rawPoint.y)+sq(rawPoint.x))>=(sq(limitPoint.y)+sq(limitPoint.x))){
    limitOutputPoint.x = mapFloatInt(limitPoint.x,-maxPoint.x,maxPoint.x,-INPUT_MAX,INPUT_MAX);
    limitOutputPoint.y = mapFloatInt(limitPoint.y,-maxPoint.y,maxPoint.y,-INPUT_MAX,INPUT_MAX);
    
  }else {
    limitOutputPoint.x = mapFloatInt(rawPoint.x,-maxPoint.x,maxPoint.x,-INPUT_MAX,INPUT_MAX);
    limitOutputPoint.y = mapFloatInt(rawPoint.y,-maxPoint.y,maxPoint.y,-INPUT_MAX,INPUT_MAX);    
  }

    limitOutputPoint.x = _direction*sgn(rawPoint.x)*limitOutputPoint.x;
    limitOutputPoint.y = _direction*sgn(rawPoint.y)*limitOutputPoint.y;    
    */
    
    joystickInputQueue.push(outputPoint);

}

int LSJoystick::getXVal() {
  return joystickInputQueue.front().x;
}

int LSJoystick::getYVal() {
  return joystickInputQueue.front().y;
}


joystickInputStruct LSJoystick::getAllVal() {
  return joystickInputQueue.front();
}

joystickInputStruct LSJoystick::limitCircle(joystickRawStruct point){
  joystickRawStruct rawPoint = {point.x-joystickRawComp.x,point.y-joystickRawComp.y};
  joystickRawStruct maxPoint, limitPoint;
  joystickInputStruct limitOutputPoint;
  
  float thetaVal = atan2(rawPoint.y, rawPoint.x);         // Get the angel of the point
  
  if( rawPoint.x > 0 && rawPoint.y >= 0){                 // Find the max point at the corner depending on the quadrant 
    maxPoint = joystickRawCalibration[1];
  }
  else if( rawPoint.x <= 0 && rawPoint.y > 0){
    maxPoint = joystickRawCalibration[2];
  }
  else if( rawPoint.x < 0 && rawPoint.y <= 0){
    maxPoint = joystickRawCalibration[3];
  }
  else if( rawPoint.x >= 0 && rawPoint.y < 0){
    maxPoint = joystickRawCalibration[4];
  }
  else {
    maxPoint = {0.00,0.00};
  }

  //Find the limiting point on perimeter of ellipse
  limitPoint.x = (maxPoint.x * maxPoint.y)/sqrt(sq(maxPoint.y)+sq(maxPoint.x)*sq(tan(thetaVal)));
  limitPoint.y = (maxPoint.x * maxPoint.y)/sqrt(sq(maxPoint.x)+sq(maxPoint.y)/sq(tan(thetaVal)));


  //Compare the magnitude of two points from center 
  //Output point on perimeter of ellipse if it's outside 
  if((sq(rawPoint.y)+sq(rawPoint.x))>=(sq(limitPoint.y)+sq(limitPoint.x))){
    limitOutputPoint.x = mapFloatInt(limitPoint.x,-maxPoint.x,maxPoint.x,-INPUT_MAX,INPUT_MAX);
    limitOutputPoint.y = mapFloatInt(limitPoint.y,-maxPoint.y,maxPoint.y,-INPUT_MAX,INPUT_MAX);
    
  }else {
    limitOutputPoint.x = mapFloatInt(rawPoint.x,-maxPoint.x,maxPoint.x,-INPUT_MAX,INPUT_MAX);
    limitOutputPoint.y = mapFloatInt(rawPoint.y,-maxPoint.y,maxPoint.y,-INPUT_MAX,INPUT_MAX);    
  }

    limitOutputPoint.x = _direction*sgn(rawPoint.x)*limitOutputPoint.x;
    limitOutputPoint.y = _direction*sgn(rawPoint.y)*limitOutputPoint.y;   

    return limitOutputPoint;
}



int LSJoystick::mapFloatInt(float input,float inputStart,float inputEnd, int outputStart, int outputEnd){
  
  float inputRange = inputEnd - inputStart;
  int outputRange = outputEnd - outputStart;
  
  int output = (input - inputStart)*outputRange / inputRange + outputStart;
  
  return output;
}

/*template <typename T>
int LSJoystick::sgn(T val) {
    return (T(0) < val) - (val < T(0));
}*/

int LSJoystick::sgn(float val) {
    return (0.0 < val) - (val < 0.0);
}
