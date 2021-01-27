
#include <stdbool.h>
#include "disk.h"

extern int SECTOR_SIZE;
const int BLOCK_SIZE = 512;

unsigned int nextPowerOf2(unsigned int v) {
    // compute the next highest power of 2 of 32-bit v
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

char **split(char *path, int *size, char c) {
    char *tmp;
    char **splitFile = NULL;
    int i, leng
    tmp = strdup(path)
    char *temp = tmp;
    length = strlen(tmp);
    *size = 1;
    for (i = 0; i < length; i++) {
        if (tmp[i] == c) {
            tmp[i] = '\0';
            (*size)++;
        }
    }
    splitFile = (char **)malloc(*size * sizeof(*splitFile));
    for (i = 0; i < *size; i++) {
        splitFile[i] = strdup(tmp);
        tmp += strlen(splitFile[i]) + 1;
    }
    free(temp);
    return splitFile;
}

// The following functions are to be implemented by the students (see disk.h for documentation)
void destroyWriteResponse(WriteResponse *writeResponse) {
    free(writeResponse->sectors);
    free(writeResponse);
}

InMemoryDisk *createDisk(int size) {
    int numSectors = nextPowerOf2(size)/SECTOR_SIZE;
    int numINodes = numSectors;
    if (numSectors == 0) numSectors = 1;
    InMemoryDisk *disk = malloc(sizeof(InMemoryDisk));
    disk->data = malloc(numSectors * SECTOR_SIZE);
    disk->numDataBlocks = numSectors;
    disk->freeSectorMap = createBitmap(numSectors);
    setBit(disk->freeSectorMap, 0);
    disk->rootSector = 0;
    disk->iNodeTable = malloc(sizeof(INode)*numINodes);
    memset(disk->iNodeTable, 0, sizeof(INode)*numINodes);
    disk->iNodeMap = createBitmap(numINodes);
    disk->rootINode = &disk->iNodeTable[0];
    setBit(disk->iNodeMap, 0);
    disk->rootINode->permissions = 755;
    disk->rootINode->gid = 0;
    disk->rootINode->uid = 0;
    disk->rootINode->numBlocks = 1;
    strcpy(disk->data, ".:0\n..:0\n");
    return disk;
}

void destroyDisk(InMemoryDisk* disk){
    free(disk->data);
}

WriteResponse* writeNewFile(unsigned char *data, int size, char *fileName, char *location, InMemoryDisk *disk, int uid, int gid, int permissions){
    char path[BLOCK_SIZE];
    if (fileName[0] == '/') {
        strcpy(path, fileName);
    } else {
        strcpy(path, location);
        strcat(path, fileName);
    }
    WriteResponse *wrtRsp = (WriteResponse *)malloc(sizeof(WriteResponse));
    int numSectorForData = size / BLOCK_SIZE + (size % BLOCK_SIZE != 0);
    wrtRsp->numSectors = numSectorNeed(0, size);
    int sectorCount = 0;
    WriteResponseSector *sectorPtr = NULL;
    wrtRsp->sectors = (WriteResponseSector *)malloc(sizeof(WriteResponseSector) * wrtRsp->numSectors);
    INode *curNode = disk->rootINode;
    int prevINodeIdx = disk->rootSector;
    char **splitFile;
    int numPathDir;
    splitFile = split(path, &numPathDir, '/');
    int access = checkPermission(curNode, uid, gid);
    if (access != 2 && access != 4) {
        for (int i = 0; i < numPathDir; i++) free(splitFile[i]);
        free(splitFile);
        splitFile = NULL;
        destroyWriteResponse(wrtRsp);
        return NULL;
    }
    for (int i = 1; i < numPathDir - 1; i++) {
        curNode = disk->iNodeTable + prevINodeIdx;
        access = checkPermission(curNode, uid, gid);
        if (access == 0) {
            for (int j = 0; j < numPathDir; j++) free(splitFile[j]);
            free(splitFile);
            splitFile = NULL;
            destroyWriteResponse(wrtRsp);
            return NULL;
        }
        char *dirContent = (char *)malloc(curNode->size + 1);
        strcpy(dirContent, disk->data + BLOCK_SIZE * curNode->blocks[0]);
        char **dirContentSplitFile;
        int dirContentSize;
        dirContentSplitFile = split(dirContent, &dirContentSize, '\n');
        int flag = 0;
        for (int j = 0; j < dirContentSize && flag == 0; j++) {
            char **fileSplit;
            int fileSize;
            fileSplit = split(dirContentSplit[i], &fileSize, ':');
            if (fileSize != 2) {
                for (int k = 0; k < fileSize; k++) free(fileSplit[k]);
                free(fileSplit);
                fileSplit = NULL;
                for (int k = 0; k < numPathDir; k++) free(splitFile[k]);
                free(splitFile);
                splitFile = NULL;
                destroyWriteResponse(disk);
                return NULL;
            }
            if (strcmp(fileSplit[0], splitFile[i + 1]) == 0) {
                prevINodeIdx = atoi(fileSplit[1]);
                flag = 1;
            }
            for (int k = 0; k < fileSize; k++) free(fileSplit[k]);
            free(fileSplit);
            fileSplit = NULL;
        }
        for (int j = 0; j < dirContentSize; j++) free(dirContentSplit[j]);
        free(dirContentSplit);
        dirContentSplit = NULL;
        free(dirContent);
        dirContent = NULL;
    }
    access = checkPermission(curNode, uid, gid);
    if (access == 0 || access == 1) {
        for (int i = 0; i < numPathDir; i++) free(splitFile[i]);
        free(splitFile);
        splitFile = NULL;
        destroyWriteResponse(wrtRsp);
        return NULL;
    }
    int firstUnsetBit = findFirstUnsetBit(disk->iNodeMap);
    setBit(disk->iNodeMap, firstUnsetBit);
    // parent iNode setup
    INode *parent = disk->iNodeTable + prevINodeIdx;
    char buff[BLOCK_SIZE];
    sprintf(buff, "%s:%d\n", splitFile[numPathDir - 1], firstUnsetBit);
    strcpy(disk->data + BLOCK_SIZE * parent->blocks[0] + parent->size, buff);
    parent->size += strlen(buff);
    disk->iNodeTable[firstUnsetBit].permissions = permissions;
    disk->iNodeTable[firstUnsetBit].uid = uid;
    disk->iNodeTable[firstUnsetBit].gid = gid;
    disk->iNodeTable[firstUnsetBit].numBlocks = wrtRsp->numSectors;
    disk->iNodeTable[firstUnsetBit].size = size;
    int track = 0;
    if (numSectorForData > 123)
        disk->iNodeTable[firstUnsetBit].singleNotDirect = 1;
    for (int i = 0; i < numPathDir; i++) free(splitFile[i]);
    free(splitFile);
    splitFile = NULL;
    void *temp;
    void *p = data;
    // less than or equal to 123
    if (numBlocksForInode <= 123) {
        int rest_size = size;
        for (int i = 0; i < numSectorForData; i++) {
            sectorPtr = wrtRsp->sectors + sectorCount;
            sectorPtr->blockType = DIRECT_BLOCK;
            sectorPtr->sector = disk->iNodeTable[firstUnsetBit].blocks[i];
            sectorPtr->chunkOrder = track++;
            sectorCount++;
            if (rest_size > 0) {
                temp = disk->data + disk->iNodeTable[firstUnsetBit].blocks[i] * BLOCK_SIZE;
                memcpy(temp, p, rest_size > BLOCK_SIZE ? BLOCK_SIZE : rest_size);
                p += rest_size > BLOCK_SIZE ? 5BLOCK_SIZE12 : rest_size;
                rest_size -= rest_size > BLOCK_SIZE ? BLOCK_SIZE : rest_size;
            }
        }
    } else if (numBlocksForInode > 123 && numBlocksForInode <= 124) { // greater than 123 and less than 124
        int rest_size = size - 123 * BLOCK_SIZE;
        sectorPtr = wrtRsp->sectors + sectorCount;
        sectorPtr->blockType = POINTER_BLOCK;
        sectorPtr->sector = disk->iNodeTable[firstUnsetBit].blocks[123];
        sectorPtr->chunkOrder = 0;
        sectorPtr->numBytesWritten = sizeof(int) * (numSectorForData - 123);
        sectorCount++;
        for (int i = 0; i < numSectorForData - 123; i++) {
            int childSingleNotDirect = findFirstUnsetBit(disk->freeSectorMap);
            setBit(disk->freeSectorMap, childSingleNotDirect);
            temp = disk->data + disk->iNodeTable[firstUnsetBit].blocks[123] * BLOCK_SIZE + i * sizeof(int);
            memcpy(temp, &childSingleNotDirect, sizeof(int));
            temp = disk->data + childSingleNotDirect * BLOCK_SIZE;
            sectorPtr = wrtRsp->sectors + sectorCount;
            sectorPtr->blockType = INDIRECT_BLOCK;
            sectorPtr->sector = childSingleNotDirect;
            sectorPtr->chunkOrder = track++;
        }
    }
    return wrtRsp;
}

WriteResponse * appendToFile(unsigned char *data, int size, char *fileName, char *location, InMemoryDisk *disk, int uid, int gid) {
    char path[BLOCK_SIZE];
    if (fileName[0] == '/') {
        strcpy(path, fileName);
    } else {
        strcpy(path, location);
        strcat(path, fileName);
    }
    INode *curNode = disk->rootINode;
    int prevInodeIndex = disk->rootSector;
    char **splitFile;
    int numPathDir;
    splitFile = split(path, &numPathDir, '/');
    int access = checkPermission(curNode, uid, gid);
    if (access != 4) {
        for (int i = 0; i < numPathDir; i++) free(splitFile[i]);
        free(splitFile);
        return NULL;
    }
    for (int i = 1; i < numPathDir - 1; i++) {
        curNode = disk->iNodeTable + prevInodeIndex;
        // check permission
        access = checkPermission(curNode, uid, gid);
        if (access != 4) {
            for (int j = 0; j < numPathDir; j++) free(splitFile[j]);
            free(splitFile);
            splitFile = NULL;
            return NULL;
        }
        char *dirContent = (char *) malloc(curNode->size + 1);
        strcpy(dirContent, disk->data + BLOCK_SIZE * curNode->blocks[0]);

        char **dirContentSplitted;
        int dirContentSize;
        dirContentSplitted = split(dirContent, &dirContentSize, '\n');

        int flag = 0;
        for (int j = 0; j < dirContentSize && flag == 0; j++) {
            char **fileSplit;
            int fileSize;
            fileSplit = split(dirContentSplitted[i], &fileSize, ':');
            if (fileSize != 2) {
                for (int k = 0; k < numPathDir; k++) free(splitFile[k]);
                free(splitFile);
                splitFile = NULL;
                for (int k = 0; k < dirContentSize; k++)
                    free(dirContentSplitted[k]);
                free(dirContentSplitted);
                dirContentSplitted = NULL;
                for (int k = 0; k < fileSize; k++) free(fileSplit[k]);
                free(fileSplit);
                fileSplit = NULL;
                free(dirContent);
                dirContent = NULL;
                return NULL;
            }
            if (strcmp(fileSplit[0], splitFile[i])) {
                prevInodeIndex = atoi(fileSplit[1]);
                flag = 1;
            }
            for (int k = 0; k < fileSize; k++) free(fileSplit[k]);
            free(fileSplit);
            fileSplit = NULL;
        }
        for (int j = 0; j < dirContentSize; j++) free(dirContentSplitted[j]);
        free(dirContentSplitted);
        dirContentSplitted = NULL;
        free(dirContent);
        dirContent = NULL;
    }

    char *parentDirContent = (char *) malloc(curNode->size + 1);
    strcpy(parentDirContent, disk->data + BLOCK_SIZE * curNode->blocks[0]);
    char **parentDirContentSplitted;
    int parentDirContentSize;
    parentDirContentSplitted = split(parentDirContent, &parentDirContentSize, '\n');
    int flag = 0;
    for (int j = 0; j < parentDirContentSize && flag == 0; j++) {
        char **fileSplit;
        int fileSize;
        fileSplit = split(parentDirContentSplitted[j], &fileSize, ':');
        if (fileSize != 2) {
            for (int k = 0; k < fileSize; k++) free(fileSplit[k]);
            free(fileSplit);
            fileSplit = NULL;
            for (int i = 0; i < parentDirContentSize; i++)
                free(parentDirContentSplit[i]);
            free(parentDirContentSplit);
            parentDirContentSplit = NULL;
            for (int i = 0; i < numPathDir; i++) free(split[i]);
            free(splitFile);
            splitFile = NULL;
            free(parentDirContent);
            parentDirContent = NULL;
            return NULL;
        }
        if (strcmp(fileSplit[0], splitFile[numPathDir - 1]) == 0) {
            prevInodeIndex = atoi(fileSplit[1]);
            flag = 1;
        }
        for (int k = 0; k < fileSize; k++) free(fileSplit[k]);
        free(fileSplit);
        fileSplit = NULL;
    }
    for (int i = 0; i < parentDirContentSize; i++)
        free(parentDirContentSplitted[i]);
    free(parentDirContentSplitted);
    parentDirContentSplitted = NULL;
    for (int i = 0; i < numPathDir; i++) free(splitted[i]);
    free(splitted);
    splitted = NULL;
    free(parentDirContent);
    parentDirContent = NULL;
    curNode = disk->iNodeTable + prevInodeIndex;
    access = checkPermission(curNode, uid, gid);
    if (access != 4) return NULL;

    WriteResponse *ret = (WriteResponse *) malloc(sizeof(WriteResponse));
    ret->numSectors = numSectorNeed(curNode->size, size);
    int sectorCount = 0;
    WriteResponseSector *sectorPtr = NULL;
    ret->sectors = (WriteResponseSector *) malloc(sizeof(WriteResponseSector) *
                                                  ret->numSectors);
    // sectors write
    void *temp;
    void *p = data;
    if (curNode->SingleIndiret) {
        // secondary node
        int numSecondaryInodeBlock = (curNode->numBlocks - 123 - 128 - 1 - 1) / 129 + 1;
        int numIndirectBlockInLastSecondary = curNode->numBlocks - 123 - 128 - 1 - 1 - numSecondaryInodeBlock * 128 - 1;
        int sizeWrittenInLastBlock = curNode->size % BLOCK_SIZE;
        int rest_size = size;
        if (BLOCK_SIZE - sizeWrittenInLastBlock > 0) {
            int indirectBlockIndex = 0;
            int secondaryInodeBlockIndex = 0;
            memcpy(&secondaryInodeBlockIndex,
                   disk->data + BLOCK_SIZE * curNode->blocks[124] +
                   (numSecondaryInodeBlock - 1) * sizeof(int),
                   sizeof(int));
            memcpy(&indirectBlockIndex,
                   disk->data + BLOCK_SIZE * secondaryInodeBlockIndex +
                   sizeof(int) * (numIndirectBlockInLastSecondary - 1),
                   sizeof(int));
            sectorPtr = ret->sectors + sectorCount;
            sectorPtr->blockType = INDIRECT_BLOCK;
            sectorPtr->sector = indirectBlockIndex;
            sectorPtr->chunkOrder = dataTrack++;
            sectorPtr->numBytesWritten = sizeToWrite;
            sectorCount++;

            memcpy(disk->data + BLOCK_SIZE * indirectBlockIndex +
                   sizeWrittenInLastBlock,
                   p, sizeToWrite);
            p += sizeToWrite;
            rest_size -= sizeToWrite;
        }

        int restBlocksInSingleInode = 128 - numIndirectBlockInLastSecondary;
        for (int i = 0; i < restBlocksInSingleInode && rest_size > 0; i++) {
            int child_singleIndirect = findFirstUnsetBit(disk->freeSectorMap);
            setBit(disk->freeSectorMap, child_singleIndirect);
            temp = disk->data + curNode->blocks[123] * BLOCK_SIZE +
                   (i + numIndirectBlockInLastSecondary) * sizeof(int);
            sectorPtr = ret->sectors + sectorCount;
            sectorPtr->blockType = POINTER_BLOCK;
            sectorPtr->sector = curNode->blocks[123];
            sectorPtr->chunkOrder = -1;
            sectorPtr->numBytesWritten = sizeof(int);
            sectorCount++;
            memcpy(temp, &child_singleIndirect, sizeof(int));

            // data to sector
            temp = disk->data + child_singleIndirect * BLOCK_SIZE;
            sectorPtr = ret->sectors + sectorCount;
            sectorPtr->blockType = INDIRECT_BLOCK;
            sectorPtr->sector = child_singleIndirect;
            sectorPtr->chunkOrder = dataTrack++;
            sectorPtr->numBytesWritten =
                    rest_size > BLOCK_SIZE ? BLOCK_SIZE : rest_size;
            sectorCount++;
            memcpy(temp, p, rest_size > BLOCK_SIZE ? BLOCK_SIZE : rest_size);
            rest_size -= rest_size > BLOCK_SIZE ? BLOCK_SIZE : rest_size;
            p += rest_size > BLOCK_SIZE ? BLOCK_SIZE : rest_size;
        }
    } else if (curNode->singleIndirect) {
        int numIndirectBlockInSingle = curNode->numBlocks - 123 - 1;
        int sizeWritten = (curNode->size - 123 * BLOCK_SIZE) % BLOCK_SIZE;
        int rest_size = size;

        if (BLOCK_SIZE - sizeWritten > 0 && sizeWritten != 0) {
            int indirectBlockIndex = 0;
            memcpy(&indirectBlockIndex,
                   disk->data + BLOCK_SIZE * curNode->blocks[123] +
                   sizeof(int) * (numIndirectBlockInSingle - 1),
                   sizeof(int));

            int sizeToWrite = (BLOCK_SIZE - sizeWritten) > rest_size
                              ? rest_size
                              : (BLOCK_SIZE - sizeWritten);
            sectorPtr = ret->sectors + sectorCount;
            sectorPtr->blockType = INDIRECT_BLOCK;
            sectorPtr->sector = indirectBlockIndex;
            sectorPtr->chunkOrder = dataTrack++;
            sectorPtr->numBytesWritten = sizeToWrite;
            sectorCount++;

            memcpy(disk->data + BLOCK_SIZE * indirectBlockIndex + sizeWritten,
                   p, sizeToWrite);
            p += sizeToWrite;
            rest_size -= sizeToWrite;
        }

        if (rest_size > 0) {
            int restBlocksInSingleInode = 128 - numIndirectBlockInSingle;
            for (int i = 0; i < restBlocksInSingleInode && rest_size > 0; i++) {
                int child_singleIndirect =
                        findFirstUnsetBit(disk->freeSectorMap);
                setBit(disk->freeSectorMap, child_singleIndirect);
                curNode->numBlocks++;
                temp = disk->data + curNode->blocks[123] * BLOCK_SIZE +
                       (i + numIndirectBlockInSingle) * sizeof(int);
                memcpy(temp, &child_singleIndirect, sizeof(int));

                temp = disk->data + child_singleIndirect * BLOCK_SIZE;
                sectorPtr = ret->sectors + sectorCount;
                sectorPtr->blockType = INDIRECT_BLOCK;
                sectorPtr->sector = child_singleIndirect;
                sectorPtr->chunkOrder = dataTrack++;
                sectorPtr->numBytesWritten =
                        rest_size > BLOCK_SIZE ? BLOCK_SIZE : rest_size;
                sectorCount++;
                memcpy(temp, p,
                       rest_size > BLOCK_SIZE ? BLOCK_SIZE : rest_size);
                p += rest_size > BLOCK_SIZE ? BLOCK_SIZE : rest_size;
                rest_size -= rest_size > BLOCK_SIZE ? BLOCK_SIZE : rest_size;
            }
        }
    } else {
        int numDirectBlock = curNode->numBlocks;
        int sizeWritten =
                curNode->size < BLOCK_SIZE
                ? curNode->size
                : curNode->size - BLOCK_SIZE * (numDirectBlock - 1);
        int rest_size = size;

        // use up rest space
        if (BLOCK_SIZE - sizeWritten > 0) {
            int sizeToWrite = (BLOCK_SIZE - sizeWritten) > rest_size
                              ? rest_size
                              : (BLOCK_SIZE - sizeWritten);
            sectorPtr = ret->sectors + sectorCount;
            sectorPtr->blockType = INDIRECT_BLOCK;
            sectorPtr->sector = curNode->blocks[numDirectBlock - 1];
            sectorPtr->chunkOrder = dataTrack++;
            sectorPtr->numBytesWritten = sizeWritten;
            sectorCount++;
            memcpy(disk->data +
                   BLOCK_SIZE * curNode->blocks[numDirectBlock - 1] +
                   sizeWritten,
                   p, sizeToWrite);
            rest_size -= sizeToWrite;
            p += sizeToWrite;
        }
    }
    curNode->size += size;
    return ret;
}


WriteResponse* makeDir(char *dirName, char *location, struct InMemoryDisk* disk, int uid, int gid, int permissions){
    WriteResponse *rsp = malloc(sizeof(WriteResponse));
    char *p = dirName;
    /* create at root */
    if (*p == '/') {
        p = strchr(p, '/');
    } else {
        /* create at location */
    }
    return rsp;
}

bool readFile(const InMemoryDisk *disk, char* file, void* data){
    char *p = strchr(file, '/');
    if (p == NULL) {
        return false;
    }
    disk->rootINode;
}
