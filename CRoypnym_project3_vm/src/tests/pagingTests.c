#include <stdlib.h>
#include "pagingTests.h"
#include "memory.h"
#include "thread.h"
#include "utils.h"
#include "unity.h"

extern const int PAGE_SIZE;
extern const int USER_BASE_ADDR;

void testDataPagedOutCorrectly() {
    Thread* thread1 = createThread();
    void *data = createRandomData(PAGE_SIZE);
    int addr = allocateAndWriteHeapData(thread1, data, PAGE_SIZE, PAGE_SIZE);
    int savedAddr = addr;
    while (addr !=-1) {
        addr = allocateAndWriteHeapData(thread1, data, PAGE_SIZE, PAGE_SIZE);
    }

    addr = allocateAndWriteStackData(thread1, data, PAGE_SIZE, PAGE_SIZE);

    while (addr !=-1) {
        addr = allocateAndWriteStackData(thread1, data, PAGE_SIZE, PAGE_SIZE);
    }


    Thread* thread2 = createThread();
    void* data2 = createRandomData(PAGE_SIZE);
    addr = allocateAndWriteHeapData(thread2, data2, PAGE_SIZE, PAGE_SIZE);

    FILE* file = fopen(getCacheFileName(thread1, savedAddr), "r+");
    void* fileData = malloc(PAGE_SIZE);
    fread(fileData, 1,  PAGE_SIZE, file);
    TEST_ASSERT_EQUAL_MEMORY(data, fileData, PAGE_SIZE);
    fclose(file);
    destroyThread(thread1);
    destroyThread(thread2);
    free(data);
    free(data2);
}

void testDataPagedInCorrectly() {
    Thread* thread1 = createThread();
    void *data = createRandomData(PAGE_SIZE);
    int addr = allocateAndWriteHeapData(thread1, data, PAGE_SIZE, PAGE_SIZE);
    int savedAddr = addr;

    while (addr !=-1) {
        addr = allocateAndWriteHeapData(thread1, data, PAGE_SIZE, PAGE_SIZE);
    }

    addr = allocateAndWriteStackData(thread1, data, PAGE_SIZE, PAGE_SIZE);

    while (addr !=-1) {
        addr = allocateAndWriteStackData(thread1, data, PAGE_SIZE, PAGE_SIZE);
    }

    Thread* thread2 = createThread();
    void* data2 = createRandomData(PAGE_SIZE);
    allocateAndWriteHeapData(thread2, data2, PAGE_SIZE, PAGE_SIZE);

    void* readData = malloc(PAGE_SIZE);
    readFromAddr(thread1, savedAddr, PAGE_SIZE, readData);

    TEST_ASSERT_EQUAL_MEMORY(data, readData, PAGE_SIZE);
    destroyThread(thread1);
    destroyThread(thread2);
    free(data);
    free(data2);
    free(readData);
}