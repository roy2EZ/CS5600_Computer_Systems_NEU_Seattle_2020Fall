#include "Lock.h"
#include <stddef.h>
#include "List.h"
#include "Map.h"
#include "thread_lock.h"

#pragma region Lock Callbacks

void lockCreated(const char* lockId) {
    PUT_IN_MAP(const char*, lockHoldMap, lockId, (void*)NULL);
}

void lockAttempted(const char* lockId, Thread* thread) {
    Thread* holdThread =
        (Thread*)GET_FROM_MAP(const char*, lockHoldMap, lockId);
    if (holdThread && holdThread->priority < thread->priority) {
        holdThread->priority = thread->priority;
        // nest donation
        bool inmap = MAP_CONTAINS(Thread*, blockedMap, holdThread);
        if (inmap) {
            const char* lockId2 =
                (const char*)GET_FROM_MAP(Thread*, blockedMap, holdThread);
            inmap = MAP_CONTAINS(const char*, lockHoldMap, lockId2);
            if (inmap) {
                Thread* holdThread2 =
                    (Thread*)GET_FROM_MAP(const char*, lockHoldMap, lockId2);
                if (holdThread2->priority < holdThread->priority) {
                    holdThread2->priority = holdThread->priority;
                }
            }
        }
        sortList(readyList, compThreadPrior);
    }
    PUT_IN_MAP(Thread*, blockedMap, thread, (void*)lockId);
}

void lockAcquired(const char* lockId, Thread* thread) {
    PUT_IN_MAP(const char*, lockHoldMap, lockId, (void*)thread);
    REMOVE_FROM_MAP(Thread*, blockedMap, thread);
}

void lockFailed(const char* lockId, Thread* thread) {
    bool inmap = MAP_CONTAINS(Thread*, blockedMap, thread);
    if (inmap)
        REMOVE_FROM_MAP(Thread*, blockedMap, thread);
}

void lockReleased(const char* lockId, Thread* thread) {
    PUT_IN_MAP(const char*, lockHoldMap, lockId, (void*)NULL);
    thread->priority = thread->originalPriority;
    sortList(readyList, compThreadPrior);
}

#pragma endregion

#pragma region Lock Functions

Thread* getThreadHoldingLock(const char* lockId) {
    bool inmap = MAP_CONTAINS(const char*, lockHoldMap, lockId);
    if (inmap) {
        Thread* thread =
            (Thread*)GET_FROM_MAP(const char*, lockHoldMap, lockId);
        return thread;
    }
    return NULL;
}

#pragma endregion