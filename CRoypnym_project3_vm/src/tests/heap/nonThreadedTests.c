#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "unity.h"
#include "thread.h"
#include "memory.h"
#include "utils.h"

extern const int ALL_MEM_SIZE;
extern const int NUM_PAGES;
extern const int PAGE_SIZE;
extern const int USER_BASE_ADDR;
extern const int STACK_END_ADDR;
extern bool panicExpected;

void testWriteHeapBeyondEndOfPage() {
    panicExpected = true;
    Thread* thread = createThread();
    int addr = allocateHeapMem(thread, PAGE_SIZE/2);
    void* data = createRandomData(PAGE_SIZE);
    writeToAddr(thread, addr, PAGE_SIZE, data);
    destroyThread(thread);
}

void testReadHeapFullPage() {
    Thread *thread = createThread();
    void *data = createRandomData(PAGE_SIZE);
    int addr = allocateAndWriteHeapData(thread, data, PAGE_SIZE, PAGE_SIZE);
    void *readData = malloc(PAGE_SIZE);
    bzero(readData, PAGE_SIZE);
    readFromAddr(thread, addr, PAGE_SIZE, readData);

    TEST_ASSERT_EQUAL_MEMORY(data, readData, PAGE_SIZE);
    free(data);
    free(readData);
    destroyThread(thread);
}

void testReadHeapAcrossTwoPages() {
    Thread* thread = createThread();
    void* data = createRandomData(PAGE_SIZE*2);
    int addr = allocateAndWriteHeapData(thread, data, PAGE_SIZE * 2, PAGE_SIZE * 2);

    void *readData = malloc(PAGE_SIZE*2);
    bzero(readData, PAGE_SIZE*2);
    readFromAddr(thread, addr, PAGE_SIZE*2, readData);

    TEST_ASSERT_EQUAL_MEMORY(data, readData, PAGE_SIZE*2);
    free(data);
    free(readData);
    destroyThread(thread);
}

void testReadHeapMiddleOfPage() {
    Thread* thread = createThread();
    void* data = createRandomData(PAGE_SIZE/2);
    void* secondData = createRandomData(PAGE_SIZE/4);
    int addr = allocateAndWriteHeapData(thread, data, PAGE_SIZE / 2, PAGE_SIZE / 2);
    int secondAddr = allocateAndWriteHeapData(thread, secondData, PAGE_SIZE / 4, PAGE_SIZE / 4);

    void *readData = malloc(PAGE_SIZE/2);
    bzero(readData, PAGE_SIZE/2);
    readFromAddr(thread, addr, PAGE_SIZE/2, readData);
    TEST_ASSERT_EQUAL_MEMORY(data, readData, PAGE_SIZE/2);

    bzero(readData, PAGE_SIZE/2);
    readFromAddr(thread, secondAddr, PAGE_SIZE/4, readData);
    TEST_ASSERT_EQUAL_MEMORY(secondData, readData, PAGE_SIZE/4);

    free(data);
    free(secondData);
    free(readData);
    destroyThread(thread);
}

void testReadHeapPartialPage() {
    Thread* thread = createThread();
    void* data = createRandomData(PAGE_SIZE*3/4);
    int addr = allocateAndWriteHeapData(thread, data, PAGE_SIZE, PAGE_SIZE / 2);
    writeToAddr(thread, addr+(PAGE_SIZE/2), (PAGE_SIZE/4), data+(PAGE_SIZE/2));

    void *readData = malloc(PAGE_SIZE*3/4);
    bzero(readData, PAGE_SIZE*3/4);
    readFromAddr(thread, addr, PAGE_SIZE*3/4, readData);
    TEST_ASSERT_EQUAL_MEMORY(data, readData, PAGE_SIZE*3/4);

    bzero(readData, PAGE_SIZE*3/4);
    readFromAddr(thread, addr, PAGE_SIZE/2, readData);
    TEST_ASSERT_EQUAL_MEMORY(data, readData, PAGE_SIZE/2);

    bzero(readData, PAGE_SIZE*3/4);
    readFromAddr(thread, addr+(PAGE_SIZE/2), PAGE_SIZE/4, readData);
    TEST_ASSERT_EQUAL_MEMORY(data+(PAGE_SIZE/2), readData, PAGE_SIZE/4);

    free(data);
    free(readData);
    destroyThread(thread);
}

void testReadAllHeapMem() {
    Thread *thread = createThread();
    int size = STACK_END_ADDR - USER_BASE_ADDR;
    void *data = createRandomData(size);
    int addr = allocateAndWriteHeapData(thread, data, size, size);
    void *readData = malloc(size);
    bzero(readData, size);
    readFromAddr(thread, addr, size, readData);

    TEST_ASSERT_EQUAL_MEMORY(data, readData, size);
    free(data);
    free(readData);
    destroyThread(thread);
}