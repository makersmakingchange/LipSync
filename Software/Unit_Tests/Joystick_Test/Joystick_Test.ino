
#include <Arduino.h>
#include "Adafruit_TinyUSB.h"
#include <Wire.h>  
#include <StopWatch.h>

#include "LSQueue.h"
#include "LSJoystick.h"
#include "LSOutput.h"

#define LED_BRIGHTNESS 50

#define JOY_MAIN_STATE_CENTER 0
#define JOY_MAIN_STATE_ACTIVE 1
#define JOY_MAIN_STATE_OUT    2

#define JOY_SEC_STATE_WAITING 0
#define JOY_SEC_STATE_STARTED 1
#define JOY_SEC_STATE_RELEASED 2


typedef struct {
  int mainState;            //center = 0, active = 1, out = 2
  int secondaryState;       //waiting = 0, started = 1, released = 2
  int quadState;            //Only for out
  unsigned long elapsedTime;     //in ms
} joystickStateStruct;

int joystickState;

int xVal;
int yVal;

LSQueue <joystickStateStruct> joystickStateQueue(12);   //Create a Queue of type joystickStruct

StopWatch joystickTimer[1];

LSJoystick js;                  //Starts an instance of the LSJoystick object
LSOutput led;                   //Starts an instance of the LSOutput led object

joystickStateStruct joystickCurrState, joystickPrevState, sapActionState;

void setup() {
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  while (!Serial) { delay(1); }
  
  delay(2000);

  initJoystick();
  
  led.begin();    
  
  initLedFeedback();
  
  Scheduler.startLoop(joystickDataloop);
  
} //end setup


//The loop handling joystick polling, sip and puff state evaluation 
void joystickDataloop() {
  
  updateJoystick();               //Request new values

  readJoystick();                 //Read the filtered values 

  printJoystickData(); 

  //joystickPrevState = joystickStateQueue.end();  //Get the previous state
  
/*
  if(joystickPrevState.mainState == joystickState){
    joystickCurrState = {joystickState, joystickPrevState.secondaryState, joystickTimer[0].elapsed()};
    //Serial.println("a");
    joystickStateQueue.update(joystickCurrState);
  } else {  
      if(joystickPrevState.secondaryState==JOY_SEC_STATE_RELEASED && joystickState==JOY_MAIN_STATE_CENTER){
        joystickCurrState = {joystickState, JOY_SEC_STATE_WAITING, 0};
        //Serial.println("b");
      }
      else if(joystickPrevState.secondaryState==JOY_SEC_STATE_RELEASED && joystickState!=JOY_MAIN_STATE_CENTER){
        joystickCurrState = {joystickState, JOY_SEC_STATE_STARTED, 0};
        //Serial.println("c");
      }      
      else if(joystickPrevState.secondaryState==JOY_SEC_STATE_WAITING){
        joystickCurrState = {joystickState, JOY_SEC_STATE_STARTED, 0};
        //Serial.println("d");
      }    
      else if(joystickPrevState.secondaryState==JOY_SEC_STATE_STARTED){
        joystickCurrState = {joystickPrevState.mainState, JOY_SEC_STATE_RELEASED, joystickPrevState.elapsedTime};
        //Serial.println("e");
      }
      //Push the new state   
      joystickStateQueue.push(joystickCurrState);
      if(joystickStateQueue.count()==12){joystickStateQueue.pop(); }  //Keep last 12 objects 
      //Reset and start the timer
      joystickTimer[0].stop();      
      joystickTimer[0].reset();                                                                        
      joystickTimer[0].start(); 
  }

  //No action in 1 minute : reset timer
  if(joystickPrevState.secondaryState==JOY_SEC_STATE_WAITING && joystickTimer[0].elapsed()>60000){
      joystickTimer[0].stop();                                //Reset and start the timer         
      joystickTimer[0].reset();                                                                        
      joystickTimer[0].start();     
  }
*/
  delay(100);
}


void loop() {

}

void updateJoystick() {
  js.update();
}


void readJoystick() {

  joystickInputStruct joystickValues = js.getAllVal();
  xVal = joystickValues.x;
  yVal = joystickValues.y;
 
}

void printJoystickData() {

  Serial.print(" xVal: "); Serial.print(xVal);Serial.print(", ");
  Serial.print(" yVal: "); Serial.print(yVal);Serial.print(", ");
  
  Serial.println();

}


void initJoystick() {

  js.begin();
  js.setMagDirection(MAG_DIRECTION_INVERSE);
  js.setRawComp();
  calibrateJoystick();
}

void calibrateJoystick() {

  led.setLedBlinkById(4,3,500,LED_CLR_YELLOW,LED_BRIGHTNESS);
  delay(1000);
  js.setRawMax(1);
  js.setRawMax(2);
  js.setRawMax(3);
  js.setRawMax(4);
  
  /*
  for (int i=1; i < 5; i++) {
  led.setLedBlinkById(6,3,500,LED_CLR_YELLOW,LED_BRIGHTNESS);
  js.setRawMax(1);
  led.setLedBlinkById(1,3,1000,LED_CLR_RED,LED_BRIGHTNESS);    
  }
  */
}

void printStateData() {

  Serial.print(" main: "); Serial.print(sapActionState.mainState);Serial.print(", ");
  Serial.print(" secondary: "); Serial.print(sapActionState.secondaryState);Serial.print(", ");
  
  Serial.println();
 
}

void initJoystickArray(){

  //Push initial state to state Queue
  
  joystickCurrState = joystickPrevState = {JOY_MAIN_STATE_CENTER, JOY_SEC_STATE_WAITING, 0};
  joystickStateQueue.push(joystickCurrState);

  //Reset and start the timer   
  joystickTimer[0].stop();                                      
  joystickTimer[0].reset();                                                                        
  joystickTimer[0].start(); 
}


void initLedFeedback(){
  led.setLedBlinkById(4,3,500,LED_CLR_GREEN,LED_BRIGHTNESS);
  delay(5);
}
