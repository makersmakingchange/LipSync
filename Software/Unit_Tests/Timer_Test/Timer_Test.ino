
#include <Adafruit_TinyUSB.h> 
#include "LSTimer.h"
int intervalTimerId[3];
int stateTimerId;
int countdownTimerId;
int timeoutTimerId;

LSTimer<void> intervalTimer;
LSTimer<int> mainStateTimer;
LSTimer<int> countdownTimer;
LSTimer<int> timeoutTimer;

#define DEBUG_LOOP 1

void setup() {
  Serial.begin(115200);                                       //Start Serial
  while( !TinyUSBDevice.mounted() ) delay(1);
  while (!Serial) { delay(1); }                               //Start Serial
  Serial.println("Serial connected");
  stateTimerId =  mainStateTimer.startTimer();

  intervalTimerId[0] = intervalTimer.setInterval(1500, 5000, pollLoop1); // call pollLoop1 every 1.5 seconds after an initial delay of 5000
  intervalTimerId[1] = intervalTimer.setInterval(5000, 0, pollLoop2);
  intervalTimerId[2] = intervalTimer.setInterval(5000, 2500, pollLoop3);

  // Serial.print("Number of Timers: ");
  // Serial.println(intervalTimer.getNumTimers());

  // Serial.print("Poll0.isEnabled = ");
  // Serial.println(intervalTimer.isEnabled(intervalTimerId[0]));

  // Serial.print("Poll1.isEnabled = ");
  // Serial.println(intervalTimer.isEnabled(intervalTimerId[1]));

  // Serial.print("Poll2.isEnabled = ");
  // Serial.println(intervalTimer.isEnabled(intervalTimerId[2]));

  // Start a timer with a 2 second interval, 10 second delay that executes the countdownEffect function 10 times.
  countdownTimerId = countdownTimer.setTimer(2000, 10000, 10, countdownEffect);


// Serial.println("============");
//   Serial.println("intervalTimer 0");
//   Serial.print( "delay: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].delay);
//   Serial.print( "offset: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].offset);
//   Serial.print( "offsetEnabled: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].offsetEnabled);
//   Serial.print( "hasParam: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].hasParam);
//   Serial.print( "enabled: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].enabled);
//   Serial.print( "prev_millis: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].prev_millis);
//   Serial.print( "toBeCalled: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].toBeCalled);
//   Serial.print( "numRuns: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].numRuns);
//   Serial.print( "maxNumRuns: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].maxNumRuns);

//   Serial.println("intervalTimer 1");
//   Serial.print( "delay: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].delay);
//   Serial.print( "offset: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].offset);
//   Serial.print( "offsetEnabled: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].offsetEnabled);
//   Serial.print( "hasParam: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].hasParam);
//   Serial.print( "enabled: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].enabled);
//   Serial.print( "prev_millis: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].prev_millis);
//   Serial.print( "toBeCalled: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].toBeCalled);
//   Serial.print( "numRuns: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].numRuns);
//   Serial.print( "maxNumRuns: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].maxNumRuns);

//   Serial.println("countdownTimer");
//   Serial.print( "delay: "); Serial.println(countdownTimer.timer[countdownTimerId].delay);
//   Serial.print( "offset: "); Serial.println(countdownTimer.timer[countdownTimerId].offset);
//   Serial.print( "offsetEnabled: "); Serial.println(countdownTimer.timer[countdownTimerId].offsetEnabled);
//   Serial.print( "hasParam: "); Serial.println(countdownTimer.timer[countdownTimerId].hasParam);
//   Serial.print( "enabled: "); Serial.println(countdownTimer.timer[countdownTimerId].enabled);
//   Serial.print( "prev_millis: "); Serial.println(countdownTimer.timer[countdownTimerId].prev_millis);
//   Serial.print( "toBeCalled: "); Serial.println(countdownTimer.timer[countdownTimerId].toBeCalled);
//   Serial.print( "numRuns: "); Serial.println(countdownTimer.timer[countdownTimerId].numRuns);
//   Serial.print( "maxNumRuns: "); Serial.println(countdownTimer.timer[countdownTimerId].maxNumRuns);

}

