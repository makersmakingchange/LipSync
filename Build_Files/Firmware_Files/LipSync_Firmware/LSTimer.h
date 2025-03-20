/* 
* File: LSTimer.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: v4.1rc (10 March 2025)
  License: GPL v3.0 or later

  Copyright (C) 2024 - 2025 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>
  
* Based on: SimpleTimer.h (https://github.com/jfturcot/SimpleTimer)
*/
 
// Header definition
#ifndef _LSTIMER_H
#define _LSTIMER_H


template<typename T>
class LSTimer {
  private:
    const static int MAX_TIMERS = 10;                                             // Maximum number of timers
    const static int RUN_FOREVER = 0;                                             // Flag used for timer that should run continously                 
    const static int RUN_ONCE = 1;                                                // Flag used for timer that should only run once
    const static int DEFCALL_DONTRUN = 0;                                         // Don't call the callback function
    const static int DEFCALL_RUNONLY = 1;                                         // Call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;                                       // Call the callback function and delete the timer
    const static int MAX_INT = 65535;                                             // Maximum integer used to prevent overrun
    typedef void (*timerCallbackPtr)(void);                                       // Define a function pointer type for a callback function with no parameters.
    typedef void (*timerCallbackParamPtr)(T *);                                   // Define a function pointer type for a callback function with parameters.
    typedef struct {                                                              // Define timer structure
      unsigned long previousTime;                                                 // Last time the timer was triggered 
      void* callback;                                                             // Pointer to the callback function
      T* param;                                                                   // Pointer to the function parameter
      boolean hasParam;                                                           // Check if callback takes a parameter
      unsigned long intervalTime;                                                 // Time between triggers
      unsigned maxNumRuns;                                                        // Number of runs to be executed
      unsigned numRuns;                                                           // Number of executed runs
      boolean enabled;                                                            // Check if timer is enabled
      unsigned long startDelayTime;                                               // Initial start delay
      boolean startDelayEnabled;                                                  // Check if offset is enabled
      unsigned toBeCalled;                                                        // Deferred function call
    } timer_t;
    
    int numTimers; 
    int findFirstFreeSlot();                                                      // Find the first available slot
    int setupTimer(unsigned long interval, unsigned long startDelay, boolean on, boolean hasParam, unsigned n, void* f, T* p);  
    
  public:
    LSTimer();                                                                    // Constructor
    void run();                                                                   // Check whether timers should be triggered and call callbacks. Must be called inside loop()
    int setInterval(unsigned long interval, unsigned long startDelay, timerCallbackPtr f);   // Call function f every interval milliseconds, start in startDelay milliseconds
    int setInterval(unsigned long interval, unsigned long startDelay, timerCallbackParamPtr f, T* p);
    int setTimeout(unsigned long startDelay, timerCallbackPtr f);                 // Call function f once after interval milliseconds
    int setTimeout(unsigned long startDelay, timerCallbackParamPtr f, T* p);      // Call function f once after interval milliseconds with parameters p
    template <typename ObjType> int setTimeout(unsigned long interval, void (ObjType::*method)(), ObjType* obj);  // Overloaded function to handle calls from within class
    int setTimer(unsigned long interval, unsigned long startDelay, unsigned n, timerCallbackPtr f); // Call function f every interval milliseconds for n times, start in startDelay milliseconds
    int setTimer(unsigned long interval, unsigned long startDelay, unsigned n, timerCallbackParamPtr f, T* p);
    int startTimer();                                                             // Start a timer 
    unsigned long elapsedTime(int timerId);                                       // Time elapsed for specificed timer
    int deleteTimer(int timerId);                                                 // Destroy the specified timer
    void restartTimer(int timerId);                                               // Restart the specified timer
    boolean isEnabled(int timerId);                                               // Returns true if the specified timer is enabled
    void enable(int timerId);                                                     // Enables the specified timer
    void disable(int timerId);                                                    // Disables the specified timer
    void toggle(int timerId);                                                     // Toggle(Enables/Disables) the specified timer
    int getNumTimers();                                                           // Returns the number of used timers
    int getNumAvailableTimers() { return MAX_TIMERS - numTimers; };               // Returns the number of available timers
    int getNumRuns(int timerId);                                                  // Returns the number of executed runs
    timer_t timer[MAX_TIMERS];                                                    // Array of timer structures                                                 
                                                             
};


