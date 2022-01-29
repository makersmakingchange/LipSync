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
		const static int MAX_TIMERS = 10;	                // maximum number of timers
		const static int RUN_FOREVER = 0;                 // setTimer() constants
		const static int RUN_ONCE = 1;
		LSTimer();                                        // constructor
		void run();							                          // this function must be called inside loop()
		int setInterval(unsigned long d, unsigned long s, timer_callback f);        // call function f every d milliseconds
    int setInterval(unsigned long d, unsigned long s, timer_callback_p f, void* p);
		int setTimeout(unsigned long s, timer_callback f);         // call function f once after d milliseconds
    int setTimeout( unsigned long s, timer_callback_p f, void* p);
		int setTimer(unsigned long d, unsigned long s, timer_callback f, unsigned n);    // call function f every d milliseconds for n times
    int setTimer(unsigned long d, unsigned long s, timer_callback_p f, void* p, unsigned n);
    int startTimer(); 
    unsigned long elapsedTime(int timerId);
		void deleteTimer(int numTimer);                   // destroy the specified timer
		void restartTimer(int numTimer);                  // restart the specified timer
		boolean isEnabled(int numTimer);                  // returns true if the specified timer is enabled
		void enable(int numTimer);                        // enables the specified timer
		void disable(int numTimer);                       // disables the specified timer
		void toggle(int numTimer);
		int getNumTimers();                               // returns the number of used timers
		int getNumAvailableTimers() { return MAX_TIMERS - numTimers; };       // returns the number of available timers
  
  private:
    const static int DEFCALL_DONTRUN = 0;        // don't call the callback function
    const static int DEFCALL_RUNONLY = 1;        // call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;      // call the callback function and delete the timer
    int findFirstFreeSlot();                     // find the first available slot
    int setupTimer(unsigned long d, unsigned long s, void* f, void* p, boolean h, unsigned n);
    typedef struct {
      unsigned long prev_millis;        // value returned by the millis() function in the previous run() call
      void* callback;                   // pointer to the callback function
      void* param;                      // function parameter
      boolean hasParam;                 // true if callback takes a parameter
      unsigned long delay;              // delay value
      unsigned maxNumRuns;              // number of runs to be executed
      unsigned numRuns;                 // number of executed runs
      boolean enabled;                  // true if enabled
      boolean startup;                  // true if there's startup initial delay
      unsigned toBeCalled;              // deferred function call (sort of) - N.B.: only used in run()
    } timer_t;

    timer_t timer[MAX_TIMERS];

    // actual number of timers in use
    unsigned numTimers;
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

    // get current time
    current_millis = elapsed();

    for (i = 0; i < MAX_TIMERS; i++) {

        timer[i].toBeCalled = DEFCALL_DONTRUN;

        // no callback == no timer, i.e. jump over empty slots
        if (timer[i].callback != NULL) {

            // is it time to process this timer ?

            (timer[i].numRuns==0) ? delay_millis=timer[i].startup : delay_millis=timer[i].delay;
            
            if ((current_millis - timer[i].prev_millis) >= delay_millis) {

                // update time
                timer[i].prev_millis += delay_millis;

                if(timer[i].startup>0){ timer[i].startup=0; }

                // check if the timer callback has to be executed
                if (timer[i].enabled) {

                    // "run forever" timers must always be executed
                    if (timer[i].maxNumRuns == RUN_FOREVER) {
                        timer[i].toBeCalled = DEFCALL_RUNONLY;
                    }
                    // other timers get executed the specified number of times
                    else if (timer[i].numRuns < timer[i].maxNumRuns) {
                        timer[i].toBeCalled = DEFCALL_RUNONLY;
                        timer[i].numRuns++;

                        // after the last run, delete the timer
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


// find the first available slot
// return -1 if none found
int LSTimer::findFirstFreeSlot() {
    // all slots are used
    if (numTimers >= MAX_TIMERS) {
        return -1;
    }

    // return the first slot with no callback (i.e. free)
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timer[i].callback == NULL) {
            return i;
        }
    }

    // no free slots found
    return -1;
}

int LSTimer::setupTimer(unsigned long d, unsigned long s, void* f, void* p, boolean h, unsigned n) {
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
    timer[freeTimer].startup = s;
    timer[freeTimer].prev_millis = elapsed();

    numTimers++;

    return freeTimer;
}

int LSTimer::setInterval(unsigned long d, unsigned long s, timer_callback f) {
    return setupTimer(d, s, (void *)f, NULL, false, RUN_FOREVER);
}

int LSTimer::setInterval(unsigned long d, unsigned long s, timer_callback_p f, void* p) {
  return setupTimer(d, s, (void *)f, p, true, RUN_FOREVER);
}

int LSTimer::setTimeout(unsigned long s, timer_callback f) {
    return setupTimer(0, s, (void *)f, NULL, false, RUN_ONCE);
}

int LSTimer::setTimeout(unsigned long s, timer_callback_p f, void* p) {
  return setupTimer(0, s, (void *)f, p, true, RUN_ONCE);
}

int LSTimer::setTimer(unsigned long d, unsigned long s, timer_callback f, unsigned n) {
  return setupTimer(d, s, (void *)f, NULL, false, n);
}

int LSTimer::setTimer(unsigned long d, unsigned long s, timer_callback_p f, void* p, unsigned n) {
  return setupTimer(d, s, (void *)f, p, true, n);
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
    timer[freeTimer].startup = 0;
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

    // nothing to delete if no timers are in use
    if (numTimers == 0) {
        return;
    }

    // don't decrease the number of timers if the
    // specified slot is already empty
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