void loop() {
  // put your main code here, to run repeatedly:
  intervalTimer.run();
  mainStateTimer.run();
  countdownTimer.run();
  if(mainStateTimer.elapsedTime(stateTimerId) >= 2000) {
    Serial.print("MainState Elapsed: ");
    Serial.println(millis());
    mainStateTimer.restartTimer(stateTimerId);

// Serial.println("============");
//   Serial.println("intervalTimer 0");
//   Serial.print( "delay: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].delay);
//   Serial.print( "offset: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].offset);
//   Serial.print( "offsetEnabled: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].offsetEnabled);
//   Serial.print( "hasParam: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].hasParam);
//   Serial.print( "enabled: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].enabled);
//   Serial.print( "prev_millis: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].prev_millis);
//   Serial.print( "toBeCalled: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].toBeCalled);
//   Serial.print( "numRuns: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].numRuns);
//   Serial.print( "maxNumRuns: "); Serial.println(intervalTimer.timer[intervalTimerId[0]].maxNumRuns);

//   Serial.println("intervalTimer 1");
//   Serial.print( "delay: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].delay);
//   Serial.print( "offset: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].offset);
//   Serial.print( "offsetEnabled: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].offsetEnabled);
//   Serial.print( "hasParam: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].hasParam);
//   Serial.print( "enabled: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].enabled);
//   Serial.print( "prev_millis: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].prev_millis);
//   Serial.print( "toBeCalled: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].toBeCalled);
//   Serial.print( "numRuns: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].numRuns);
//   Serial.print( "maxNumRuns: "); Serial.println(intervalTimer.timer[intervalTimerId[1]].maxNumRuns);

//   Serial.println("countdownTimer");
//   Serial.print( "delay: "); Serial.println(countdownTimer.timer[countdownTimerId].delay);
//   Serial.print( "offset: "); Serial.println(countdownTimer.timer[countdownTimerId].offset);
//   Serial.print( "offsetEnabled: "); Serial.println(countdownTimer.timer[countdownTimerId].offsetEnabled);
//   Serial.print( "hasParam: "); Serial.println(countdownTimer.timer[countdownTimerId].hasParam);
//   Serial.print( "enabled: "); Serial.println(countdownTimer.timer[countdownTimerId].enabled);
//   Serial.print( "prev_millis: "); Serial.println(countdownTimer.timer[countdownTimerId].prev_millis);
//   Serial.print( "toBeCalled: "); Serial.println(countdownTimer.timer[countdownTimerId].toBeCalled);
//   Serial.print( "numRuns: "); Serial.println(countdownTimer.timer[countdownTimerId].numRuns);
//   Serial.print( "maxNumRuns: "); Serial.println(countdownTimer.timer[countdownTimerId].maxNumRuns);
   

  }
  //if (DEBUG_LOOP){ Serial.println("loop");}
  //delay(500);
}


void pollLoop1() {
  if (DEBUG_LOOP){
    Serial.print("pollLoop1: ");
    Serial.print(millis());
    Serial.print(" : ");
    Serial.println(intervalTimer.getNumRuns(intervalTimerId[0]));
  }

}

void pollLoop2() {
  if (DEBUG_LOOP){
    Serial.print("pollLoop2: ");
    Serial.print(millis());
    Serial.print(" : ");
    Serial.println(intervalTimer.getNumRuns(intervalTimerId[1]));
  }
}

void pollLoop3() {
  if (DEBUG_LOOP) {
    Serial.print("pollLoop3: ");
    Serial.print(millis());
    Serial.print(" : ");
    Serial.println(intervalTimer.getNumRuns(intervalTimerId[2]));
  }
}

void countdownEffect() {
  Serial.print("Countdown: ");
  Serial.print(10 - countdownTimer.getNumRuns(countdownTimerId));
  Serial.print(" : ");
  Serial.print(millis());
  Serial.print(" : ");
  Serial.println(countdownTimer.elapsedTime(countdownTimerId));

}



