#include "ThreadManager.h"
#include <unistd.h>
#include <cerrno>
#include "InternalThread.h"
#include "io/InternalLogger.h"

using namespace std;
using namespace Threading;

ThreadManager* ThreadManager::singleton = NULL;
InternalThread* ThreadManager::threadToSignal = NULL;
pthread_mutex_t ThreadManager::threadSignalMutex;

// This can also be resolved using lambdas, std::bind, or simply relying on
// undefined behavior.
void* ThreadManager::startIdleThread(ThreadManager* threadManager) {
    return threadManager->idleFunc();
}

ThreadManager::ThreadManager() {
    tick = 0;
    idleThread = shared_ptr<InternalThread>(
        new InternalThread((void* (*)(void*)) & startIdleThread, this));
    pthread_mutex_init(&runningThreadMutex, NULL);
    pthread_mutex_init(&shutdownMutex, NULL);
    pthread_mutex_init(&threadMappingMutex, NULL);
    pthread_mutex_init(&threadSignalMutex, NULL);
    runningThread = idleThread;
    InternalLogger::init();
    keepRunning = true;
    lockManager = LockManager::getInstance();
}

ThreadManager::~ThreadManager() {
    pthread_mutex_destroy(&runningThreadMutex);
    pthread_mutex_destroy(&shutdownMutex);
    pthread_mutex_destroy(&threadMappingMutex);
    pthread_mutex_destroy(&threadSignalMutex);
}

void ThreadManager::start() {
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::getLogger() << "Starting system\n";
        InternalLogger::getLogger().flush();
    }
    idleThread->start();
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::getLogger()
            << "System started on tick " << tick << "\n";
        InternalLogger::getLogger().flush();
    }
}

void ThreadManager::setRunningThread(shared_ptr<InternalThread> running) {
    pthread_mutex_lock(&runningThreadMutex);
    runningThread = running;
    pthread_mutex_unlock(&runningThreadMutex);
}

void* ThreadManager::idleFunc() {
    bool cont = true;
    pthread_mutex_lock(&threadMappingMutex);
    while (cont || !areAllThreadsTerminated()) {
        tick++;
        InternalLogger::getLogger().setTick(tick);
        InternalLogger::getLogger().flush();
        if (InternalLogger::getLogger().isVerbose()) {
            InternalLogger::eventSink() << "[ThreadManager] "
                                        << "Tick start"
                                        << "\n";
            InternalLogger::getLogger().flush();
        }
        Thread* newThread = nextThreadToRun(tick);
        if (newThread == NULL) {
            pthread_mutex_unlock(&threadMappingMutex);
            usleep(MICROSECONDS_TICK);
        } else {
            shared_ptr<InternalThread> currentThread = threadMapping[newThread];
            switch (currentThread->getState()) {
                case CREATED: {
                    setRunningThread(currentThread);
                    if (InternalLogger::getLogger().isVerbose()) {
                        InternalLogger::eventSink()
                            << "[ThreadManager] "
                            << "Starting thread " << newThread->name << "\n";
                        InternalLogger::getLogger().flush();
                    }
                    currentThread->start();
                    break;
                }
                case PAUSED: {
                    setRunningThread(currentThread);
                    if (InternalLogger::getLogger().isVerbose()) {
                        InternalLogger::eventSink()
                            << "[ThreadManager] "
                            << "Resuming thread " << newThread->name << "\n";
                        InternalLogger::getLogger().flush();
                    }
                    currentThread->resume();
                    if (InternalLogger::getLogger().isVerbose()) {
                        InternalLogger::eventSink()
                            << "[ThreadManager] "
                            << "Successfully resumed thread " << newThread->name
                            << "\n";
                        InternalLogger::getLogger().flush();
                    }
                    break;
                }
            }
            int status = currentThread->joinWithTimeout();
            if (InternalLogger::getLogger().isVerbose()) {
                InternalLogger::eventSink()
                    << "[ThreadManager] "
                    << "Status of thread " << newThread->name << " is "
                    << status << "\n";
                InternalLogger::getLogger().flush();
            }
            setRunningThread(idleThread);
            if (InternalLogger::getLogger().isVerbose()) {
                InternalLogger::eventSink()
                    << "[ThreadManager] "
                    << "End of cycle for thread " << newThread->name << "\n";
                InternalLogger::getLogger().flush();
            }
            if (status == ETIMEDOUT || status == EBUSY) {
                if (InternalLogger::getLogger().isVerbose()) {
                    InternalLogger::eventSink()
                        << "[ThreadManager] "
                        << "Pausing thread " << newThread->name << "\n";
                    InternalLogger::getLogger().flush();
                }
                currentThread->pause();
                if (InternalLogger::getLogger().isVerbose()) {
                    InternalLogger::eventSink() << "[ThreadManager] "
                                                << "Successfully paused thread "
                                                << newThread->name << "\n";
                    InternalLogger::getLogger().flush();
                }
            } else if (status == 0) {
                if (InternalLogger::getLogger().isVerbose()) {
                    InternalLogger::eventSink()
                        << "[ThreadManager] "
                        << "Terminating thread " << newThread->name << "\n";
                    InternalLogger::getLogger().flush();
                }
                currentThread->terminated();
            }
            pthread_mutex_lock(&shutdownMutex);
            cont = keepRunning;
            pthread_mutex_unlock(&shutdownMutex);
            pthread_mutex_unlock(&threadMappingMutex);
        }
    }
    InternalLogger::getLogger().flush();
}

