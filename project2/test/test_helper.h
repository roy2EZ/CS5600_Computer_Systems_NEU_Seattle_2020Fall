#ifndef PROJECT2_THREADING_TESTHELPER_H
#define PROJECT2_THREADING_TESTHELPER_H

#include <pthread.h>
#include "Thread.h"

typedef struct ThreadCallbackInfo {
    const char* threadName;
    void* (*func)(void*);
    void* arg;
    int pri;
} ThreadCallbackInfo;

typedef struct Multiply {
    int val;
    int multiplier;
    int answer;
} Multiply;

typedef struct PriorityInfo {
    char* threadName;
    int priority;
} PriorityInfo;

typedef struct PriorityInfoList {
    PriorityInfo* list;
    int size;
    ThreadCallbackInfo** tcbiList;
    bool* cont;
    int* numThreadsStarted;
    pthread_mutex_t varMemLock;
} PriorityInfoList;

typedef struct SleepInfo {
    int tickSleepStarted;
    int ticksToSleep;
    int tickWokenUp;
} SleepInfo;

typedef struct ThreadLockInfo {
    Thread thread;
    bool lockHeld;
} ThreadLockInfo;

typedef struct DonationInfo {
    ThreadLockInfo* threadOrder;
    int* numThreadsFinished;
    const char* lock;
    pthread_mutex_t* varMemLock;
    bool* cont;
    ThreadCallbackInfo* tcbi;
} DonationInfo;

void* multiply(void* arg);
void* recordThreadPriority(void* arg);
void* sleepTest(void* arg);
void* simpleLock(void* arg);
void* donationPriority(void* arg);
void* setMyPriorityTest(void* arg);
#endif  // PROJECT2_THREADING_TESTHELPER_H
