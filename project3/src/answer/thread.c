#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "thread.h"
#include "utils.h"

int currentThreadId = 1;

extern const int NUM_PAGES;
extern const int USER_BASE_ADDR;
extern const int STACK_END_ADDR;
extern const int ALL_MEM_SIZE;
extern const int PAGE_SIZE;

extern void freePageFrames(Thread *);
extern void removeCacheFile(Thread *);

Thread* createThread() {
    Thread* ret = malloc(sizeof(Thread));
    bzero(ret, sizeof(Thread));
    

    ret->threadId = currentThreadId;
    currentThreadId++;

    ret->PFN = malloc(NUM_PAGES * sizeof(int));
    for (int i = 0; i < NUM_PAGES; i++)
        ret->PFN[i] = -1;
    ret->heap_ptr = USER_BASE_ADDR;
    ret->stack_ptr = ALL_MEM_SIZE;
    return ret;
}

void destroyThread(Thread* thread) {
    // This is line is ABSOLUTELY REQUIRED for the tests to run properly. This allows the thread to finish its work
    // DO NOT REMOVE.
    if (thread->thread) pthread_join(thread->thread, NULL);
    freePageFrames(thread);
    free(thread->PFN);
    removeCacheFile(thread);
    free(thread);
}