bool ThreadManager::areAllThreadsTerminated() {
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink()
            << "[ThreadManager] "
            << "Checking if all threads are terminated\n";
        InternalLogger::getLogger().flush();
    }
    for (map<Thread*, shared_ptr<InternalThread>>::iterator iter =
             threadMapping.begin();
         iter != threadMapping.end(); iter++) {
        if (iter->second->getState() != TERMINATED)
            return false;
    }
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink() << "[ThreadManager] "
                                    << "All threads have been terminated\n";
        InternalLogger::getLogger().flush();
    }
    return true;
}

void ThreadManager::shutdown() {
    pthread_mutex_t shutdownMutex = ThreadManager::getInstance()->shutdownMutex;
    pthread_mutex_lock(&shutdownMutex);
    ThreadManager::getInstance()->keepRunning = false;
    pthread_mutex_unlock(&shutdownMutex);
}

ThreadManager* ThreadManager::getInstance() {
    if (!ThreadManager::singleton) {
        ThreadManager::singleton = new ThreadManager();
    }
    return ThreadManager::singleton;
}

shared_ptr<InternalThread> ThreadManager::currentThread() {
    return runningThread;
}

void ThreadManager::sleepCurrentThread() {
    runningThread->stopExecution();
}

void ThreadManager::createThread(Thread* thread) {
    pthread_mutex_lock(&threadMappingMutex);
    threadMapping[thread] = shared_ptr<InternalThread>(
        new InternalThread(thread->func, thread->arg, thread));
    pthread_mutex_unlock(&threadMappingMutex);
}

void ThreadManager::waitForFinish() {
    idleThread->join();
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink() << "[ThreadManager] "
                                    << "Idle thread has terminated\n";
        InternalLogger::getLogger().flush();
    }
}

void ThreadManager::destroyThreadManager() {
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink() << "[ThreadManager] "
                                    << "Destroying thread manager\n";
        InternalLogger::getLogger().flush();
    }
    if (ThreadManager::singleton) {
        delete ThreadManager::singleton;
        ThreadManager::singleton = NULL;
    }
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink() << "[ThreadManager] "
                                    << "Thread manager destroyed\n";
        InternalLogger::getLogger().flush();
    }
}

void ThreadManager::signalFunc(int sig) {
    threadToSignal->runningSigFunc(sig);
}

int ThreadManager::currentTick() {
    return this->tick;
}

Thread* getCurrentThread() {
    return ThreadManager::getInstance()->currentThread()->getExternalThread();
}

void createThread(Thread* thread) {
    ThreadManager::getInstance()->createThread(thread);
}

void startSystem() {
    initializeCallback();
    ThreadManager::getInstance()->start();
}

void stopSystem() {
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink() << "[ThreadManager] "
                                    << "Stopping system\n";
        InternalLogger::getLogger().flush();
    }
    ThreadManager::shutdown();
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink() << "[ThreadManager] "
                                    << "Waiting for threads to finish\n";
        InternalLogger::getLogger().flush();
    }
    ThreadManager::getInstance()->waitForFinish();
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink() << "[ThreadManager] "
                                    << "All threads finished\n";
        InternalLogger::getLogger().flush();
    }
    shutdownCallback();
    ThreadManager::destroyThreadManager();
}

void stopExecutingThreadForCycle() {
    ThreadManager::getInstance()->sleepCurrentThread();
}

int getCurrentTick() {
    return ThreadManager::getInstance()->currentTick();
}