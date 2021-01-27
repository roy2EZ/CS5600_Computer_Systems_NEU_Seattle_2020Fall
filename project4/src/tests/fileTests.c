#include <stdlib.h>
#include "fileTests.h"
#include "disk.h"
#include "unity.h"

extern int SECTOR_SIZE;

void checkWriteResponse(WriteResponse* sectorList, InMemoryDisk* disk, void* data) {
    TEST_ASSERT_NOT_NULL(sectorList);
    char message[4096];
    int viewedBlocks[sectorList->numSectors];
    for (int x = 0; x < sectorList->numSectors; x++) viewedBlocks[x] = -1;

    for (int x = 0, y=0; x < sectorList->numSectors; x++, y++) {
        if (sectorList->sectors[x].blockType == POINTER_BLOCK) continue;
        viewedBlocks[y] = sectorList->sectors[x].sector;
        sprintf(message, "x:%d SECTOR:%d CHUNK:%d", x, sectorList->sectors[x].sector, sectorList->sectors[x].chunkOrder);
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(data+(SECTOR_SIZE*sectorList->sectors[x].chunkOrder), disk->data+(SECTOR_SIZE*sectorList->sectors[x].sector), sectorList->sectors[x].numBytesWritten,message);
    }

    for (int x = 0; x < sectorList->numSectors; x++) {
        if (sectorList->sectors[x].blockType != POINTER_BLOCK) continue;
        int *pointerBlock = disk->data + (sectorList->sectors[x].sector*SECTOR_SIZE);
        for (int x = 0; x < 128; x++, pointerBlock++) {
            if (*pointerBlock == 0) break;
            bool sectorInList = false;
            for(int y = 0; y < sectorList->numSectors; y++) {
                if (*pointerBlock == sectorList->sectors[y].sector) {
                    sectorInList = true;
                    break;
                }
            }
            TEST_ASSERT_TRUE_MESSAGE(sectorInList, "A sector in the pointer block is not in the sector list");
        }
    }
}
void testCreateFileDirectBlocksInRoot() {
    InMemoryDisk* disk = createDisk(SECTOR_SIZE*124);
    int size = 512*123;
    unsigned char buffer[size];
    for (int x = 0; x < size; x++) {
        buffer[x] = rand()%57+65;
    }
    WriteResponse* writeResponse = writeNewFile(buffer, 512 * 123, "NewFile", "/", disk, 0, 0, 755);

    checkWriteResponse(writeResponse, disk, buffer);
    destroyWriteResponse(writeResponse);
    destroyDisk(disk);
}

