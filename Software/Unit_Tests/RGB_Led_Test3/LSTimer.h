/*
 * Based on SimpleTimer.h
 *
 */
//Header definition
#ifndef _LSTIMER_H
#define _LSTIMER_H

typedef void (*timer_callback)(void);
typedef void (*timer_callback_p)(void *);

class LSTimer {
  public:
    const static int MAX_TIMERS = 10;                                             //Maximum number of timers
    const static int RUN_FOREVER = 0;                 
    const static int RUN_ONCE = 1;
    LSTimer();                                                                    //Constructor
    void run();                                                                   //Must be called inside loop()
    int setInterval(unsigned long d, unsigned long o, timer_callback f);          //Call function f every d milliseconds, start in o milliseconds
    int setInterval(unsigned long d, unsigned long o, timer_callback_p f, void* p);
    int setTimeout(unsigned long o, timer_callback f);                            //Call function f once after d milliseconds
    int setTimeout( unsigned long o, timer_callback_p f, void* p);
    int setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback f); //Call function f every d milliseconds for n times, start in o milliseconds
    int setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback_p f, void* p);
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
  private:
    const static int DEFCALL_DONTRUN = 0;                                         //Don't call the callback function
    const static int DEFCALL_RUNONLY = 1;                                         //Call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;                                       //Call the callback function and delete the timer
    int findFirstFreeSlot();                                                      //Find the first available slot
    int setupTimer(unsigned long d, unsigned long o, boolean on, boolean h, unsigned n, void* f, void* p);
    typedef struct {
      unsigned long prev_millis;                                                  //Value returned in the previous run() call
      void* callback;                                                             //Pointer to the callback function
      void* param;                                                                //Function parameter
      boolean hasParam;                                                           //Check if callback takes a parameter
      unsigned long delay;                                                        //Delay value
      unsigned maxNumRuns;                                                        //Number of runs to be executed
      unsigned numRuns;                                                           //Number of executed runs
      boolean enabled;                                                            //Check if timer is enabled
      boolean offset;                                                             //Initial offset delay
      boolean offsetEnabled;                                                      //Check if offset is enabled
      unsigned toBeCalled;                                                        //Deferred function call
    } timer_t;
    timer_t timer[MAX_TIMERS];
    unsigned numTimers;                                                           //Number of timers in use
};

static inline unsigned long elapsed() { return millis(); }

LSTimer::LSTimer() {
    unsigned long current_millis = elapsed();

   for (int i = 0; i < MAX_TIMERS; i++) {
        memset(&timer[i], 0, sizeof (timer_t));
        timer[i].prev_millis = current_millis;
    }

    numTimers = 0;
}


void LSTimer::run() {
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
int LSTimer::findFirstFreeSlot() {
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

int LSTimer::setupTimer(unsigned long d, unsigned long o, boolean on, boolean h, unsigned n, void* f, void* p) {
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

int LSTimer::setInterval(unsigned long d, unsigned long o, timer_callback f) {
    return setupTimer(d, o, true, false, RUN_FOREVER, (void *)f, NULL);
}

int LSTimer::setInterval(unsigned long d, unsigned long o, timer_callback_p f, void* p) {
  return setupTimer(d, o, true, true, RUN_FOREVER, (void *)f, p);
}

int LSTimer::setTimeout(unsigned long o, timer_callback f) {
    return setupTimer(o, 0, false, false, RUN_ONCE, (void *)f, NULL);
}

int LSTimer::setTimeout(unsigned long o, timer_callback_p f, void* p) {
  return setupTimer(o, 0, false, true, RUN_ONCE, (void *)f, p);
}

int LSTimer::setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback f) {
  return setupTimer(d, o, true, false, n, (void *)f, NULL);
}

int LSTimer::setTimer(unsigned long d, unsigned long o, unsigned n, timer_callback_p f, void* p) {
  return setupTimer(d, o, true, true, n, (void *)f, p);
}

int LSTimer::startTimer() {
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

unsigned long LSTimer::elapsedTime(int timerId) {
    if (timerId >= MAX_TIMERS) {
        return 0;
    }

    unsigned long current_millis,diff_millis;

    // get current time
    current_millis = elapsed();
    diff_millis = current_millis - timer[timerId].prev_millis;
    return diff_millis;
}

void LSTimer::deleteTimer(int timerId) {
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

void LSTimer::restartTimer(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].prev_millis = elapsed();
    timer[numTimer].numRuns = 0;
}


boolean LSTimer::isEnabled(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return false;
    }

    return timer[numTimer].enabled;
}


void LSTimer::enable(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].enabled = true;
}


void LSTimer::disable(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].enabled = false;
}


void LSTimer::toggle(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    timer[numTimer].enabled = !timer[numTimer].enabled;
}


int LSTimer::getNumTimers() {
    return numTimers;
}

#endif 
