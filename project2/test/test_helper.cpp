#include "test_helper.h"
#include <Lock.h>
#include <Logger.h>
#include <Map.h>
#include <Thread.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

extern const char* lockMap;

void* multiply(void* arg) {
    Multiply* innerArg = (Multiply*)arg;
    innerArg->answer = innerArg->val * innerArg->multiplier;
    return NULL;
}

void* recordThreadPriority(void* arg) {
    char line[1024];
    PriorityInfoList* infoList = (PriorityInfoList*)arg;
    sprintf(line, "[recordThreadPriority] %d:%s:numThreadsStarted = %d\n",
            __LINE__, getCurrentThread()->name, *(infoList->numThreadsStarted));
    verboseLog(line);
    ThreadCallbackInfo* tcbi =
        infoList->tcbiList[*(infoList->numThreadsStarted)];
    sprintf(line, "[recordThreadPriority] %d:%s:numThreadsStarted = %d\n",
            __LINE__, getCurrentThread()->name, *(infoList->numThreadsStarted));
    verboseLog(line);
    if (tcbi) {
        pthread_mutex_lock(&(infoList->varMemLock));
        (*(infoList->numThreadsStarted))++;
        pthread_mutex_unlock(&(infoList->varMemLock));
        createAndSetThreadToRun(tcbi->threadName, tcbi->func, tcbi->arg,
                                tcbi->pri);
    } else {
        pthread_mutex_lock(&(infoList->varMemLock));
        sprintf(line, "[recordThreadPriority] %d:%s:numThreadsStarted = %d\n",
                __LINE__, getCurrentThread()->name,
                *(infoList->numThreadsStarted));
        *(infoList->cont) = true;
        pthread_mutex_unlock(&(infoList->varMemLock));
    }

    sprintf(line, "[recordThreadPriority] %d:%s:numThreadsStarted = %d\n",
            __LINE__, getCurrentThread()->name, *(infoList->numThreadsStarted));
    verboseLog(line);
    pthread_mutex_lock(&(infoList->varMemLock));
    bool cont = *(infoList->cont);
    pthread_mutex_unlock(&(infoList->varMemLock));
    sprintf(line, "[recordThreadPriority] %d:%s:numThreadsStarted = %d\n",
            __LINE__, getCurrentThread()->name, *(infoList->numThreadsStarted));
    verboseLog(line);
    while (!cont) {
        stopExecutingThreadForCycle();
        pthread_mutex_lock(&(infoList->varMemLock));
        cont = *(infoList->cont);
        pthread_mutex_unlock(&(infoList->varMemLock));
    }
    stopExecutingThreadForCycle();
    sprintf(line, "[recordThreadPriority] %d:%s:numThreadsStarted = %d\n",
            __LINE__, getCurrentThread()->name, *(infoList->numThreadsStarted));
    verboseLog(line);
    PriorityInfo* info =
        (PriorityInfo*)malloc(sizeof(PriorityInfo) * (infoList->size + 1));
    Thread* thread = getCurrentThread();
    info[infoList->size].threadName = (char*)malloc(strlen(thread->name) + 1);
    strcpy(info[infoList->size].threadName, thread->name);
    info[infoList->size].priority = thread->priority;
    sprintf(line, "[recordThreadPriority] %d:%s:numThreadsStarted = %d\n",
            __LINE__, getCurrentThread()->name, *(infoList->numThreadsStarted));
    verboseLog(line);
    if (infoList->list) {
        memcpy(info, infoList->list, sizeof(PriorityInfo) * infoList->size);
        free(infoList->list);
    }
    infoList->list = info;
    infoList->size++;
    return NULL;
}

void* sleepTest(void* arg) {
    SleepInfo* sleepInfo = (SleepInfo*)arg;
    int sleepStarted = tickSleep(sleepInfo->ticksToSleep);
    sleepInfo->tickWokenUp = getCurrentTick();
    sleepInfo->tickSleepStarted = sleepStarted;
    return NULL;
}

void* simpleLock(void* arg) {
    const char* lockId = createLock();

    lock(lockId);
    Thread* lockHoldingThread = getThreadHoldingLock(lockId);
    if (lockHoldingThread) {
        memcpy(arg, lockHoldingThread, sizeof(Thread));
    }

    unlock(lockId);
}

void* donationPriority(void* arg) {
    DonationInfo* donationInfo = (DonationInfo*)arg;
    Thread* currentThread = getCurrentThread();
    if (donationInfo->tcbi) {
        createAndSetThreadToRun(
            donationInfo->tcbi->threadName, donationInfo->tcbi->func,
            donationInfo->tcbi->arg, donationInfo->tcbi->pri);
    } else {
        pthread_mutex_lock(donationInfo->varMemLock);
        *(donationInfo->cont) = true;
        pthread_mutex_unlock(donationInfo->varMemLock);
    }

    lock(donationInfo->lock);

    pthread_mutex_lock(donationInfo->varMemLock);
    bool cont = *(donationInfo->cont);
    pthread_mutex_unlock(donationInfo->varMemLock);
    while (!cont) {
        stopExecutingThreadForCycle();
        pthread_mutex_lock(donationInfo->varMemLock);
        cont = *(donationInfo->cont);
        pthread_mutex_unlock(donationInfo->varMemLock);
    }

    pthread_mutex_lock(donationInfo->varMemLock);
    memcpy(&(donationInfo->threadOrder[*(donationInfo->numThreadsFinished)]
                 .thread),
           currentThread, sizeof(Thread));
    Thread* threadHoldingLock = getThreadHoldingLock(donationInfo->lock);
    if (threadHoldingLock) {
        donationInfo->threadOrder[*(donationInfo->numThreadsFinished)]
            .lockHeld =
            (memcmp(currentThread, threadHoldingLock, sizeof(Thread)) == 0);
    }
    *(donationInfo->numThreadsFinished) =
        *(donationInfo->numThreadsFinished) + 1;
    pthread_mutex_unlock(donationInfo->varMemLock);
    unlock(donationInfo->lock);
}

void* setMyPriorityTest(void* arg) {
    int* newPri = (int*)arg;
    setMyPriority(*newPri);
}