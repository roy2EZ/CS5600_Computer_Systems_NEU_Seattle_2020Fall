#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "unity.h"
#include "thread.h"
#include "memory.h"

//#define LOGGING_ON

extern const int PAGE_SIZE;
const int MAX_BUFFER_SIZE=4096;
pthread_mutex_t loggerMutex;
const char logBuffer[4096];

bool panicExpected;
void kernelPanic(const Thread* thread, int addr) {
    char* buffer = malloc(8192);
    sprintf(buffer, "Kernel panic at addr %x\n", addr);
    if (thread != NULL) {
        sprintf(buffer,"Thread id %d\n", thread->threadId);
    }

    logData(buffer);
    flushLog();
    free(buffer);
    if (panicExpected) {
        TEST_PASS_MESSAGE("Kernel panicked as expected");
    } else {
        TEST_FAIL_MESSAGE("Kernel panic during test");
    }
}

void* createRandomData(int size) {
    void* ret = malloc(size);
    char* data = ret;
    for (int x = 0; x < size; x++) {
        char c = random()%255;
        *data = c;
        data++;
    }
    return ret;
}

int allocateAndWriteHeapData(Thread *thread, void *data, int allocateSize, int writeSize) {
    int addr = allocateHeapMem(thread, allocateSize);
    if (addr != -1) writeToAddr(thread, addr, writeSize, data);
    return addr;
}

int allocateAndWriteStackData(Thread *thread, void *data, int allocateSize, int writeSize) {
    int addr = allocateStackMem(thread, allocateSize);
    if (addr != -1) writeToAddr(thread, addr, writeSize, data);
    return addr;
}

void logData(const char* info) {
#ifdef LOGGING_ON
    pthread_mutex_lock(&loggerMutex);
    if(strlen(logBuffer) + strlen(info) > MAX_BUFFER_SIZE) {
        printf(logBuffer);
        strcpy(logBuffer, info);
    } else {
        strcat(logBuffer, info);
    }
    pthread_mutex_unlock(&loggerMutex);
#endif
}
void flushLog() {
#ifdef LOGGING_ON
    pthread_mutex_lock(&loggerMutex);
    printf(logBuffer);
    bzero(&logBuffer, MAX_BUFFER_SIZE);
    pthread_mutex_unlock(&loggerMutex);
#endif
}