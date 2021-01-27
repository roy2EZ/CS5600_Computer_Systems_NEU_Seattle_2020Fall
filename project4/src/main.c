#include <unity.h>
#include "tests/diskTests.h"
#include "tests/dirTests.h"
#include "tests/fileTests.h"
#include "string.h"


void setUp() {
}

void tearDown() {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testDiskCreationPowerOfTwo);
    RUN_TEST(testDiskCreationNonPowerOfTwo);
    RUN_TEST(testMakeDirAbsoluteDir);
    RUN_TEST(testEveryonePermissionMakeDirSuccess);
    RUN_TEST(testMakeDirRelativeDir);
    RUN_TEST(testMakeDirFailure);
    RUN_TEST(testUserPermissionMakeDirSuccess);
    RUN_TEST(testGroupPermissionMakeDirSuccess);
    RUN_TEST(testCreateFileDirectBlocksInRoot);
    RUN_TEST(testCreateFilePermissionsFail);
    RUN_TEST(testAppendToFileDirectBlocks);
    RUN_TEST(testCreateFileDirectBlocksInRootOddSize);
    RUN_TEST(testAppendToFileDirectBlocksOddSize);
    RUN_TEST(testAppendFilePermissionsFail);
    RUN_TEST(testCreateFileSingleIndirectBlocksInRoot);
    RUN_TEST(testAppendFileSingleIndirectBlocksOnBoundaryInRoot);
    RUN_TEST(testAppendToFileSingleIndirectBlocksOddSize);
    RUN_TEST(testAppendToFileAcrossDirectBlockBoundary);
    RUN_TEST(testCreateFileDoubleIndirectBlocksInRoot);
    RUN_TEST(testAppendFileDoubleIndirectBlocksOnBoundaryInRoot);
    RUN_TEST(testAppendToFileAcrossSingleIndirectBlockBoundary);
    RUN_TEST(testAppendToFileDoubleIndirectBlocksOddSize);
    RUN_TEST(testReadDirectBlockFile);
    RUN_TEST(testReadSingleIndirectDirectBlockFile);
    RUN_TEST(testReadDoubleIndirectDirectBlockFile);
}