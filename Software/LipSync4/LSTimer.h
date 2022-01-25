/*
 * Based on LSTimer.h
 *
 */

typedef void (*timer_callback)(void);

class LSTimer {
	public:
		const static int MAX_TIMERS = 10;	                // maximum number of timers
		const static int RUN_FOREVER = 0;                 // setTimer() constants
		const static int RUN_ONCE = 1;
		LSTimer();                                        // constructor
		void run();							                          // this function must be called inside loop()
		int setInterval(long d, timer_callback f);        // call function f every d milliseconds
		int setTimeout(long d, timer_callback f);         // call function f once after d milliseconds
		int setTimer(long d, timer_callback f, int n);    // call function f every d milliseconds for n times
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
    unsigned long prev_millis[MAX_TIMERS];       // value returned by the millis() function in the previous run() call
    timer_callback callbacks[MAX_TIMERS];        // pointers to the callback functions
    unsigned long delays[MAX_TIMERS];            // delay values
    int maxNumRuns[MAX_TIMERS];                  // number of runs to be executed for each timer
    int numRuns[MAX_TIMERS];                     // number of executed runs for each timer
    boolean enabled[MAX_TIMERS];                 // which timers are enabled
    int toBeCalled[MAX_TIMERS];                  // this array is only used in run()
    int numTimers;                               // actual number of timers in use
};

static inline unsigned long elapsed() { return millis(); }

LSTimer::LSTimer() {
    unsigned long current_millis = elapsed();

    for (int i = 0; i < MAX_TIMERS; i++) {
        enabled[i] = false;
        callbacks[i] = 0;                   // if the callback pointer is zero, the slot is free, i.e. doesn't "contain" any timer
        prev_millis[i] = current_millis;
        numRuns[i] = 0;
    }

    numTimers = 0;
}


void LSTimer::run() {
    int i;
    unsigned long current_millis;

    // get current time
    current_millis = elapsed();

    for (i = 0; i < MAX_TIMERS; i++) {

        toBeCalled[i] = DEFCALL_DONTRUN;

        // no callback == no timer, i.e. jump over empty slots
        if (callbacks[i]) {

            // is it time to process this timer ?

            if (current_millis - prev_millis[i] >= delays[i]) {

                // update time
                prev_millis[i] += delays[i];

                // check if the timer callback has to be executed
                if (enabled[i]) {

                    // "run forever" timers must always be executed
                    if (maxNumRuns[i] == RUN_FOREVER) {
                        toBeCalled[i] = DEFCALL_RUNONLY;
                    }
                    // other timers get executed the specified number of times
                    else if (numRuns[i] < maxNumRuns[i]) {
                        toBeCalled[i] = DEFCALL_RUNONLY;
                        numRuns[i]++;

                        // after the last run, delete the timer
                        if (numRuns[i] >= maxNumRuns[i]) {
                            toBeCalled[i] = DEFCALL_RUNANDDEL;
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < MAX_TIMERS; i++) {
        switch(toBeCalled[i]) {
            case DEFCALL_DONTRUN:
                break;

            case DEFCALL_RUNONLY:
                (*callbacks[i])();
                break;

            case DEFCALL_RUNANDDEL:
                (*callbacks[i])();
                deleteTimer(i);
                break;
        }
    }
}


// find the first available slot
// return -1 if none found
int LSTimer::findFirstFreeSlot() {
    int i;

    // all slots are used
    if (numTimers >= MAX_TIMERS) {
        return -1;
    }

    // return the first slot with no callback (i.e. free)
    for (i = 0; i < MAX_TIMERS; i++) {
        if (callbacks[i] == 0) {
            return i;
        }
    }

    // no free slots found
    return -1;
}


int LSTimer::setTimer(long d, timer_callback f, int n) {
    int freeTimer;

    freeTimer = findFirstFreeSlot();
    if (freeTimer < 0) {
        return -1;
    }

    if (f == NULL) {
        return -1;
    }

    delays[freeTimer] = d;
    callbacks[freeTimer] = f;
    maxNumRuns[freeTimer] = n;
    enabled[freeTimer] = true;
    prev_millis[freeTimer] = elapsed();

    numTimers++;

    return freeTimer;
}

int LSTimer::startTimer() {
    int freeTimer;

    freeTimer = findFirstFreeSlot();
    if (freeTimer < 0) {
        return -1;
    }


    delays[freeTimer] = 0;
    callbacks[freeTimer] = 0;
    maxNumRuns[freeTimer] = 1;
    enabled[freeTimer] = true;
    prev_millis[freeTimer] = elapsed();

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
    diff_millis = current_millis - prev_millis[timerId];
    return diff_millis;
}


int LSTimer::setInterval(long d, timer_callback f) {
    return setTimer(d, f, RUN_FOREVER);
}


int LSTimer::setTimeout(long d, timer_callback f) {
    return setTimer(d, f, RUN_ONCE);
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
    if (callbacks[timerId] != NULL) {
        callbacks[timerId] = 0;
        enabled[timerId] = false;
        toBeCalled[timerId] = DEFCALL_DONTRUN;
        delays[timerId] = 0;
        numRuns[timerId] = 0;

        // update number of timers
        numTimers--;
    }
}

void LSTimer::restartTimer(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    prev_millis[numTimer] = elapsed();
}


boolean LSTimer::isEnabled(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return false;
    }

    return enabled[numTimer];
}


void LSTimer::enable(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    enabled[numTimer] = true;
}


void LSTimer::disable(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    enabled[numTimer] = false;
}


void LSTimer::toggle(int numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    enabled[numTimer] = !enabled[numTimer];
}


int LSTimer::getNumTimers() {
    return numTimers;
}
