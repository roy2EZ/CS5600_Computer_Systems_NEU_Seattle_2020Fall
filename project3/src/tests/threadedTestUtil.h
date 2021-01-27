#ifndef VIRTUALMEMFRAMEWORKC_THREADEDTESTUTIL_H
#define VIRTUALMEMFRAMEWORKC_THREADEDTESTUTIL_H

#include <thread.h>
#include <stdbool.h>

typedef struct TestReadWriteInfo {
    void* inputData;
    int allocateSize;
    int writeSize;
    void* outputData;
    int readOffset;
    bool writeToHeap;
} TestReadWriteInfo;

typedef struct TestReadWriteInfoList {
    TestReadWriteInfo* readWriteInfoList;
    Thread* thread;
    int numItems;
} TestReadWriteInfoList;

typedef struct MultipleThreadReadWriteInfoList {
    TestReadWriteInfoList* readWriteInfo;
    int numItems;
} MultipleThreadReadWriteInfoList;

void* writeAndReadMem(void* testInfo);
TestReadWriteInfo createTestReadWriteInfo(int allocateSize, int writeSize, int readOffset, bool writeToHeap);
MultipleThreadReadWriteInfoList* createMultipleThreads(int numThreads, int numItems, int allocateSize, int writeSize, int readOffset, bool writeToHeap);
TestReadWriteInfoList* getTestList(int numItems);
void destroyTestReadWriteInfoList(TestReadWriteInfoList* testReadWriteInfoList);
#endif