void testCreateFileSingleIndirectBlocksInRoot() {
    InMemoryDisk* disk = createDisk((SECTOR_SIZE*125)+(SECTOR_SIZE*128));
    int size = (SECTOR_SIZE*123)+(SECTOR_SIZE*128);
    unsigned char buffer[size];
    for (int x = 0; x < size; x++) {
        buffer[x] = rand()%57+65;
    }

    WriteResponse* writeResponse = writeNewFile(buffer, size, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    destroyWriteResponse(writeResponse);
    destroyDisk(disk);
}

void testCreateFileDoubleIndirectBlocksInRoot() {
    int size = (SECTOR_SIZE*123)+(SECTOR_SIZE*128)+(SECTOR_SIZE*128*2);
    InMemoryDisk* disk = createDisk(size+(5*SECTOR_SIZE));
    unsigned char buffer[size];
    for (int x = 0; x < size; x++) {
        buffer[x] = rand()%57+65;
    }

    WriteResponse* writeResponse = writeNewFile(buffer, size, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    destroyWriteResponse(writeResponse);
    destroyDisk(disk);
}

void testCreateFileDirectBlocksInRootOddSize() {
    InMemoryDisk* disk = createDisk(SECTOR_SIZE*124);
    int size = (512*121) + rand()%512;
    unsigned char buffer[size];
    for (int x = 0; x < size; x++) {
        buffer[x] = rand()%57+65;
    }

    WriteResponse* writeResponse = writeNewFile(buffer, size, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    destroyWriteResponse(writeResponse);
    destroyDisk(disk);
}


void testCreateFilePermissionsFail() {
    InMemoryDisk* disk = createDisk(SECTOR_SIZE*124);
    int size = 512*123;
    unsigned char buffer[size];
    for (int x = 0; x < size; x++) {
        buffer[x] = rand()%57+65;
    }

    WriteResponse* writeResponse = writeNewFile(buffer, 512 * 123, "NewFile", "/", disk, 1, 1, 755);
    TEST_ASSERT_NULL(writeResponse);
    destroyDisk(disk);
}

void testAppendToFileDirectBlocks() {
    InMemoryDisk* disk = createDisk(SECTOR_SIZE*124);
    int initialSize = SECTOR_SIZE*61;
    int fullSize = SECTOR_SIZE*123;
    unsigned char buffer[fullSize];
    for (int x = 0; x < fullSize; x++) {
        buffer[x] = rand()%57+65;
    }

    WriteResponse* writeResponse = writeNewFile(buffer, initialSize, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    destroyWriteResponse(writeResponse);
    writeResponse = appendToFile(buffer+initialSize, fullSize-initialSize, "NewFile", "/", disk, 0, 0);
    checkWriteResponse(writeResponse, disk, buffer + initialSize);
    destroyWriteResponse(writeResponse);
    destroyDisk(disk);
}

void testAppendFileSingleIndirectBlocksOnBoundaryInRoot() {
    InMemoryDisk* disk = createDisk((SECTOR_SIZE*125)+(SECTOR_SIZE*128));
    int size = (SECTOR_SIZE*123)+(SECTOR_SIZE*128);
    unsigned char buffer[size];
    for (int x = 0; x < size; x++) {
        buffer[x] = rand()%57+65;
    }

    int initialSize = SECTOR_SIZE*123;
    int appendSize = SECTOR_SIZE*128;

    WriteResponse* writeResponse = writeNewFile(buffer, initialSize, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    writeResponse = appendToFile(buffer+initialSize, appendSize, "NewFile", "/", disk, 0, 0);
    checkWriteResponse(writeResponse, disk, buffer + initialSize);
    destroyWriteResponse(writeResponse);
    destroyDisk(disk);
}

void testAppendFileDoubleIndirectBlocksOnBoundaryInRoot() {
    InMemoryDisk* disk = createDisk((SECTOR_SIZE*125)+(5*SECTOR_SIZE*128));
    int size = (SECTOR_SIZE*123)+(5*SECTOR_SIZE*128);
    unsigned char buffer[size];
    for (int x = 0; x < size; x++) {
        buffer[x] = rand()%57+65;
    }

    int initialSize = SECTOR_SIZE*(123+128);
    int appendSize = 4*SECTOR_SIZE*128;

    WriteResponse* writeResponse = writeNewFile(buffer, initialSize, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    writeResponse = appendToFile(buffer+initialSize, appendSize, "NewFile", "/", disk, 0, 0);
    checkWriteResponse(writeResponse, disk, buffer + initialSize);
    destroyWriteResponse(writeResponse);
    destroyDisk(disk);
}

void testAppendToFileDirectBlocksOddSize() {
    InMemoryDisk* disk = createDisk(SECTOR_SIZE*124);
    int initialSize = SECTOR_SIZE * 61;
    int appendSize = (SECTOR_SIZE*61) + rand()%SECTOR_SIZE;
    unsigned char buffer[SECTOR_SIZE*123];
    for (int x = 0; x < (initialSize + appendSize); x++) {
        buffer[x] = rand()%57+65;
    }

    WriteResponse* writeResponse = writeNewFile(buffer, initialSize, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    writeResponse = appendToFile(buffer + initialSize, appendSize, "NewFile", "/", disk, 0, 0);
    checkWriteResponse(writeResponse, disk, buffer + initialSize);
    destroyDisk(disk);
}

void testAppendToFileSingleIndirectBlocksOddSize() {
    InMemoryDisk* disk = createDisk((SECTOR_SIZE*125)+(SECTOR_SIZE*128));
    int initialSize = SECTOR_SIZE * 123;
    int appendSize = (SECTOR_SIZE*5) + rand()%SECTOR_SIZE;
    unsigned char buffer[initialSize+appendSize];
    for (int x = 0; x < (initialSize + appendSize); x++) {
        buffer[x] = rand()%57+65;
    }

    WriteResponse* writeResponse = writeNewFile(buffer, initialSize, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    writeResponse = appendToFile(buffer + initialSize, appendSize, "NewFile", "/", disk, 0, 0);
    checkWriteResponse(writeResponse, disk, buffer + initialSize);
    destroyDisk(disk);
}

void testAppendToFileDoubleIndirectBlocksOddSize() {
    InMemoryDisk* disk = createDisk((SECTOR_SIZE*125)+(6*SECTOR_SIZE*128));
    int initialSize = SECTOR_SIZE * (123+128);
    int appendSize = (SECTOR_SIZE*4*128) + rand()%SECTOR_SIZE;
    unsigned char buffer[initialSize+appendSize];
    for (int x = 0; x < (initialSize + appendSize); x++) {
        buffer[x] = rand()%57+65;
    }

    WriteResponse* writeResponse = writeNewFile(buffer, initialSize, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    writeResponse = appendToFile(buffer + initialSize, appendSize, "NewFile", "/", disk, 0, 0);
    checkWriteResponse(writeResponse, disk, buffer + initialSize);
    destroyDisk(disk);
}

void testAppendToFileAcrossDirectBlockBoundary() {
    InMemoryDisk* disk = createDisk(SECTOR_SIZE*(125+128));
    int initialSize = SECTOR_SIZE * 120;
    int appendSize = (SECTOR_SIZE*10) + rand()%SECTOR_SIZE;
    unsigned char buffer[initialSize+appendSize];
    for (int x = 0; x < (initialSize + appendSize); x++) {
        buffer[x] = rand()%57+65;
    }

    WriteResponse* writeResponse = writeNewFile(buffer, initialSize, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    writeResponse = appendToFile(buffer + initialSize, appendSize, "NewFile", "/", disk, 0, 0);
    checkWriteResponse(writeResponse, disk, buffer + initialSize);
    destroyDisk(disk);
}

void testAppendToFileAcrossSingleIndirectBlockBoundary() {
    InMemoryDisk* disk = createDisk(SECTOR_SIZE*(125+128+10));
    int initialSize = SECTOR_SIZE * (123+125);
    int appendSize = (SECTOR_SIZE*10) + rand()%SECTOR_SIZE;
    unsigned char buffer[initialSize+appendSize];
    for (int x = 0; x < (initialSize + appendSize); x++) {
        buffer[x] = rand()%57+65;
    }

    WriteResponse* writeResponse = writeNewFile(buffer, initialSize, "NewFile", "/", disk, 0, 0, 755);
    checkWriteResponse(writeResponse, disk, buffer);
    writeResponse = appendToFile(buffer + initialSize, appendSize, "NewFile", "/", disk, 0, 0);
    checkWriteResponse(writeResponse, disk, buffer + initialSize);
    destroyDisk(disk);
}

void testAppendFilePermissionsFail() {
    InMemoryDisk* disk = createDisk(SECTOR_SIZE*124);
    unsigned char buffer[512*123];
    for (int x = 0; x < 512*123; x++) {
        buffer[x] = rand()%57+65;
    }
    TEST_ASSERT_TRUE(writeNewFile(buffer, 512 *61, "NewFile", "/", disk, 0, 0,0));
    TEST_ASSERT_EQUAL_MEMORY(buffer, disk->data+SECTOR_SIZE, 512*61);
    TEST_ASSERT_FALSE(appendToFile(buffer, 512 * 62, "NewFile", "/", disk, 0, 0));
    destroyDisk(disk);
}

void testReadDirectBlockFile() {

    InMemoryDisk* disk = createDisk(SECTOR_SIZE*124);
    int size = SECTOR_SIZE*123;
    unsigned char buffer[size];
    for (int x = 0; x < size; x++) {
        buffer[x] = rand()%57+65;
    }
    WriteResponse* writeResponse = writeNewFile(buffer, 512 * 123, "NewFile", "/", disk, 0, 0, 755);

    unsigned char readBuffer[size];

    bool success = readFile(disk, "/NewFile", readBuffer);

    TEST_ASSERT_EQUAL(true, success);
    TEST_ASSERT_EQUAL_MEMORY(buffer, readBuffer, size);
    destroyDisk(disk);
}

void testReadSingleIndirectDirectBlockFile() {
    int size = SECTOR_SIZE*(123+128);
    InMemoryDisk* disk = createDisk(size+(5*SECTOR_SIZE));
    unsigned char buffer[size];
    for (int x = 0; x < size; x++) {
        buffer[x] = rand()%57+65;
    }
    WriteResponse* writeResponse = writeNewFile(buffer, size, "NewFile", "/", disk, 0, 0, 755);

    unsigned char readBuffer[size];

    bool success = readFile(disk, "/NewFile", readBuffer);

    TEST_ASSERT_EQUAL(true, success);
    TEST_ASSERT_EQUAL_MEMORY(buffer, readBuffer, size);

    destroyDisk(disk);
}

void testReadDoubleIndirectDirectBlockFile() {
    int size = SECTOR_SIZE*(123+(5*128));
    InMemoryDisk* disk = createDisk(size+(5*SECTOR_SIZE));
    unsigned char buffer[size];
    for (int x = 0; x < size; x++) {
        buffer[x] = rand()%57+65;
    }
    WriteResponse* writeResponse = writeNewFile(buffer, size, "NewFile", "/", disk, 0, 0, 755);

    unsigned char readBuffer[size];

    bool success = readFile(disk, "/NewFile", readBuffer);

    TEST_ASSERT_EQUAL(true, success);
    TEST_ASSERT_EQUAL_MEMORY(buffer, readBuffer, size);
    destroyDisk(disk);
}
