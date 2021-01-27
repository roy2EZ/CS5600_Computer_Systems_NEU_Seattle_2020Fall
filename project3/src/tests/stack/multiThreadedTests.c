#include <stdlib.h>
#include <string.h>
#include "threadedTestUtil.h"
#include "unity.h"
#include "utils.h"

extern const int PAGE_SIZE;
extern const int STACK_END_ADDR;
extern const int ALL_MEM_SIZE;

void testMultiThreadedWriteStackAndReadFullPage() {
    int numThreads = rand()%20+3;
    int numItems = rand()%20+3;
    MultipleThreadReadWriteInfoList* list = createMultipleThreads(numThreads,numItems,PAGE_SIZE, PAGE_SIZE, 0, false);
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

void testMultiThreadedWriteStackAndReadAcrossTwoPages() {
    int numThreads = rand()%20+3;
    int numItems = rand()%20+3;
    MultipleThreadReadWriteInfoList* list = createMultipleThreads(numThreads,numItems,PAGE_SIZE*2, PAGE_SIZE*2, 0, false);
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

void testMultiThreadedWriteStackAndReadMiddleOfPage() {
    int numThreads = rand()%20+3;
    int numItems = rand()%20+3;
    MultipleThreadReadWriteInfoList* list = createMultipleThreads(numThreads,numItems ,PAGE_SIZE/2, PAGE_SIZE/2, 0, false);
    for (int x = 0; x < list->numItems; x++) {
        list->readWriteInfo[x].readWriteInfoList[0].allocateSize = PAGE_SIZE/4;
        list->readWriteInfo[x].readWriteInfoList[0].writeSize = PAGE_SIZE/4;
        free(list->readWriteInfo[x].readWriteInfoList[0].outputData);
        list->readWriteInfo[x].readWriteInfoList[0].outputData = malloc(PAGE_SIZE/4);
        bzero (list->readWriteInfo[x].readWriteInfoList[0].outputData, PAGE_SIZE/4);
        void* temp = list->readWriteInfo[x].readWriteInfoList[0].inputData;
        list->readWriteInfo[x].readWriteInfoList[0].inputData =malloc(PAGE_SIZE/4);
        memcpy(list->readWriteInfo[x].readWriteInfoList[0].inputData, temp, PAGE_SIZE/4);
        free(temp);
    }
    for (int x = 0; x < list->numItems; x ++) {
        pthread_create(&(list->readWriteInfo[x].thread->thread), NULL, writeAndReadMem, &(list->readWriteInfo[x]));
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyThread(list->readWriteInfo[x].thread);
    }
    for (int x = 0; x < list->numItems; x ++) {
        for (int y = 0; y < list->readWriteInfo[x].numItems; y++)
        {
            TEST_ASSERT_EQUAL_MEMORY(list->readWriteInfo[x].readWriteInfoList[y].inputData, list->readWriteInfo[x].readWriteInfoList[y].outputData,  list->readWriteInfo[x].readWriteInfoList[y].writeSize);
        }
    }
    for (int x = 0; x < list->numItems; x ++) {
        destroyTestReadWriteInfoList(&(list->readWriteInfo[x]));
    }

    free(list);
}

void testMultiThreadedWriteStackAndReadPartial() {
    int numThreads = rand()%20+3;
    int numItems = rand()%20+3;
    int randomOffset = rand()%PAGE_SIZE;
    MultipleThreadReadWriteInfoList* list = createMultipleThreads(numThreads,numItems,PAGE_SIZE, PAGE_SIZE, randomOffset, false);
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

void testMultiThreadedReadAllStackMemory() {
    int numThreads = rand()%3+3;
    int size = ALL_MEM_SIZE-STACK_END_ADDR-1;
    MultipleThreadReadWriteInfoList* list = createMultipleThreads(numThreads,1,size, size, 0, false);
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