/* 
* File: LSTimer.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: v4.1rc (20 January 2025)
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
    const static int RUN_FOREVER = 0;                 
    const static int RUN_ONCE = 1;
    const static int DEFCALL_DONTRUN = 0;                                         // Don't call the callback function
    const static int DEFCALL_RUNONLY = 1;                                         // Call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;                                       // Call the callback function and delete the timer
    const static int MAX_INT = 65535;
    typedef void (*timer_callback)(void);
    typedef void (*timer_callback_p)(T *);
    typedef struct {
      unsigned long prev_millis;                                                  // Last time the timer was triggered 
      void* callback;                                                             // Pointer to the callback function
      T* param;                                                                   // Pointer to the function parameter
      boolean hasParam;                                                           // Check if callback takes a parameter
      unsigned long delay;                                                        // Delay value
      unsigned maxNumRuns;                                                        // Number of runs to be executed
      unsigned numRuns;                                                           // Number of executed runs
      boolean enabled;                                                            // Check if timer is enabled
      unsigned long offset;                                                       // Initial offset delay
      boolean offsetEnabled;                                                      // Check if offset is enabled
      unsigned toBeCalled;                                                        // Deferred function call
    } timer_t;
    
    int numTimers; 
    int findFirstFreeSlot();                                                      // Find the first available slot
    int setupTimer(unsigned long d, unsigned long o, boolean on, boolean h, unsigned n, void* f, T* p);  
    
  public:
    LSTimer();                                                                    // Constructor
    void run();                                                                   // Must be called inside loop()
    int setInterval(unsigned long interval, unsigned long o, timer_callback f);   // Call function f every d milliseconds, start in o milliseconds
    int setInterval(unsigned long d, unsigned long o, timer_callback_p f, T* p);
    int setTimeout(unsigned long o, timer_callback f);                            // Call function f once after d milliseconds
    int setTimeout(unsigned long o, timer_callback_p f, T* p);
    int setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback f); // Call function f every d milliseconds for n times, start in o milliseconds
    int setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback_p f, T* p);
    int startTimer(); 
    unsigned long elapsedTime(int timerId);                                       // Time elapsed
    void deleteTimer(int numTimer);                                               // Destroy the specified timer
    void restartTimer(int numTimer);                                              // Restart the specified timer
    boolean isEnabled(int numTimer);                                              // Returns true if the specified timer is enabled
    void enable(int numTimer);                                                    // Enables the specified timer
    void disable(int numTimer);                                                   // Disables the specified timer
    void toggle(int numTimer);                                                    // Toggle(Enables/Disables) the specified timer
    int getNumTimers();                                                           // Returns the number of used timers
    int getNumAvailableTimers() { return MAX_TIMERS - numTimers; };               // Returns the number of available timers
    int getNumRuns(int numTimer);                                                 // Returns the number of executed runs
    timer_t timer[MAX_TIMERS];                                                    // Array of timer structures                                                 
                                                             
};

static inline unsigned long elapsed() { return millis(); }


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
    unsigned long current_millis = elapsed();

   for (int i = 0; i < MAX_TIMERS; i++) {
        memset(&timer[i], 0, sizeof (timer_t)); //  Initialize each timer
        timer[i].prev_millis = current_millis;  //  Set the start time for each timer
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
    current_millis = elapsed();

    // Determine which timers should be called
    for (i = 0; i < MAX_TIMERS; i++) {

        timer[i].toBeCalled = DEFCALL_DONTRUN;  // Default case is not to run

        if (timer[i].callback != NULL) {

            if ((timer[i].numRuns == 0) && timer[i].offsetEnabled) {  // If offset is enabled, use offset as delay for first run
              delay_millis = timer[i].offset;
            } else {
              delay_millis = timer[i].delay;
            } 
            
            // Check if it's time to process timer
            if ((current_millis - timer[i].prev_millis) >= delay_millis) {

                // Update the time the timer was triggered
                timer[i].prev_millis += delay_millis;  //  TODO shouldn't this be the actual triggered time, e.g., current_millis (vs, idealized trigger time when current-prev = delay) (should also be moved to where function is actually called?)

                // Check if the timer callback has to be executed
                if (timer[i].enabled) {

                    //  Always executed RUN_FOREVER timers
                    if (timer[i].maxNumRuns == RUN_FOREVER) {
                        timer[i].toBeCalled = DEFCALL_RUNONLY;
                    }
                    // Other timers
                    else if (timer[i].numRuns < timer[i].maxNumRuns) {
                        timer[i].toBeCalled = DEFCALL_RUNONLY;

                        // Delete timer after the last run
                        if (timer[i].numRuns >= timer[i].maxNumRuns) {
                            timer[i].toBeCalled = DEFCALL_RUNANDDEL;
                        }
                    }
                }
            }
        }
    }

    //  Run the timers that needs to be run
    for (i = 0; i < MAX_TIMERS; i++) {
      switch(timer[i].toBeCalled) {
        case DEFCALL_DONTRUN:
          break;
        case DEFCALL_RUNONLY:
          if (timer[i].hasParam) {
            (*(timer_callback_p)timer[i].callback)(timer[i].param);
          } else {
            (*(timer_callback)timer[i].callback)();
          }
          if (timer[i].numRuns == (MAX_INT-1)) { // -1 ensures that even / odd remain consistent
            timer[i].numRuns = 1;  // Reset to avoid overflow
          } else {
            timer[i].numRuns++;  //  Increment the number of times   
          }
          break;
        case DEFCALL_RUNANDDEL:
          if (timer[i].hasParam) {
            (*(timer_callback_p)timer[i].callback)(timer[i].param);
          } else {
            (*(timer_callback)timer[i].callback)();
          }
          deleteTimer(i);
          break;

      }

    }
}


//*********************************//
// Function   : findFirstFreeSlot 
// 
// Description: This function finds the first available slotn
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
// Arguments :  unsigned long : d : delay (ms)
//           :  unsigned long : o : offset (ms)
//           :  boolean : on : whether offset is enabled
//           :  boolean : h : Whether callback has parameters
//           :  unsigned : n : Maximum number of runs 
//           :  void* : f : callback Function
//           :  T* : p : Callback function parameters
// 
// Return     : int : Timer index
//*********************************//
template<typename T>
int LSTimer<T>::setupTimer(unsigned long d, unsigned long o, boolean on, boolean h, unsigned n, void* f, T* p) {
    int freeTimer;

    freeTimer = findFirstFreeSlot();
    if (freeTimer < 0) {
        return -1;
    }

    if (f == NULL) {
        return -1;
    }

    timer[freeTimer].delay = d;
    timer[freeTimer].callback = f;
    timer[freeTimer].param = p;
    timer[freeTimer].hasParam = h;
    timer[freeTimer].maxNumRuns = n;
    timer[freeTimer].numRuns = 0;
    timer[freeTimer].enabled = true;
    timer[freeTimer].offset = o;
    timer[freeTimer].offsetEnabled = on;
    timer[freeTimer].prev_millis = elapsed();

    numTimers++;

    return freeTimer;
}


