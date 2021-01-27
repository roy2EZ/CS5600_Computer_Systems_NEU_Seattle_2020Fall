#ifndef _INCLUDED_FROM_THREAD_H
#error This should never be included directly; #include Thread.h instead!
#endif

/**
 * The heart of the simulator, this function is called every tick and the thread
 * returned is what runs for that tick. The default implementation is a simple
 * queue that works as first in first out.  You will require significant changes
 * to support sleep and priority.
 *
 * @param currentTick The current tick that the returned thread will run in.
 * @return The thread to run for the current tick.
 */
Thread* nextThreadToRun(int currentTick);

/**
 * This function should prepare a thread to run which at a minimum means
 * building the thread object, calling createThread and adding to some sort of
 * ready list.  This is exactly what the default implementation does.
 *
 * @param name The name of a thread.
 * @param func The function to run when starting the thread, this function
 * returns a void* and takes a void*.  This is required because pthread takes
 * this as its argument to start a thread.
 * @param arg The argument to pass to the function that starts the thread.
 * @param pri The priority of a thread, minimum priority is 1, max is 10,
 * default is 5
 * @return The thread that was set to run.
 */
Thread* createAndSetThreadToRun(const char* name,
                                void* (*func)(void*),
                                void* arg,
                                int pri);

/**
 * Tells the current thread to sleep for the number of ticks in the argument.
 * This should sleep for exactly that number of tick given. This function must
 * return the tick that the sleep was supposed to start as no guarantee can be
 * made on the exact tick that the sleep started.
 *
 * @param numTicks The number of ticks to sleep
 * @return The tick that the sleep is supposed to start.
 */
int tickSleep(int numTicks);

/**
 * Free the memory association with the thread object. Attach an image of thread
 * being destroyed for a small present.
 *
 * @param thread The thread to free memory from.
 */
void destroyThread(Thread* thread);

/**
 * Allow a thread to set its priority to whatever it wants, this can be useful
 * during priority donation and may also have to adjust the ready list so this
 * thread gets run in the current priority on the next tick.
 *
 * @param newPriority
 */
void setMyPriority(int newPriority);

/**
 * This function is called after the simulator is started but before the idle
 * thread starts running to allow you to initialize any objects you need to
 * initialize.
 */
void initializeCallback();

/**
 * This function is called before shutdown of the simulator to do any clean up
 * actions you need to do.
 */
void shutdownCallback();