//*********************************//
// Function   : LSTimer 
// 
// Description: Constructor
//
// Arguments :  void
// 
// Return     : void
//*********************************//
template<typename T>
LSTimer<T>::LSTimer() { // TODO 2025-Feb-07 Consider adding overridden function with size argument for better memory manager
    unsigned long current_millis = millis();

   for (int i = 0; i < MAX_TIMERS; i++) {
        memset(&timer[i], 0, sizeof (timer_t)); //  Initialize each timer
        timer[i].previousTime = current_millis;  //  Set the start time for each timer
    }

    numTimers = 0;
}

//*********************************//
// Function   : run 
// 
// Description: This function checks to see which timers should be run and runs them
//
// Arguments :  void
// 
// Return     : void
//*********************************//
template<typename T>
void LSTimer<T>::run() {
    int i;  // Timer index
    unsigned long current_millis; //  Current time
    unsigned long delay_millis;  //  

    // Get the current time
    current_millis = millis();

    // Determine which timers should be called
    for (i = 0; i < MAX_TIMERS; i++) {

        timer[i].toBeCalled = DEFCALL_DONTRUN;  // Default case is not to run

        if (timer[i].callback != NULL) {

            if ((timer[i].numRuns == 0) && timer[i].startDelayEnabled) {  // If offset is enabled, use offset as delay for first run
              delay_millis = timer[i].startDelayTime;
            } else {
              delay_millis = timer[i].intervalTime;
            } 
            
            // Check if it's time to process timer
            if ((current_millis - timer[i].previousTime) >= delay_millis) {

                // Update the time the timer was triggered
                timer[i].previousTime += delay_millis;  //  TODO shouldn't this be the actual triggered time, e.g., current_millis (vs, idealized trigger time when current-prev = delay) (should also be moved to where function is actually called?)

                // Check if the timer callback has to be executed
                if (timer[i].enabled) {

                    //  If a timer is triggered, increment the number of runs
                    if (timer[i].numRuns == (MAX_INT-1)) { // -1 ensures that even / odd remain consistent
                      timer[i].numRuns = 1;  // Reset to avoid overflow
                    } else {
                      timer[i].numRuns++;  //  Increment the number of times   
                    }

                    //  Always executed RUN_FOREVER timers
                    if (timer[i].maxNumRuns == RUN_FOREVER) { //  RUN_FOREVER == 0
                        timer[i].toBeCalled = DEFCALL_RUNONLY;
                    }
                    // Other timers
                    else if (timer[i].numRuns < timer[i].maxNumRuns) {
                        timer[i].toBeCalled = DEFCALL_RUNONLY;
                    }
                    // Delete timer after the last run
                    else if (timer[i].numRuns >= timer[i].maxNumRuns) {
                        timer[i].toBeCalled = DEFCALL_RUNANDDEL;
                    }
                }
            }
        }
    }

    //  Trigger the timers that needs to be run
    for (i = 0; i < MAX_TIMERS; i++) {
      
      if (timer[i].toBeCalled != DEFCALL_DONTRUN){    // Check if timer should be run, if not equal to DONTRUN (either RUNONLY or RUNANDDEL)
          if (timer[i].hasParam) {
            (*(timerCallbackParamPtr)timer[i].callback)(timer[i].param);
          } else {
            (*(timerCallbackPtr)timer[i].callback)();
          }

          if (timer[i].toBeCalled == DEFCALL_RUNANDDEL){  // Check if timer should be deleted 
            deleteTimer(i);
          }
      }

    }
}


//*********************************//
// Function   : findFirstFreeSlot 
// 
// Description: This function finds the first available slot
//
// Arguments :  void
// 
// Return     : void
//*********************************//
template<typename T>
int LSTimer<T>::findFirstFreeSlot() {
    // All slots are used
    if (numTimers >= MAX_TIMERS) {
        return -1;
    }

    // return the first slot with no callback (i.e. free)
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timer[i].callback == NULL) {
            return i;
        }
    }

    // No available slots
    return -1;
}


