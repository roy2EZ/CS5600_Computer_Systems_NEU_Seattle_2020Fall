#include <malloc.h>
#include <stdio.h>
#include <cstring>
#include "Lock.h"
#include "Logger.h"
#include "Map.h"
#include "Thread.h"
#include "gtest/gtest.h"
#include "test_config.h"
#include "test_helper.h"

const char* lockId;
const char* NAME_MULTIPLY = "Multiply";
const char* NAME_LO_PRI = "Lo Pri Thread";
const char* NAME_MD_PRI = "Md Pri Thread";
const char* NAME_HI_PRI = "Hi Pri Thread";

void* sleepTest(void* arg);
bool sortSleepTimers(void* first, void* second);
bool sortThreadsByPriority(void* first, void* second);
void setCurrentReadyList();
void destroyThread(Thread* thread);

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(Running, SingleThread) {
    startSystem();
#ifdef TEST_VERBOSE
    setVerbose(true);
#endif
    Multiply* testMultiply = (Multiply*)malloc(sizeof(Multiply));
    testMultiply->val = 5;
    testMultiply->multiplier = 31;
    testMultiply->answer = 0;
    Thread* thread = createAndSetThreadToRun(NAME_MULTIPLY, multiply,
                                             (void*)testMultiply, DEFAULT_PRI);
    stopSystem();
    destroyThread(thread);
    EXPECT_EQ(155, testMultiply->answer);
    free(testMultiply);
}

TEST(Running, PriorityOrder) {
#ifdef I_HAVE_NOT_IMPLEMENTED_PRIORITY_SCHEDULING
    FAIL() << "To enable this test look at answer/test_config.h\n";
#endif
    startSystem();
#ifdef TEST_VERBOSE
    setVerbose(true);
#endif
    int numThreads = 3;
    PriorityInfoList* infoList =
        (PriorityInfoList*)malloc(sizeof(PriorityInfoList));
    infoList->tcbiList =
        (ThreadCallbackInfo**)malloc(sizeof(ThreadCallbackInfo*) * numThreads);
    infoList->list = NULL;
    infoList->size = 0;
    infoList->cont = (bool*)malloc(sizeof(bool));
    *(infoList->cont) = false;
    infoList->numThreadsStarted = (int*)malloc(sizeof(int));
    *(infoList->numThreadsStarted) = 0;
    pthread_mutex_init(&(infoList->varMemLock), NULL);

    ThreadCallbackInfo midCallback;
    midCallback.threadName = NAME_MD_PRI;
    midCallback.func = recordThreadPriority;
    midCallback.arg = infoList;
    midCallback.pri = DEFAULT_PRI;

    ThreadCallbackInfo hiCallback;
    hiCallback.threadName = NAME_HI_PRI;
    hiCallback.func = recordThreadPriority;
    hiCallback.arg = infoList;
    hiCallback.pri = DEFAULT_PRI + 1;

    infoList->tcbiList[0] = &midCallback;
    infoList->tcbiList[1] = &hiCallback;
    infoList->tcbiList[2] = NULL;

    Thread* lowPriThread = createAndSetThreadToRun(
        NAME_LO_PRI, recordThreadPriority, (void*)infoList, DEFAULT_PRI - 1);
    stopSystem();

    EXPECT_EQ(0, strcmp(NAME_HI_PRI, infoList->list[0].threadName));
    EXPECT_EQ(DEFAULT_PRI + 1, infoList->list[0].priority);
    EXPECT_EQ(0, strcmp(NAME_MD_PRI, infoList->list[1].threadName));
    EXPECT_EQ(DEFAULT_PRI, infoList->list[1].priority);
    EXPECT_EQ(0, strcmp(NAME_LO_PRI, infoList->list[2].threadName));
    EXPECT_EQ(DEFAULT_PRI - 1, infoList->list[2].priority);

    destroyThread(lowPriThread);
    free(infoList->list);
    free(infoList);
}

TEST(Running, SetMyPriority) {
    startSystem();
    int* arg = (int*)malloc(sizeof(int));
    *arg = MAX_PRI;
    Thread* thread = createAndSetThreadToRun("Thread", setMyPriorityTest,
                                             (void*)arg, DEFAULT_PRI);
    stopSystem();
    EXPECT_EQ(MAX_PRI, thread->priority);
    destroyThread(thread);
    free(arg);
}

TEST(Sleep, SingleThread) {
    startSystem();
#ifdef TEST_VERBOSE
    setVerbose(true);
#endif
    SleepInfo* sleepInfo = (SleepInfo*)malloc(sizeof(SleepInfo));
    sleepInfo->ticksToSleep = 10;
    Thread* thread = createAndSetThreadToRun("Sleep", sleepTest,
                                             (void*)sleepInfo, DEFAULT_PRI);
    stopSystem();

    EXPECT_EQ(sleepInfo->ticksToSleep,
              sleepInfo->tickWokenUp - sleepInfo->tickSleepStarted);

    destroyThread(thread);
    free(sleepInfo);
}

