#include <stdlib.h>
#include "unity.h"
#include "thread.h"
#include "utils.h"
#include "threadedTestUtil.h"
#include "memory.h"

extern int PAGE_SIZE;
extern bool panicExpected;
extern const int USER_BASE_ADDR;
extern const int STACK_END_ADDR;

void testSingleThreadedWriteHeapAndReadFullPage() {
    TestReadWriteInfoList *list = getTestList(1);
    list->readWriteInfoList[0] = createTestReadWriteInfo(PAGE_SIZE, PAGE_SIZE, 0, true);
    pthread_create(&(list->thread->thread), NULL, writeAndReadMem, list);
    destroyThread(list->thread);
    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[0].inputData, list->readWriteInfoList[0].outputData, PAGE_SIZE);
    destroyTestReadWriteInfoList(list);
    free(list);
}

void testSingleThreadedWriteHeapAndReadAcrossTwoPages() {
    TestReadWriteInfoList *list = getTestList(1);
    list->readWriteInfoList[0] = createTestReadWriteInfo(PAGE_SIZE*2, PAGE_SIZE*2, 0, true);
    pthread_create(&(list->thread->thread), NULL, writeAndReadMem, list);
    destroyThread(list->thread);
    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[0].inputData, list->readWriteInfoList[0].outputData, PAGE_SIZE);
    destroyTestReadWriteInfoList(list);
    free(list);
}

void testSingleThreadedWriteHeapAndReadMiddleOfPage() {
    TestReadWriteInfoList *list = getTestList(2);
    list->readWriteInfoList[0] = createTestReadWriteInfo(PAGE_SIZE/2, PAGE_SIZE/2, 0, true);
    list->readWriteInfoList[1] = createTestReadWriteInfo(PAGE_SIZE/4, PAGE_SIZE/4, 0, true);
    pthread_create(&(list->thread->thread), NULL, writeAndReadMem, list);
    destroyThread(list->thread);

    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[0].inputData,
            list->readWriteInfoList[0].outputData, PAGE_SIZE/2);
    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[1].inputData,
                             list->readWriteInfoList[1].outputData, PAGE_SIZE/4);
    destroyTestReadWriteInfoList(list);
    free(list);
}

void testSingleThreadedWriteHeapAndReadPartial() {
    TestReadWriteInfoList *list = getTestList(1);
    int randomOffset = rand()%PAGE_SIZE;
    list->readWriteInfoList[0] = createTestReadWriteInfo(PAGE_SIZE, PAGE_SIZE, randomOffset, true);
    pthread_create(&(list->thread->thread), NULL, writeAndReadMem, list);
    destroyThread(list->thread);
    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[0].inputData+randomOffset,
            list->readWriteInfoList[0].outputData, PAGE_SIZE-randomOffset);
    destroyTestReadWriteInfoList(list);
    free(list);
}

void testSingleThreadedReadAllHeapMem() {
    TestReadWriteInfoList *list = getTestList(1);
    int size = STACK_END_ADDR - USER_BASE_ADDR;
    list->readWriteInfoList[0] = createTestReadWriteInfo(size, size, 0, true);
    pthread_create(&(list->thread->thread), NULL, writeAndReadMem, list);
    destroyThread(list->thread);
    TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfoList[0].inputData,
                             list->readWriteInfoList[0].outputData, size);
    destroyTestReadWriteInfoList(list);
    free(list);
}

void* beyondEndOfPageThreadWrite(void* input) {
    Thread* thread = input;
    int addr = allocateHeapMem(thread, PAGE_SIZE/2);
    void* data = createRandomData(PAGE_SIZE);
    writeToAddr(thread, addr, PAGE_SIZE, data);
    free(data);
    return NULL;
}
void testSingleThreadedWriteHeapBeyondEndOfPage() {
    panicExpected = true;
    Thread* thread = createThread();
    pthread_create(&(thread->thread), NULL, beyondEndOfPageThreadWrite, thread);
    destroyThread(thread);
}

