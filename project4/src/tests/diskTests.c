#include "diskTests.h"
#include "disk.h"
#include "unity.h"

const int MEGABYTE = 1024*1024;
const int SECTOR_SIZE = 512;

void testDiskCreationPowerOfTwo() {
    InMemoryDisk* disk = createDisk(MEGABYTE*64);
    TEST_ASSERT_EQUAL(1, disk->freeSectorMap.bitmap[0]);
    TEST_ASSERT_EQUAL(((MEGABYTE*64)/SECTOR_SIZE), disk->numDataBlocks);
    TEST_ASSERT_EQUAL(755, disk->iNodeTable->permissions);
    TEST_ASSERT_EQUAL(0, disk->iNodeTable->uid);
    TEST_ASSERT_EQUAL(0, disk->iNodeTable->gid);
    TEST_ASSERT_EQUAL(1, disk->iNodeTable->numBlocks);
    TEST_ASSERT_EACH_EQUAL_CHAR('\0', &(disk->iNodeTable[1]), disk->numDataBlocks*sizeof(INode));
    TEST_ASSERT_EQUAL_STRING(".:0\n..:0\n", disk->data);
    TEST_ASSERT_EACH_EQUAL_CHAR('\0', disk->data+9, (MEGABYTE*64)-9);
    destroyDisk(disk);
}

void testDiskCreationNonPowerOfTwo() {
    InMemoryDisk* disk = createDisk((MEGABYTE*64)+5);
    TEST_ASSERT_EQUAL(1, disk->freeSectorMap.bitmap[0]);
    TEST_ASSERT_EQUAL(((MEGABYTE*64)/SECTOR_SIZE)*2, disk->numDataBlocks);
    TEST_ASSERT_EQUAL(755, disk->iNodeTable->permissions);
    TEST_ASSERT_EQUAL(0, disk->iNodeTable->uid);
    TEST_ASSERT_EQUAL(0, disk->iNodeTable->gid);
    TEST_ASSERT_EQUAL(1, disk->iNodeTable->numBlocks);
    TEST_ASSERT_EACH_EQUAL_CHAR('\0', &(disk->iNodeTable[1]), disk->numDataBlocks*sizeof(INode));
    TEST_ASSERT_EQUAL_STRING(".:0\n..:0\n", disk->data);
    TEST_ASSERT_EACH_EQUAL_CHAR('\0', disk->data+9, (MEGABYTE*64)-9);
    destroyDisk(disk);
}