TEST(Sleep, MultipleThreads) {
    startSystem();
#ifdef TEST_VERBOSE
    setVerbose(true);
#endif
    SleepInfo* sleepInfo = (SleepInfo*)malloc(sizeof(SleepInfo) * 2);
    sleepInfo[0].ticksToSleep = 10;
    sleepInfo[1].ticksToSleep = 20;
    Thread* thread = createAndSetThreadToRun("Sleep", sleepTest,
                                             (void*)&sleepInfo[0], DEFAULT_PRI);
    Thread* thread2 = createAndSetThreadToRun(
        "Sleep", sleepTest, (void*)&sleepInfo[1], DEFAULT_PRI);
    stopSystem();

    for (int x = 0; x < 2; x++) {
        EXPECT_EQ(sleepInfo[x].ticksToSleep,
                  sleepInfo[x].tickWokenUp - sleepInfo[x].tickSleepStarted);
    }

    destroyThread(thread);
    destroyThread(thread2);
    free(sleepInfo);
}

TEST(Locking, SingleLock) {
    startSystem();
#ifdef TEST_VERBOSE
    setVerbose(true);
#endif
    Thread* threadHoldingLock = (Thread*)malloc(sizeof(Thread));
    bzero((void*)threadHoldingLock, sizeof(Thread));

    Thread* thread = createAndSetThreadToRun(
        "Lock", simpleLock, (void*)threadHoldingLock, DEFAULT_PRI);
    stopSystem();

    if (threadHoldingLock->name)
        EXPECT_EQ(0, strcmp(thread->name, threadHoldingLock->name));
    EXPECT_EQ(thread->priority, threadHoldingLock->priority);
    EXPECT_EQ(thread->originalPriority, threadHoldingLock->originalPriority);
    destroyThread(thread);
    free(threadHoldingLock);
}

TEST(Locking, PriorityDonation) {
#ifdef I_HAVE_NOT_IMPLEMENTED_PRIORITY_DONATION
    FAIL() << "To enable this test look at answer/test_config.h\n";
#endif
    startSystem();
#ifdef TEST_VERBOSE
    setVerbose(true);
#endif
    int numThreads = 3;
    int* count = (int*)malloc(sizeof(int));
    *count = 0;
    int* numThreadsFinished = (int*)malloc(sizeof(int));
    *numThreadsFinished = 0;
    pthread_mutex_t* mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);
    ThreadLockInfo* threadOrder = (ThreadLockInfo*)calloc(
        numThreads, sizeof(ThreadLockInfo) * numThreads);
    const char* lock = createLock();
    bool* cont = (bool*)malloc(sizeof(bool));
    *cont = false;
    DonationInfo* donationInfo =
        (DonationInfo*)malloc(sizeof(DonationInfo) * numThreads);
    for (int x = 0; x < numThreads; x++) {
        donationInfo[x].numThreadsFinished = numThreadsFinished;
        donationInfo[x].varMemLock = mutex;
        donationInfo[x].threadOrder = threadOrder;
        donationInfo[x].lock = lock;
        donationInfo[x].cont = cont;
    }

    ThreadCallbackInfo midCallback;
    midCallback.threadName = NAME_MD_PRI;
    midCallback.func = donationPriority;
    midCallback.arg = &(donationInfo[1]);
    midCallback.pri = DEFAULT_PRI + 1;

    ThreadCallbackInfo hiCallback;
    hiCallback.threadName = NAME_HI_PRI;
    hiCallback.func = donationPriority;
    hiCallback.arg = &(donationInfo[2]);
    hiCallback.pri = DEFAULT_PRI + 2;

    donationInfo[0].tcbi = &midCallback;
    donationInfo[1].tcbi = &hiCallback;
    donationInfo[2].tcbi = NULL;

    Thread* lowPriThread = createAndSetThreadToRun(
        NAME_LO_PRI, donationPriority, (void*)&donationInfo[0], DEFAULT_PRI);

    stopSystem();

    EXPECT_EQ(DEFAULT_PRI + 2, donationInfo->threadOrder[0].thread.priority);
    EXPECT_EQ(0, strcmp(NAME_LO_PRI, donationInfo->threadOrder[0].thread.name));

    for (int x = 0; x < numThreads; x++) {
        EXPECT_TRUE(donationInfo->threadOrder[x].lockHeld);
    }

    destroyThread(lowPriThread);
    free(count);
    free(numThreadsFinished);
    free(threadOrder);
    free(donationInfo);
}
