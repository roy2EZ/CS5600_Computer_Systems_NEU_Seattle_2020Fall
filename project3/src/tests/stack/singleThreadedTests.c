#include <stdlib.h>
#include "unity.h"
#include "thread.h"
#include "utils.h"
#include "threadedTestUtil.h"
#include "memory.h"

extern int PAGE_SIZE;
extern bool panicExpected;
extern const int STACK_END_ADDR;
extern const int ALL_MEM_SIZE;

void testSingleThreadedWriteStackAndReadFullPage() {
    TestReadWriteInfoList *list = getTestList(1);
    list->readWriteInfoList[0] = createTestReadWriteInfo(PAGE_SIZE, PAGE_SIZE, 0, true);
    pthread_create(&(list->thread->thread), NULL, writeAndReadMem, list);
    destroyThread(list->thread);
    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[0].inputData, list->readWriteInfoList[0].outputData, PAGE_SIZE);
    destroyTestReadWriteInfoList(list);
}

void testSingleThreadedWriteStackAndReadAcrossTwoPages() {
    TestReadWriteInfoList *list = getTestList(1);
    list->readWriteInfoList[0] = createTestReadWriteInfo(PAGE_SIZE*2, PAGE_SIZE*2, 0, true);
    pthread_create(&(list->thread->thread), NULL, writeAndReadMem, list);
    destroyThread(list->thread);
    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[0].inputData, list->readWriteInfoList[0].outputData, PAGE_SIZE);
    destroyTestReadWriteInfoList(list);
}

void testSingleThreadedWriteStackAndReadMiddleOfPage() {
    TestReadWriteInfoList *list = getTestList(2);
    list->readWriteInfoList[0] = createTestReadWriteInfo(PAGE_SIZE/2, PAGE_SIZE/2, 0, false);
    list->readWriteInfoList[1] = createTestReadWriteInfo(PAGE_SIZE/4, PAGE_SIZE/4, 0, false);
    pthread_create(&(list->thread->thread), NULL, writeAndReadMem, list);
    destroyThread(list->thread);

    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[0].inputData,
            list->readWriteInfoList[0].outputData, PAGE_SIZE/2);
    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[1].inputData,
                             list->readWriteInfoList[1].outputData, PAGE_SIZE/4);
    destroyTestReadWriteInfoList(list);
}

void testSingleThreadedWriteStackAndReadPartial() {
    TestReadWriteInfoList *list = getTestList(1);
    int randomOffset = rand()%PAGE_SIZE;
    list->readWriteInfoList[0] = createTestReadWriteInfo(PAGE_SIZE, PAGE_SIZE, randomOffset, false);
    pthread_create(&(list->thread->thread), NULL, writeAndReadMem, list);
    destroyThread(list->thread);
    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[0].inputData+randomOffset,
            list->readWriteInfoList[0].outputData, PAGE_SIZE-randomOffset);
    destroyTestReadWriteInfoList(list);
}

void testSingleThreadedWriteStackAndReadAllStack() {
    TestReadWriteInfoList *list = getTestList(1);
    int size = ALL_MEM_SIZE-STACK_END_ADDR-1;
    list->readWriteInfoList[0] = createTestReadWriteInfo(size, size,0, true);
    pthread_create(&(list->thread->thread), NULL, writeAndReadMem, list);
    destroyThread(list->thread);
    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[0].inputData, list->readWriteInfoList[0].outputData, PAGE_SIZE);
    destroyTestReadWriteInfoList(list);
}

void* beyondEndOfPageThreadStackWrite(void* input) {
    Thread* thread = input;
    int addr = allocateStackMem(thread, PAGE_SIZE/2);
    void* data = createRandomData(PAGE_SIZE);
    writeToAddr(thread, addr, PAGE_SIZE, data);
    free(data);
    return NULL;
}
void testSingleThreadedWriteStackBeyondEndOfPage() {
    panicExpected = true;
    Thread* thread = createThread();
    pthread_create(&(thread->thread), NULL, beyondEndOfPageThreadStackWrite, thread);
    destroyThread(thread);
}