//*********************************//
// Function   : setInterval 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay
//
// Arguments :  unsigned long : interval : Time between timer runs (ms)
//           :  unsigned long : startDelay : Time to delay between start of timer and first run
//           :  timer_callback : f : Function called when timer runs
// 
// Return     : int : Timer index
//*********************************//
template<typename T>
int LSTimer<T>::setInterval(unsigned long interval, unsigned long startDelay, timer_callback f) {
    return setupTimer(interval, startDelay, true, false, RUN_FOREVER, (void *)f, NULL);
}


//*********************************//
// Function   : setInterval 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay with callback parameters
//
// Arguments :  unsigned long : interval : Time between timer runs (ms)
//           :  unsigned long : startDelay : Time to delay between start of timer and first run
//           :  timer_callback : f : Function called when timer runs
//           :  T* : p : Function parameters
// 
// Return     : int : Timer index
//*********************************//
template<typename T>
int LSTimer<T>::setInterval(unsigned long interval, unsigned long startDelay, timer_callback_p f, T* p) {
  return setupTimer(interval, // delay
                    startDelay, // offset
                    true, // offset enabled
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
//           :  timer_callback : f : Function called when timer runs
// 
// Return     : int : Timer index
//*********************************//
template<typename T>
int LSTimer<T>::setTimeout(unsigned long interval, timer_callback f) {
    return setupTimer(interval, 0, false, false, RUN_ONCE, (void *)f, NULL);
}


//*********************************//
// Function   : setTimeout 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay
//
// Arguments :  unsigned long : interval : Time between timer runs (ms)
//           :  timer_callback : f : Function called when timer runs
//           :  T* : p : Function parameters
// 
// Return     : int : Timer index
//*********************************//
template<typename T>
int LSTimer<T>::setTimeout(unsigned long interval, timer_callback_p f, T* p) {
  return setupTimer(interval, 0, false, true, RUN_ONCE, (void *)f, p);
}


//*********************************//
// Function   : setTimer 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay
//
// Arguments :  void
// 
// Return     : void
//*********************************//
template<typename T>
int LSTimer<T>::setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback f) {
  return setupTimer(d, o, true, false, n, (void *)f, NULL);
}


//*********************************//
// Function   : setTimer 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay
//
// Arguments :  void
// 
// Return     : void
//*********************************//
template<typename T>
int LSTimer<T>::setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback_p f, T* p) {
  return setupTimer(d, o, true, true, n, (void *)f, p);
}


