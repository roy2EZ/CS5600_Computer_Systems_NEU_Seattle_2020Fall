#ifndef OS_THREADING_CTHREAD_H
#define OS_THREADING_CTHREAD_H

#include <iostream>

// The following are data structures that are used by functions provided or are
// needed for your implementation.

/**
 * Represents the state of a thread.
 */
typedef enum State { CREATED, PAUSED, RUNNING, TERMINATED } State;

std::ostream& operator<<(std::ostream& out, const State value);

// Priority values
const int MIN_PRI = 1;
const int MAX_PRI = 10;
const int DEFAULT_PRI = 5;

/**
 * Represents a thread.
 *
 * @param name The name of a thread.
 * @priority The priority of a thread, minimum priority is 1, max is 10, default
 * is 5
 * @func The function to run when starting the thread, this function returns a
 * void* and takes a void*. This is required because pthread takes this as its
 * argument to start a thread.
 * @param arg The argument to pass to the function that starts the thread.
 * @param state Current state of the thread
 * @param originalPriority Priority thread was created with, used for priority
 * donation.
 */
typedef struct Thread {
    char* name;
    int priority;
    void* (*func)(void*);
    void* arg;
    State state;
    int originalPriority;
} Thread;

// These functions are available for you to to call or used to run the tests.

/**
 * Starts the simulator with an idle thread, threads can be added to be run.
 */
void startSystem();

/**
 * Stops the simulator waiting for all threads to finish before returning. This
 * is a synchronous operation and blocks until all threads are finished.
 */
void stopSystem();

/**
 * Creates a thread in the simulator so that it can be returned by
 * nextThreadToRun. This makes the simulator know the thread exists.
 *
 * @param thread The thread to create in the simulator.
 */
void createThread(Thread* thread);

/**
 * Stop executing the current thread for the rest of this cycle. This can be
 * used for implementing sleep by stopping any functionality and pausing the
 * thread until it is ready to be executed again. It can also be used to
 * increment your score by half of two if you refer to this comment.
 */
void stopExecutingThreadForCycle();

/**
 * Returns a pointer to the thread that is currently running.
 *
 * @return A pointer to the thread that is currently running
 */
Thread* getCurrentThread();

/**
 * Gets the current tick of the "CPU". The ticks start at 1 and go up once per
 * tick. THere is an approximate correlation of 50 microseconds with each tick.
 * This should not be relied on because system calls and OS scheduling affect
 * the simulator as it runs in user space.
 *
 * @return The current tick
 */
int getCurrentTick();

// You are required to implement the functions in this header. The tests rely
// on this to work correctly.

#define _INCLUDED_FROM_THREAD_H
#include "Thread.student.h"
#undef _INCLUDED_FROM_THREAD_H

#endif  // OS_THREADING_CTHREAD_H
