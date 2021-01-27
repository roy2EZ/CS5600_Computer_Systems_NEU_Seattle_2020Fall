#include <stdlib.h>
#include <string.h>
#include "thread.h"
#include "threadedTestUtil.h"
#include "utils.h"
#include "memory.h"

extern int PAGE_SIZE;

void* writeAndReadMem(void* testInfo) {
    TestReadWriteInfoList* testReadWriteInfo = testInfo;
    for (int x = 0; x < testReadWriteInfo->numItems; x++) {
        TestReadWriteInfo info = testReadWriteInfo->readWriteInfoList[x];
        int addr = -1;

        if (info.writeToHeap)
            addr = allocateAndWriteHeapData(testReadWriteInfo->thread,
                                                   info.inputData,
                                                   info.allocateSize,
                                                   info.writeSize);
        else
            addr = allocateAndWriteStackData(testReadWriteInfo->thread,
                                                     info.inputData,
                                                     info.allocateSize,
                                                     info.writeSize);

        if (addr == -1) kernelPanic(testReadWriteInfo->thread, addr);
        bzero(info.outputData, info.writeSize);
        int readAddr = info.writeToHeap ? addr + info.readOffset : addr - info.readOffset;
        readFromAddr(testReadWriteInfo->thread, readAddr,
                info.writeSize-info.readOffset, info.outputData);
    }
    return NULL;
}

TestReadWriteInfo createTestReadWriteInfo(int allocateSize, int writeSize, int readOffset, bool writeToHeap) {
    TestReadWriteInfo testReadWriteInfo;
    testReadWriteInfo.inputData = createRandomData(writeSize);
    testReadWriteInfo.allocateSize = allocateSize;
    testReadWriteInfo.writeSize = writeSize;
    testReadWriteInfo.readOffset = readOffset;
    testReadWriteInfo.writeToHeap = writeToHeap;
    void* outputData = malloc(writeSize);
    bzero(outputData, writeSize);
    testReadWriteInfo.outputData = outputData;
    return testReadWriteInfo;
}

MultipleThreadReadWriteInfoList* createMultipleThreads(int numThreads, int numItems, int allocateSize, int writeSize, int readOffset, bool writeToHeap) {
    MultipleThreadReadWriteInfoList* ret = malloc(sizeof(MultipleThreadReadWriteInfoList));
    ret->numItems = numThreads;
    ret->readWriteInfo = malloc(sizeof(TestReadWriteInfo)*numThreads);
    for (int x = 0; x < numThreads; x++) {
        TestReadWriteInfoList* list = getTestList(numItems);
        for (int y = 0; y < numItems; y++) {
            list->readWriteInfoList[y] = createTestReadWriteInfo(allocateSize, writeSize, readOffset, writeToHeap);
        }
        memcpy(&(ret->readWriteInfo[x]),list, sizeof(TestReadWriteInfoList));
        for (int y = 0; y < numItems; y++) {
        }
        free(list);
    }

    return ret;
}

void destroyTestReadWriteInfo(TestReadWriteInfo* testReadWriteInfo) {
    free(testReadWriteInfo->outputData);
    free(testReadWriteInfo->inputData);
}

void destroyTestReadWriteInfoList(TestReadWriteInfoList* testReadWriteInfoList) {
    for (int x = 0; x < testReadWriteInfoList->numItems; x++)
        destroyTestReadWriteInfo(&testReadWriteInfoList->readWriteInfoList[x]);
    free(testReadWriteInfoList->readWriteInfoList);
}

TestReadWriteInfoList* getTestList(int numItems) {
    TestReadWriteInfoList* list = malloc(sizeof(TestReadWriteInfoList));
    list->readWriteInfoList = malloc(sizeof(TestReadWriteInfo)*numItems);
    list->numItems = numItems;
    list->thread = createThread();
    return list;
}