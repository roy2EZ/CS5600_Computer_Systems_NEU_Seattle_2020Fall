#include <Logger.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "List.h"
#include "Map.h"
#include "Thread.h"
#include "thread_lock.h"

const char* readyList = NULL;
const char* sleepMap = NULL;
const char* lockHoldMap = NULL;
const char* blockedMap = NULL;

typedef struct {
    int sleepTick;
    int numTicks;
} SleepInfo;

bool compThreadPrior(void* t1, void* t2) {
    return ((Thread*)t1)->priority > ((Thread*)t2)->priority;
}

Thread* createAndSetThreadToRun(const char* name,
                                void* (*func)(void*),
                                void* arg,
                                int pri) {
    Thread* ret = (Thread*)malloc(sizeof(Thread));
    ret->name = (char*)malloc(strlen(name) + 1);
    strcpy(ret->name, name);
    ret->func = func;
    ret->arg = arg;
    ret->priority = pri;
    ret->originalPriority = pri;

    createThread(ret);
    addToList(readyList, (void*)ret);
    sortList(readyList, compThreadPrior);
    return ret;
}

void destroyThread(Thread* thread) {
    free(thread->name);
    free(thread);
}

Thread* nextThreadToRun(int currentTick) {
    char line[1024];
    if (listSize(readyList) == 0)
        return NULL;
    Thread* ret = NULL;
    int threadIndex = 0;
    do {
        sprintf(line, "[nextThreadToRun] trying thread index %d\n",
                threadIndex);
        verboseLog(line);
        ret = ((Thread*)listGet(readyList, threadIndex));
        if (ret->state == TERMINATED) {
            sprintf(line, "[nextThreadToRun] thread %d was terminated\n",
                    threadIndex);
            verboseLog(line);
            removeFromList(readyList, ret);
            ret = NULL;
        } else {
            bool inmap = MAP_CONTAINS(Thread*, sleepMap, ret);
            if (inmap) {
                SleepInfo* info =
                    (SleepInfo*)GET_FROM_MAP(Thread*, sleepMap, ret);
                if (info->sleepTick + info->numTicks <= currentTick) {
                    REMOVE_FROM_MAP(Thread*, sleepMap, ret);
                } else {
                    ret = NULL;
                    threadIndex++;
                }
            } else {
                inmap = MAP_CONTAINS(Thread*, blockedMap, ret) if (inmap) {
                    const char* lockId =
                        (const char*)GET_FROM_MAP(Thread*, blockedMap, ret);
                    Thread* holdThread =
                        (Thread*)GET_FROM_MAP(const char*, lockHoldMap, lockId);
                    if (holdThread) {
                        ret = NULL;
                        threadIndex++;
                    }
                }
            }
        }

    } while (threadIndex < listSize(readyList) && ret == NULL);
    return ret;
}

void initializeCallback() {
    readyList = createNewList();
    sleepMap = CREATE_MAP(Thread*);
    lockHoldMap = CREATE_MAP(const char*);
    blockedMap = CREATE_MAP(Thread*);
}

void shutdownCallback() {
    destroyList(readyList);
}

int tickSleep(int numTicks) {
    Thread* currentThread = (Thread*)getCurrentThread();
    SleepInfo* info = (SleepInfo*)malloc(sizeof(SleepInfo));
    info->numTicks = numTicks;
    info->sleepTick = getCurrentTick();
    PUT_IN_MAP(Thread*, sleepMap, currentThread, (void*)info);
    stopExecutingThreadForCycle();
    int sleepTick = info->sleepTick;
    free(info);
    return sleepTick;
}

void setMyPriority(int priority) {
    Thread* currentThread = (Thread*)getCurrentThread();
    currentThread->priority = priority;
    sortList(readyList, compThreadPrior);
}