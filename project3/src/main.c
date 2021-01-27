#include <stdbool.h>
#include "tests/utils.h"
#include "tests/heap/nonThreadedTests.h"
#include "tests/heap/singleThreadedTests.h"
#include "tests/heap/multiThreadedTests.h"
#include "tests/stack/nonThreadedTests.h"
#include "tests/stack/singleThreadedTests.h"
#include "tests/stack/multiThreadedTests.h"
#include "pagingTests.h"
#include "unity.h"
#include "system.h"

define RUN_NON_THREADED_HEAP_TESTS
define RUN_NON_THREADED_STACK_TESTS
define RUN_SINGLE_THREADED_HEAP_TESTS
define RUN_SINGLE_THREADED_STACK_TESTS
define RUN_MULTI_THREADED_HEAP_TESTS
define RUN_MULTI_THREADED_STACK_TESTS
define RUN_PAGING_TESTS
define EXTRA_LONG_RUNNING_TESTS


extern bool panicExpected;

void setUp() {
    systemInit();
    panicExpected = false;
}

void tearDown() {
    systemShutdown();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testWriteIntoKernelFails);
    #ifdef RUN_NON_THREADED_HEAP_TESTS
    RUN_TEST(testReadHeapFullPage);
    RUN_TEST(testReadHeapAcrossTwoPages);
    RUN_TEST(testReadHeapMiddleOfPage);
    RUN_TEST(testReadHeapPartialPage);
    RUN_TEST(testReadAllHeapMem);
    #endif
    #ifdef RUN_NON_THREADED_STACK_TESTS
    RUN_TEST(testReadStackFullPage);
    RUN_TEST(testReadStackAcrossTwoPages);
    RUN_TEST(testReadStackMiddleOfPage);
    RUN_TEST(testReadStackPartialPage);
    RUN_TEST(testReadAllStackMem);
    #endif
    #ifdef RUN_SINGLE_THREADED_HEAP_TESTS
    RUN_TEST(testSingleThreadedWriteHeapAndReadFullPage);
    RUN_TEST(testSingleThreadedWriteHeapAndReadAcrossTwoPages);
    RUN_TEST(testSingleThreadedWriteHeapAndReadMiddleOfPage);
    RUN_TEST(testSingleThreadedWriteHeapAndReadPartial);
    RUN_TEST(testSingleThreadedWriteHeapBeyondEndOfPage);
    RUN_TEST(testSingleThreadedReadAllHeapMem);

    #endif
    #ifdef RUN_SINGLE_THREADED_STACK_TESTS
    RUN_TEST(testSingleThreadedWriteStackAndReadFullPage);
    RUN_TEST(testSingleThreadedWriteStackAndReadAcrossTwoPages);
    RUN_TEST(testSingleThreadedWriteStackAndReadMiddleOfPage);
    RUN_TEST(testSingleThreadedWriteStackAndReadPartial);
    RUN_TEST(testSingleThreadedWriteStackBeyondEndOfPage);
    RUN_TEST(testSingleThreadedWriteStackAndReadAllStack);
    #endif
    #ifdef RUN_MULTI_THREADED_HEAP_TESTS
    RUN_TEST(testMultiThreadedWriteHeapAndReadFullPage);
    RUN_TEST(testMultiThreadedWriteHeapAndReadAcrossTwoPages);
    RUN_TEST(testMultiThreadedWriteHeapAndReadMiddleOfPage);
    RUN_TEST(testMultiThreadedWriteHeapAndReadPartial);
    #endif
    #ifdef RUN_MULTI_THREADED_STACK_TESTS
    RUN_TEST(testMultiThreadedWriteStackAndReadFullPage);
    RUN_TEST(testMultiThreadedWriteStackAndReadAcrossTwoPages);
    RUN_TEST(testMultiThreadedWriteStackAndReadMiddleOfPage);
    RUN_TEST(testMultiThreadedWriteStackAndReadPartial);
    #endif
    #ifdef RUN_PAGING_TESTS
    RUN_TEST(testDataPagedInCorrectly);
    RUN_TEST(testDataPagedOutCorrectly);
    #endif
    #ifdef EXTRA_LONG_RUNNING_TESTS
    RUN_TEST(testMultiThreadedReadAllHeapMemory);
    RUN_TEST(testMultiThreadedReadAllStackMemory);
    #endif

    return UNITY_END();
}
