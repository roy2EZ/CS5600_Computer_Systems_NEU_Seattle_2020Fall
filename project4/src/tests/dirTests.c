#include "dirTests.h"
#include "disk.h"
#include "unity.h"

extern int SECTOR_SIZE;

void testMakeDirRelativeDir() {
    InMemoryDisk *disk = createDisk(SECTOR_SIZE*5);
    WriteResponse* sectorList = makeDir("newDir", "/", disk, 0, 0, 755);
    TEST_ASSERT_NOT_NULL(sectorList);
    TEST_ASSERT_EQUAL(1, sectorList->numSectors);
    TEST_ASSERT_EQUAL_STRING(".:0\n..:0\nnewDir:1\n", disk->data+(disk->rootSector*SECTOR_SIZE));
    TEST_ASSERT_EQUAL_STRING(".:1\n..:0\n", disk->data +(sectorList->sectors[0].sector*SECTOR_SIZE));
    destroyWriteResponse(sectorList);
    destroyDisk(disk);
}

void testMakeDirAbsoluteDir() {
    InMemoryDisk *disk = createDisk(SECTOR_SIZE*5);
    WriteResponse* sectorList = makeDir("/newDir", "/", disk, 0, 0, 755);
    TEST_ASSERT_NOT_NULL(sectorList);
    TEST_ASSERT_EQUAL(1, sectorList->numSectors);
    TEST_ASSERT_EQUAL_STRING(".:0\n..:0\nnewDir:1\n", disk->data+(disk->rootSector*SECTOR_SIZE));
    TEST_ASSERT_EQUAL_STRING(".:1\n..:0\n", disk->data +(sectorList->sectors[0].sector*SECTOR_SIZE));
    destroyWriteResponse(sectorList);
    destroyDisk(disk);
}

void testEveryonePermissionMakeDirSuccess() {
    InMemoryDisk *disk = createDisk(SECTOR_SIZE*5);
    disk->iNodeTable[0].permissions = 2;
    WriteResponse* sectorList = makeDir("/newDir", "/", disk, 0, 0, 2);
    TEST_ASSERT_NOT_NULL(sectorList);
    TEST_ASSERT_EQUAL(1, sectorList->numSectors);
    TEST_ASSERT_EQUAL_STRING(".:0\n..:0\nnewDir:1\n", disk->data+(disk->rootSector*SECTOR_SIZE));
    TEST_ASSERT_EQUAL_STRING(".:1\n..:0\n", disk->data +(sectorList->sectors[0].sector*SECTOR_SIZE));
    destroyWriteResponse(sectorList);
    destroyDisk(disk);
}

void testMakeDirFailure() {
    InMemoryDisk *disk = createDisk(SECTOR_SIZE*5);
    WriteResponse* sectorList =  makeDir("/newDir", "/", disk, 0, 0, 0);
    TEST_ASSERT_NOT_NULL(sectorList);
    TEST_ASSERT_EQUAL_STRING(".:0\n..:0\nnewDir:1\n", disk->data+(disk->rootSector*SECTOR_SIZE));
    TEST_ASSERT_EQUAL_STRING(".:1\n..:0\n", disk->data +(sectorList->sectors[0].sector*SECTOR_SIZE));
    destroyWriteResponse(sectorList);
    sectorList = makeDir("/newDir/inner", "/", disk, 0, 0, 0);
    TEST_ASSERT_NULL(sectorList);
}

void testUserPermissionMakeDirSuccess() {
    InMemoryDisk *disk = createDisk(SECTOR_SIZE*5);
    disk->iNodeTable[0].permissions = 200;
    WriteResponse* sectorList = makeDir("/newDir", "/", disk, 0, 0, 200);
    TEST_ASSERT_NOT_NULL(sectorList);
    TEST_ASSERT_EQUAL_STRING(".:0\n..:0\nnewDir:1\n", disk->data+(disk->rootSector*SECTOR_SIZE));
    TEST_ASSERT_EQUAL_STRING(".:1\n..:0\n", disk->data +(sectorList->sectors[0].sector*SECTOR_SIZE));
    destroyWriteResponse(sectorList);
    destroyDisk(disk);
}

void testGroupPermissionMakeDirSuccess() {
    InMemoryDisk *disk = createDisk(SECTOR_SIZE*5);
    disk->iNodeTable[0].permissions = 20;
    WriteResponse* sectorList = makeDir("/newDir", "/", disk, 0, 0, 20);
    TEST_ASSERT_NOT_NULL(sectorList);
    TEST_ASSERT_EQUAL_STRING(".:0\n..:0\nnewDir:1\n", disk->data+(disk->rootSector*SECTOR_SIZE));
    TEST_ASSERT_EQUAL_STRING(".:1\n..:0\n", disk->data +(sectorList->sectors[0].sector*SECTOR_SIZE));
    destroyWriteResponse(sectorList);
    destroyDisk(disk);
}