//*********************************//
// Function   : setupTimer 
// 
// Description: Set up timer
//
// Arguments :  unsigned long : interval : interval timey (ms)
//           :  unsigned long : startDelay : start delay time (ms)
//           :  boolean : on : whether start delay is enabled 
//           :  boolean : h : Whether callback has parameters
//           :  unsigned : n : Maximum number of runs 
//           :  void* : f : callback Function
//           :  T* : p : Callback function parameters
// 
// Return     : int : Timer index
//*********************************//
template<typename T>
int LSTimer<T>::setupTimer(unsigned long interval, unsigned long startDelay, boolean on, boolean h, unsigned n, void* f, T* p) {
    int freeTimerIndex;

    freeTimerIndex = findFirstFreeSlot();
    if (freeTimerIndex < 0) {
        Serial.println("ERROR: No free timers");
        return -1;
    }

    if (f == NULL) {
        return -1;
    }

    timer[freeTimerIndex].intervalTime = interval;
    timer[freeTimerIndex].callback = f;
    timer[freeTimerIndex].param = p;
    timer[freeTimerIndex].hasParam = h;
    timer[freeTimerIndex].maxNumRuns = n;
    timer[freeTimerIndex].numRuns = 0;
    timer[freeTimerIndex].enabled = true;
    timer[freeTimerIndex].startDelayTime = startDelay;
    timer[freeTimerIndex].startDelayEnabled = on;
    timer[freeTimerIndex].previousTime = millis();

    numTimers++;

    return freeTimerIndex;
}


//*********************************//
// Function   : setInterval 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay
//
// Arguments :  unsigned long : interval : Time between timer runs (ms)
//           :  unsigned long : startDelay : Time to delay between start of timer and first run
//           :  timerCallbackPtr : f : Function called when timer runs
// 
// Return     : int : Timer index
//*********************************//
template<typename T>
int LSTimer<T>::setInterval(unsigned long interval, unsigned long startDelay, timerCallbackPtr f) {
    return setupTimer(interval, startDelay, true, false, RUN_FOREVER, (void *)f, NULL);
}


//*********************************//
// Function   : setInterval 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay with callback parameters
//
// Arguments :  unsigned long : interval : Time between timer runs (ms)
//           :  unsigned long : startDelay : Time to delay between start of timer and first run
//           :  timerCallbackPtr : f : Function called when timer runs
//           :  T* : p : Function parameters
// 
// Return     : int : Timer index
//*********************************//
template<typename T>
int LSTimer<T>::setInterval(unsigned long interval, unsigned long startDelay, timerCallbackParamPtr f, T* p) {
  return setupTimer(interval, // delay
                    startDelay, // startDelay
                    true, // startDelay enabled
                    true, // has parameter
                    RUN_FOREVER, //  Keep running
                    (void *)f, //  Callback function
                    p); // Callback function parameter
}


//*********************************//
// Function   : setTimeout 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay
//
// Arguments :  unsigned long : interval : Time between timer runs (ms)
//           :  timerCallbackPtr : f : Function called when timer runs
// 
// Return     : int : Timer index
//*********************************//
template<typename T>
int LSTimer<T>::setTimeout(unsigned long interval, timerCallbackPtr f) {
    return setupTimer(interval, 0, false, false, RUN_ONCE, (void *)f, NULL);
}


//*********************************//
// Function   : setTimeout 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay
//
// Arguments :  unsigned long : interval : Time between timer runs (ms)
//           :  timerCallbackPtr : f : Function called when timer runs
//           :  T* : p : Function parameters
// 
// Return     : int : Timer index
//*********************************//
template<typename T>
int LSTimer<T>::setTimeout(unsigned long interval, timerCallbackParamPtr f, T* p) {
  return setupTimer(interval, 0, false, true, RUN_ONCE, (void *)f, p);
}

