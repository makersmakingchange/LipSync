#include <Tlv493d.h>  //Infinion TLV493 magnetic sensor

#define STACK_SIZE 10

Tlv493d Tlv493dSensor = Tlv493d();

typedef struct {
  float xVal;
  float yVal;
} joystickRawType;

class LSJoystick {
  public:
    LSJoystick();
    joystickRawType joystickRawStack[STACK_SIZE];    
    void begin();                                    
    //int pointer;                                                  //Pointer to top of the stack
    void clear();                                                  //Clear the stack
    void update();                                                                                                 //Pop last dot or dash from the stack    
    float getXVal();
    float getYVal();
    joystickRawType getAll();
};

LSJoystick::LSJoystick() {

}

void LSJoystick::begin() {
  Tlv493dSensor.begin();
  clear();
}

void LSJoystick::clear() {
  Tlv493dSensor.updateData();
  for (int i=0; i < STACK_SIZE; i++) {
     joystickRawStack[i] =  {Tlv493dSensor.getY(),
     Tlv493dSensor.getX()};
  }
}

void LSJoystick::update() {
  Tlv493dSensor.updateData();
  memmove( joystickRawStack, &joystickRawStack[1], (STACK_SIZE-1) * sizeof(joystickRawStack[0]));
  joystickRawStack[STACK_SIZE-1] = {Tlv493dSensor.getY(),
  Tlv493dSensor.getX()};
}

float LSJoystick::getXVal() {
  return joystickRawStack[STACK_SIZE-1].xVal;
}

float LSJoystick::getYVal() {
  return joystickRawStack[STACK_SIZE-1].yVal;
}


joystickRawType LSJoystick::getAll() {
  return joystickRawStack[STACK_SIZE-1];
}
