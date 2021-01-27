#include "InternalThread.h"
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "io/InternalLogger.h"

using namespace Threading;
using namespace std;

InternalThread::InternalThread(void* (*func)(void*), void* arg) {
    signal(SIGUSR1, ThreadManager::signalFunc);
    signal(SIGUSR2, ThreadManager::signalFunc);
    this->func = func;
    this->arg = arg;
    externalThread = NULL;
    pthread_mutex_init(&sleepMutex, NULL);
    pthread_mutex_init(&stateMutex, NULL);
    pthread_mutex_init(&signalMutex, NULL);
    pthread_mutex_init(&stopExecutionMutex, NULL);
    pthread_cond_init(&stopExecutionCond, NULL);
    currentState = CREATED;
}

InternalThread::InternalThread(void* (*func)(void*),
                               void* arg,
                               Thread* externalThread)
    : InternalThread(func, arg) {
    this->externalThread = externalThread;
    this->externalThread->state = CREATED;
}

InternalThread::~InternalThread() {}

State InternalThread::getState() {
    pthread_mutex_lock(&stateMutex);
    State ret = currentState;
    pthread_mutex_unlock(&stateMutex);
    return ret;
}

void InternalThread::setState(State newState) {
    pthread_mutex_lock(&stateMutex);
    currentState = newState;
    if (externalThread != NULL)
        externalThread->state = newState;
    pthread_mutex_unlock(&stateMutex);
}

void InternalThread::runningSigFunc(int sig) {
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink()
            << "[InternalThread] "
            << "Got signal " << strsignal(sig) << " on thread "
            << externalThread->name << "\n";
        InternalLogger::getLogger().flush();
    }
    if (sig == SIGUSR1) {
        if (InternalLogger::getLogger().isVerbose()) {
            InternalLogger::eventSink()
                << "[InternalThread] " << strsignal(sig)
                << " so pausing thread " << externalThread->name << "\n";
            InternalLogger::getLogger().flush();
        }
        setState(PAUSED);
        sigset_t sigSet;
        sigemptyset(&sigSet);
        sigaddset(&sigSet, SIGUSR2);

        bool cont = true;
        while (cont) {
            int sig;
            int ret = sigwait(&sigSet, &sig);
            if (ret == 0 && sig == SIGUSR2) {
                setState(RUNNING);
                pthread_mutex_lock(&stopExecutionMutex);
                pthread_cond_signal(&stopExecutionCond);
                pthread_mutex_unlock(&stopExecutionMutex);
                cont = false;
            }
        }
    }
}

void InternalThread::exit() {
    terminated();
    pthread_exit(NULL);
}

int InternalThread::joinWithTimeout() {
    return pthread_timedjoin_np(thread, NULL, &THREAD_JOIN_TIMEOUT);
}

// Yes, good work searching for "point" but there are none in this file.

int InternalThread::join() {
    return pthread_join(thread, NULL);
}

void InternalThread::terminated() {
    setState(TERMINATED);
}

void InternalThread::sendSignal(int sig, State waitForState) {
    pthread_mutex_lock(&ThreadManager::threadSignalMutex);
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink()
            << "[InternalThread] "
            << "Sending signal '" << strsignal(sig) << "' to thread "
            << externalThread->name << "\n";
        InternalLogger::getLogger().flush();
    }
    ThreadManager::threadToSignal = this;
    pthread_kill(thread, sig);
    State state = getState();
    while (state != waitForState) {
        usleep(MICROSECONDS_TICK);
        if (InternalLogger::getLogger().isVerbose()) {
            InternalLogger::eventSink()
                << "[InternalThread] "
                << "Goal state (" << waitForState << ") != current state ("
                << state << ") for thread " << externalThread->name << "\n";
            InternalLogger::getLogger().flush();
        }
        state = getState();
    }
    ThreadManager::threadToSignal = NULL;
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink() << "[InternalThread] "
                                    << "Thread " << externalThread->name
                                    << " in state " << waitForState << "\n";
        InternalLogger::getLogger().flush();
    }
    pthread_mutex_unlock(&ThreadManager::threadSignalMutex);
}

void InternalThread::pause() {
    sendSignal(SIGUSR1, PAUSED);
}

void InternalThread::start() {
    setState(RUNNING);
    pthread_create(&thread, NULL, &InternalThread::startThread, this);
}

void InternalThread::resume() {
    sendSignal(SIGUSR2, RUNNING);
}

void* InternalThread::startThread(void* thread) {
    InternalThread* actualThread = ((InternalThread*)thread);
    return actualThread->func(actualThread->arg);
}

void InternalThread::stopExecution() {
    pthread_mutex_lock(&stopExecutionMutex);
    pthread_cond_wait(&stopExecutionCond, &stopExecutionMutex);
    pthread_mutex_unlock(&stopExecutionMutex);
}

Thread* InternalThread::getExternalThread() {
    return externalThread;
}