template <typename ObjType>
struct TimerCallbackHelper {
    static void callback(void* context) {
        auto* instance = static_cast<ObjType*>(context);
        (instance->*method)();
    }
    static void (ObjType::*method)();  // Store method pointer
};

template <typename ObjType>
void (ObjType::*TimerCallbackHelper<ObjType>::method)() = nullptr;


//*********************************//
// Function   : setTimeout 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay
//
// Arguments :  unsigned long : interval : Time between timer runs (ms)
//           :  timerCallbackPtr : f : Function called when timer runs
//           :  T* : p : Function parameters
//           :  
// 
// Return     : int : Timer index
//*********************************//
template <typename T>
template <typename ObjType>
int LSTimer<T>::setTimeout(unsigned long interval, void (ObjType::*method)(), ObjType* obj) {
    TimerCallbackHelper<ObjType>::method = method;  // Store method pointer
    return setTimeout(interval, TimerCallbackHelper<ObjType>::callback, obj);
}


//*********************************//
// Function   : setTimer 
// 
// Description: This function sets up a timer to call the specified function every interval (ms), up to n times.
//
// Arguments :  unsigned long : interval : Time between function calls (ms)
//             unsigned long : startDelay : Delay before the first call (ms)
//             unsigned n     : n         : Number of times to call the function
//             timerCallbackPtr : f         : Function to be called repeatedly
// 
// Return     : int : Timer index (identifier for the timer)
//*********************************//
template<typename T>
int LSTimer<T>::setTimer(unsigned long interval, unsigned long startDelay, unsigned n, timerCallbackPtr f) {
  return setupTimer(interval, startDelay, true, false, n, (void *)f, NULL);
}


//*********************************//
// Function   : setTimer 
// 
// Description: This function sets up a timer to call the specified function every interval (ms), up to n times with function parameters p.
//
// Arguments :  unsigned long : interval : Time between function calls (ms)
//             unsigned long : startDelay : Delay before the first call (ms)
//             unsigned n     : n         : Number of times to call the function
//             timerCallbackPtr : f         : Function to be called repeatedly
//             p
// 
// Return     : int : Timer index (identifier for the timer)
//*********************************//
template<typename T>
int LSTimer<T>::setTimer(unsigned long interval, unsigned long startDelay, unsigned n, timerCallbackParamPtr f, T* p) {
  return setupTimer(interval, startDelay, true, true, n, (void *)f, p);
}


//*********************************//
// Function   : startTimer 
// 
// Description: This function sets up a timer without any delay, startDelay, or callback to be used like a stopwatch.
//
// Arguments :  void
// 
// Return     : int : freeTimerIndex : Index of first free timer
//*********************************//
template<typename T>
int LSTimer<T>::startTimer() {
    int freeTimerIndex;

    freeTimerIndex = findFirstFreeSlot();
    if (freeTimerIndex < 0) {
        return -1;
    }

    timer[freeTimerIndex].intervalTime = 0;
    timer[freeTimerIndex].callback = 0;
    timer[freeTimerIndex].param = 0;
    timer[freeTimerIndex].hasParam = 0;
    timer[freeTimerIndex].maxNumRuns = 1;
    timer[freeTimerIndex].enabled = true;
    timer[freeTimerIndex].startDelayTime = 0;
    timer[freeTimerIndex].startDelayEnabled = false;
    timer[freeTimerIndex].previousTime = millis();

    numTimers++;

    return freeTimerIndex;
}


//*********************************//
// Function   : elapsedTime 
// 
// Description: Returns the time elapsed since the timer has been started
//
// Arguments :  int : timerId : Index of timer to determine how much time has elapsed
// 
// Return     : unsigned long : Time elasped since last run
//*********************************//
template<typename T>
unsigned long LSTimer<T>::elapsedTime(int timerId) {
    if (timerId >= MAX_TIMERS) {
        return 0;
    }

    unsigned long current_millis, diff_millis;

    // Get current time
    current_millis = millis();
    diff_millis = current_millis - timer[timerId].previousTime;
    return diff_millis;
}


