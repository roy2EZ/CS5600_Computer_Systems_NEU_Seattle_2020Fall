#ifndef OS_THREADING_THREAD_H
#define OS_THREADING_THREAD_H

#include <pthread.h>
#include <cstdlib>
#include "Thread.h"
#include "ThreadingConstants.h"

using namespace std;
namespace Threading {
class InternalThread {
   public:
    InternalThread(void* (*func)(void*), void* arg);
    InternalThread(void* (*func)(void*), void* arg, Thread* externalThread);
    ~InternalThread();
    void terminated();
    void pause();
    void resume();
    void start();
    void exit();
    State getState();
    int joinWithTimeout();
    int join();
    Thread* getExternalThread();
    void runningSigFunc(int sig);
    void stopExecution();

   private:
    pthread_t thread;
    pthread_mutex_t sleepMutex;
    pthread_mutex_t stateMutex;
    pthread_mutex_t signalMutex;
    pthread_mutex_t stopExecutionMutex;
    pthread_cond_t stopExecutionCond;

    State currentState;
    void* (*func)(void*);
    void sendSignal(int sig, State waitForState);

    void setState(State newState);
    static void* startThread(void* thread);
    void* arg;
    Thread* externalThread;
};
}  // namespace Threading

#endif  // OS_THREADING_THREAD_H