//*********************************//
// Function   : startTimer 
// 
// Description: This function sets up a timer without any delay, offset, or callback to be used like a stopwatch.
//
// Arguments :  void
// 
// Return     : int : freeTimer : Index of first free timer
//*********************************//
template<typename T>
int LSTimer<T>::startTimer() {
    int freeTimer;

    freeTimer = findFirstFreeSlot();
    if (freeTimer < 0) {
        return -1;
    }

    timer[freeTimer].delay = 0;
    timer[freeTimer].callback = 0;
    timer[freeTimer].param = 0;
    timer[freeTimer].hasParam = 0;
    timer[freeTimer].maxNumRuns = 1;
    timer[freeTimer].enabled = true;
    timer[freeTimer].offset = 0;
    timer[freeTimer].offsetEnabled = false;
    timer[freeTimer].prev_millis = elapsed();

    numTimers++;

    return freeTimer;
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
    current_millis = elapsed();
    diff_millis = current_millis - timer[timerId].prev_millis;
    return diff_millis;
}


//*********************************//
// Function   : deleteTimer 
// 
// Description: This function sets up a timer to run forever every interval ms after a startDelay
//
// Arguments :  int : timerId : Index of timer to delete
// 
// Return     : void
//*********************************//
template<typename T>
void LSTimer<T>::deleteTimer(int timerId) {
    if (timerId >= MAX_TIMERS) {
        return;
    }

    // Don't delete if no timers are in use
    if (numTimers == 0) {
        return;
    }

    // Don't decrease the number of timers if the slot is already empty
    if (timer[timerId].callback != NULL) {
        memset(&timer[timerId], 0, sizeof (timer_t));
        timer[timerId].prev_millis = elapsed();

        // update number of timers
        numTimers--;
    }
}


//*********************************//
// Function   : restartTimer 
// 
// Description: Restart the specified timer
//
// Arguments :  int : numTimer : Index of the timer to restart
// 
// Return     : void
//*********************************//
template<typename T>
void LSTimer<T>::restartTimer(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].prev_millis = elapsed();
    timer[numTimer].numRuns = 0;
}


//*********************************//
// Function   : isEnabled 
// 
// Description: Returns whether the specified timer is enabled
//
// Arguments :  int : numTimer : Index of timer to check if enabled
// 
// Return     : boolean : true - timer enabled, false - timer disabled
//*********************************//
template<typename T>
boolean LSTimer<T>::isEnabled(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return false;
    }

    return timer[numTimer].enabled;
}


//*********************************//
// Function   : enable 
// 
// Description: Enable the specified timer
//
// Arguments :  int : numTimer : Index of the timer to enable
// 
// Return     : void
//*********************************//
template<typename T>
void LSTimer<T>::enable(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].enabled = true;
}


//*********************************//
// Function   : disable 
// 
// Description: Disables the specified timer
//
// Arguments :  int : numTimer : Index of timer to disable
// 
// Return     : void
//*********************************//
template<typename T>
void LSTimer<T>::disable(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].enabled = false;
}


//*********************************//
// Function   : toggle 
// 
// Description: Toggles the specified timer between enabled and disabled.
//
// Arguments :  int : numTimer : Index of timer to toggle
// 
// Return     : void
//*********************************//
template<typename T>
void LSTimer<T>::toggle(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].enabled = !timer[numTimer].enabled;
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
// Arguments :  int : numTimer : Index of timer to get number of runs
// 
// Return     : int : numRuns : The number of times the timer has been activated.
//*********************************//
template<typename T>
int LSTimer<T>::getNumRuns(int numTimer) {
    return timer[numTimer].numRuns;
}

#endif 