//*********************************//
// Function   : deleteTimer 
// 
// Description: This function deletes the timer corresponding to the timer index
//
// Arguments :  int : timerId : Index of timer to delete
// 
// Return     : void
//*********************************//
template<typename T>
int LSTimer<T>::deleteTimer(int timerId) {
    if (timerId >= MAX_TIMERS) {
        Serial.print("ERROR: Invalid Timer ID: ");
        Serial.println(timerId);
        return -1; // ERROR: Invalid timer ID
    }

    // Don't delete if no timers are in use
    if (numTimers == 0) {
        Serial.print("ERROR: No timers are in use.");
        Serial.println(timerId);
        return -1;
    }

    // Don't decrease the number of timers if the slot is already empty
    if (timer[timerId].callback != NULL) {
        memset(&timer[timerId], 0, sizeof (timer_t));
        timer[timerId].previousTime = millis();

        // update number of timers
        numTimers--;
    }
    return 0;
}


//*********************************//
// Function   : restartTimer 
// 
// Description: Restart the specified timer
//
// Arguments :  int : timerId : Index of the timer to restart
// 
// Return     : void
//*********************************//
template<typename T>
void LSTimer<T>::restartTimer(int timerId) {
    if (timerId >= MAX_TIMERS) {
        Serial.print("ERROR: Invalid Timer ID: ");
        Serial.println(timerId);
        return;
    }

    timer[timerId].previousTime = millis();
    timer[timerId].numRuns = 0;
}


//*********************************//
// Function   : isEnabled 
// 
// Description: Returns whether the specified timer is enabled
//
// Arguments :  int : timerId : Index of timer to check if enabled
// 
// Return     : boolean : true - timer enabled, false - timer disabled
//*********************************//
template<typename T>
boolean LSTimer<T>::isEnabled(int timerId) {
    if (timerId >= MAX_TIMERS) {
        Serial.print("ERROR: Invalid Timer ID: ");
        Serial.println(timerId);
        return false;
    }

    return timer[timerId].enabled;
}


//*********************************//
// Function   : enable 
// 
// Description: Enable the specified timer
//
// Arguments :  int : timerId : Index of the timer to enable
// 
// Return     : void
//*********************************//
template<typename T>
void LSTimer<T>::enable(int timerId) {
    if (timerId >= MAX_TIMERS) {
        Serial.print("ERROR: Invalid Timer ID: ");
        Serial.println(timerId);
        return;
    }

    timer[timerId].enabled = true;
}


//*********************************//
// Function   : disable 
// 
// Description: Disables the specified timer
//
// Arguments :  int : timerId : Index of timer to disable
// 
// Return     : void
//*********************************//
template<typename T>
void LSTimer<T>::disable(int timerId) {
    if (timerId >= MAX_TIMERS) {
        Serial.print("ERROR: Invalid Timer ID: ");
        Serial.println(timerId);
        return;
    }

    timer[timerId].enabled = false;
}


//*********************************//
// Function   : toggle 
// 
// Description: Toggles the specified timer between enabled and disabled.
//
// Arguments :  int : timerId : Index of timer to toggle
// 
// Return     : void
//*********************************//
template<typename T>
void LSTimer<T>::toggle(int timerId) {
    if (timerId >= MAX_TIMERS) {
        Serial.print("ERROR: Invalid Timer ID: ");
        Serial.println(timerId);
        return;
    }

    timer[timerId].enabled = !timer[timerId].enabled;
}


//*********************************//
// Function   : getNumTimers 
// 
// Description: Return the number of timers.
//
// Arguments :  void
// 
// Return     : int : numTimers : Number of timers
//*********************************//
template<typename T>
int LSTimer<T>::getNumTimers() {
    return numTimers;
}


//*********************************//
// Function   : getNumRuns 
// 
// Description: Return the number of runs for the specified timer
//
// Arguments :  int : timerId : Index of timer to get number of runs
// 
// Return     : int : numRuns : The number of times the timer has been activated.
//*********************************//
template<typename T>
int LSTimer<T>::getNumRuns(int timerId) {
    return timer[timerId].numRuns;
}

#endif 