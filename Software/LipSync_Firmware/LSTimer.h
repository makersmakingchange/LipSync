/* 
* File: LSTimer.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: v4.0.rc1 (26 January 2024)
  License: GPL v3.0 or later

  Copyright (C) 2024 Neil Squire Society
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
 
//Header definition
#ifndef _LSTIMER_H
#define _LSTIMER_H

template<typename T>
class LSTimer {
  private:
    const static int MAX_TIMERS = 10;                                             //Maximum number of timers
    const static int RUN_FOREVER = 0;                 
    const static int RUN_ONCE = 1;
    const static int DEFCALL_DONTRUN = 0;                                         //Don't call the callback function
    const static int DEFCALL_RUNONLY = 1;                                         //Call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;                                       //Call the callback function and delete the timer
    typedef void (*timer_callback)(void);
    typedef void (*timer_callback_p)(T *);
    typedef struct {
      unsigned long prev_millis;                                                  //Value returned in the previous run() call
      void* callback;                                                             //Pointer to the callback function
      T* param;                                                                   //Function parameter
      boolean hasParam;                                                           //Check if callback takes a parameter
      unsigned long delay;                                                        //Delay value
      unsigned maxNumRuns;                                                        //Number of runs to be executed
      unsigned numRuns;                                                           //Number of executed runs
      boolean enabled;                                                            //Check if timer is enabled
      unsigned long offset;                                                       //Initial offset delay
      boolean offsetEnabled;                                                      //Check if offset is enabled
      unsigned toBeCalled;                                                        //Deferred function call
    } timer_t;
    timer_t timer[MAX_TIMERS];
    int numTimers; 
    int findFirstFreeSlot();                                                      //Find the first available slot
    int setupTimer(unsigned long d, unsigned long o, boolean on, boolean h, unsigned n, void* f, T* p);                                                               
  public:
    LSTimer();                                                                    //Constructor
    void run();                                                                   //Must be called inside loop()
    int setInterval(unsigned long d, unsigned long o, timer_callback f);          //Call function f every d milliseconds, start in o milliseconds
    int setInterval(unsigned long d, unsigned long o, timer_callback_p f, T* p);
    int setTimeout(unsigned long o, timer_callback f);                            //Call function f once after d milliseconds
    int setTimeout( unsigned long o, timer_callback_p f, T* p);
    int setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback f); //Call function f every d milliseconds for n times, start in o milliseconds
    int setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback_p f, T* p);
    int startTimer(); 
    unsigned long elapsedTime(int timerId);
    void deleteTimer(int numTimer);                                               //Destroy the specified timer
    void restartTimer(int numTimer);                                              //Restart the specified timer
    boolean isEnabled(int numTimer);                                              //Returns true if the specified timer is enabled
    void enable(int numTimer);                                                    //Enables the specified timer
    void disable(int numTimer);                                                   //Disables the specified timer
    void toggle(int numTimer);                                                    //Toggle(Enables/Disables) the specified timer
    int getNumTimers();                                                           //Returns the number of used timers
    int getNumAvailableTimers() { return MAX_TIMERS - numTimers; };               //Returns the number of available timers
    int getNumRuns(int numTimer);                                                 //Returns the number of executed runs
};

static inline unsigned long elapsed() { return millis(); }
template<typename T>
LSTimer<T>::LSTimer() {
    unsigned long current_millis = elapsed();

   for (int i = 0; i < MAX_TIMERS; i++) {
        memset(&timer[i], 0, sizeof (timer_t));
        timer[i].prev_millis = current_millis;
    }

    numTimers = 0;
}

template<typename T>
void LSTimer<T>::run() {
    int i;
    unsigned long current_millis,delay_millis;

    //Get the current time
    current_millis = elapsed();

    for (i = 0; i < MAX_TIMERS; i++) {

        timer[i].toBeCalled = DEFCALL_DONTRUN;

        if (timer[i].callback != NULL) {

            //Check if it's time to process timer

            (timer[i].numRuns==0 && timer[i].offsetEnabled) ? delay_millis=timer[i].offset: delay_millis=timer[i].delay;
            
            if ((current_millis - timer[i].prev_millis) >= delay_millis) {

                //Update time
                timer[i].prev_millis += delay_millis;

                //Check if the timer callback has to be executed
                if (timer[i].enabled) {

                    //RUN_FOREVER timers must always be executed
                    if (timer[i].maxNumRuns == RUN_FOREVER) {
                        timer[i].toBeCalled = DEFCALL_RUNONLY;
                    }
                    //Other timers
                    else if (timer[i].numRuns < timer[i].maxNumRuns) {
                        timer[i].toBeCalled = DEFCALL_RUNONLY;
                        timer[i].numRuns++;

                        //Delete timer after the last run
                        if (timer[i].numRuns >= timer[i].maxNumRuns) {
                            timer[i].toBeCalled = DEFCALL_RUNANDDEL;
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < MAX_TIMERS; i++) {
        if (timer[i].toBeCalled == DEFCALL_DONTRUN)
            continue;

        if (timer[i].hasParam)
            (*(timer_callback_p)timer[i].callback)(timer[i].param);
        else
            (*(timer_callback)timer[i].callback)();

        if (timer[i].toBeCalled == DEFCALL_RUNANDDEL)
            deleteTimer(i);
    }
}


//Find the first available slot
template<typename T>
int LSTimer<T>::findFirstFreeSlot() {
    //All slots are used
    if (numTimers >= MAX_TIMERS) {
        return -1;
    }

    // return the first slot with no callback (i.e. free)
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timer[i].callback == NULL) {
            return i;
        }
    }

    //No available slots
    return -1;
}

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

template<typename T>
int LSTimer<T>::setInterval(unsigned long d, unsigned long o, timer_callback f) {
    return setupTimer(d, o, true, false, RUN_FOREVER, (void *)f, NULL);
}

template<typename T>
int LSTimer<T>::setInterval(unsigned long d, unsigned long o, timer_callback_p f, T* p) {
  return setupTimer(d, o, true, true, RUN_FOREVER, (void *)f, p);
}

template<typename T>
int LSTimer<T>::setTimeout(unsigned long o, timer_callback f) {
    return setupTimer(o, 0, false, false, RUN_ONCE, (void *)f, NULL);
}

template<typename T>
int LSTimer<T>::setTimeout(unsigned long o, timer_callback_p f, T* p) {
  return setupTimer(o, 0, false, true, RUN_ONCE, (void *)f, p);
}

template<typename T>
int LSTimer<T>::setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback f) {
  return setupTimer(d, o, true, false, n, (void *)f, NULL);
}

template<typename T>
int LSTimer<T>::setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback_p f, T* p) {
  return setupTimer(d, o, true, true, n, (void *)f, p);
}

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
template<typename T>
unsigned long LSTimer<T>::elapsedTime(int timerId) {
    if (timerId >= MAX_TIMERS) {
        return 0;
    }

    unsigned long current_millis,diff_millis;

    // get current time
    current_millis = elapsed();
    diff_millis = current_millis - timer[timerId].prev_millis;
    return diff_millis;
}
template<typename T>
void LSTimer<T>::deleteTimer(int timerId) {
    if (timerId >= MAX_TIMERS) {
        return;
    }

    //Don't delete if no timers are in use
    if (numTimers == 0) {
        return;
    }

    //Don't decrease the number of timers if the slot is already empty
    if (timer[timerId].callback != NULL) {
        memset(&timer[timerId], 0, sizeof (timer_t));
        timer[timerId].prev_millis = elapsed();

        // update number of timers
        numTimers--;
    }
}
template<typename T>
void LSTimer<T>::restartTimer(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].prev_millis = elapsed();
    timer[numTimer].numRuns = 0;
}

template<typename T>
boolean LSTimer<T>::isEnabled(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return false;
    }

    return timer[numTimer].enabled;
}

template<typename T>
void LSTimer<T>::enable(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].enabled = true;
}

template<typename T>
void LSTimer<T>::disable(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].enabled = false;
}

template<typename T>
void LSTimer<T>::toggle(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].enabled = !timer[numTimer].enabled;
}

template<typename T>
int LSTimer<T>::getNumTimers() {
    return numTimers;
}

template<typename T>
int LSTimer<T>::getNumRuns(int numTimer) {
    return timer[numTimer].numRuns;
}
#endif 
