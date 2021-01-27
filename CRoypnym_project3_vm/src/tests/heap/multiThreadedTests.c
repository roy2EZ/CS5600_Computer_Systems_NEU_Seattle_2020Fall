#include <stdlib.h>
#include "threadedTestUtil.h"
#include "unity.h"
#include "utils.h"

extern const int PAGE_SIZE;
extern const int USER_BASE_ADDR;
extern const int STACK_END_ADDR;

void testMultiThreadedWriteHeapAndReadFullPage() {
    int numThreads = rand()%20+3;
    int numItems = rand()%20+3;
    MultipleThreadReadWriteInfoList* list = createMultipleThreads(numThreads,numItems,PAGE_SIZE, PAGE_SIZE, 0, true);
    for (int x = 0; x < list->numItems; x ++) {
        pthread_create(&(list->readWriteInfo[x].thread->thread), NULL, writeAndReadMem, &(list->readWriteInfo[x]));
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyThread(list->readWriteInfo[x].thread);
    }
    for (int x = 0; x < list->numItems; x ++) {
        for (int y = 0; y < list->readWriteInfo[x].numItems; y++) {
            TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfo[x].readWriteInfoList[y].inputData, list->readWriteInfo[x].readWriteInfoList[y].outputData, PAGE_SIZE);
        }
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyTestReadWriteInfoList(&(list->readWriteInfo[x]));
    }

    free(list);
}

void testMultiThreadedWriteHeapAndReadAcrossTwoPages() {
    int numThreads = rand()%20+3;
    int numItems = rand()%20+3;
    MultipleThreadReadWriteInfoList* list = createMultipleThreads(numThreads,numItems,PAGE_SIZE*2, PAGE_SIZE*2, 0, true);
    for (int x = 0; x < list->numItems; x ++) {
        pthread_create(&(list->readWriteInfo[x].thread->thread), NULL, writeAndReadMem, &(list->readWriteInfo[x]));
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyThread(list->readWriteInfo[x].thread);
    }
    for (int x = 0; x < list->numItems; x ++) {
        for (int y = 0; y < list->readWriteInfo[x].numItems; y++) {
            TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfo[x].readWriteInfoList[y].inputData, list->readWriteInfo[x].readWriteInfoList[y].outputData, PAGE_SIZE*2);
        }
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyTestReadWriteInfoList(&(list->readWriteInfo[x]));
    }

    free(list);
}

void testMultiThreadedWriteHeapAndReadMiddleOfPage() {
    int numThreads = rand()%20+3;
    int numItems = rand()%20+3;
    MultipleThreadReadWriteInfoList* list = createMultipleThreads(numThreads,numItems,PAGE_SIZE/2, PAGE_SIZE/2, 0, true);
    for (int x = 0; x < list->numItems; x ++) {
        list->readWriteInfo[x].readWriteInfoList[0].allocateSize = PAGE_SIZE/4;
        list->readWriteInfo[x].readWriteInfoList[0].writeSize = PAGE_SIZE/4;
    }
    for (int x = 0; x < list->numItems; x ++) {
        pthread_create(&(list->readWriteInfo[x].thread->thread), NULL, writeAndReadMem, &(list->readWriteInfo[x]));
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyThread(list->readWriteInfo[x].thread);
    }
    for (int x = 0; x < list->numItems; x ++) {
        for (int y = 0; y < list->readWriteInfo[x].numItems; y++) {
            if (y == 0) TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfo[x].readWriteInfoList[y].inputData, list->readWriteInfo[x].readWriteInfoList[y].outputData, PAGE_SIZE/4);
            else TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfo[x].readWriteInfoList[y].inputData, list->readWriteInfo[x].readWriteInfoList[y].outputData, PAGE_SIZE/2);

        }
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyTestReadWriteInfoList(&(list->readWriteInfo[x]));
    }

    free(list);
}

void testMultiThreadedWriteHeapAndReadPartial() {
    int numThreads = rand()%20+3;
    int numItems = rand()%20+3;
    int randomOffset = rand()%PAGE_SIZE;
    MultipleThreadReadWriteInfoList* list = createMultipleThreads(numThreads,numItems,PAGE_SIZE, PAGE_SIZE, randomOffset, true);
    for (int x = 0; x < list->numItems; x ++) {
        pthread_create(&(list->readWriteInfo[x].thread->thread), NULL, writeAndReadMem, &(list->readWriteInfo[x]));
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyThread(list->readWriteInfo[x].thread);
    }
    for (int x = 0; x < list->numItems; x ++) {
        for (int y = 0; y < list->readWriteInfo[x].numItems; y++) {
            TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfo[x].readWriteInfoList[y].inputData+randomOffset, list->readWriteInfo[x].readWriteInfoList[y].outputData, PAGE_SIZE-randomOffset);
        }
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyTestReadWriteInfoList(&(list->readWriteInfo[x]));
    }
    free(list);
}

void testMultiThreadedReadAllHeapMemory() {
    int numThreads = rand()%3+3;
    int size = STACK_END_ADDR - USER_BASE_ADDR;
    MultipleThreadReadWriteInfoList* list = createMultipleThreads(numThreads,1,size, size, 0, true);
    for (int x = 0; x < list->numItems; x ++) {
        pthread_create(&(list->readWriteInfo[x].thread->thread), NULL, writeAndReadMem, &(list->readWriteInfo[x]));
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyThread(list->readWriteInfo[x].thread);
    }
    for (int x = 0; x < list->numItems; x ++) {
        for (int y = 0; y < list->readWriteInfo[x].numItems; y++) {
            TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfo[x].readWriteInfoList[y].inputData, list->readWriteInfo[x].readWriteInfoList[y].outputData, size);
        }
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyTestReadWriteInfoList(&(list->readWriteInfo[x]));
    }

    free(list);
}