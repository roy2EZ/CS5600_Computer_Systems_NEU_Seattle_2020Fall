#ifndef FILESYSTEM_DISK_H
#define FILESYSTEM_DISK_H

#include <stdio.h>
#include "inode.h"
#include "bitmap.h"
#include <stdbool.h>

/**
 * An in memory representation of the disk, this is not an on disk representation which means you can take liberties
 * in how things are stored. You are not limited by hard disk limitations.
 */
typedef struct InMemoryDisk {
    Bitmap freeSectorMap;
    Bitmap iNodeMap;
    INode* iNodeTable;
    int numDataBlocks;
    void* data;
    int rootSector;
    INode* rootINode;
} InMemoryDisk;

/**
 * Types of blocks stored on the disk.
 */
typedef enum BlockType {
    // A block that is directly referenced by the INode and is not a directory.
    DIRECT_BLOCK,
    // A block that represents a directory.
    DIRECTORY,
    // A block that holds pointers and no direct data.
    POINTER_BLOCK,
    // A block that is ultimately pointed to by an indirect pointer.
    INDIRECT_BLOCK,
} BlockType;

/**
 * A structure representing the write of a sector, it has the block type written, the sector, and information about the
 * write.
 */
typedef struct WriteResponseSector {
  // The type of block written
  BlockType blockType;
  // The sector written to, the tests assume this is 1 based
  int sector;
  // The chunk of the file this represents.
  int chunkOrder;
  // The number of bytes written to this sector.
  int numBytesWritten;
} WriteResponseSector;

/**
 * A response provided when we write to the disk, it is a list structure for the above structure.
 */
typedef struct WriteResponse {
    int numSectors;
    WriteResponseSector* sectors;
} WriteResponse;

/**
 * A function to clean up any memory resulting from creating a write response.
 * @param writeResponse
 */
void destroyWriteResponse(WriteResponse* writeResponse);

/**
 * Creates an in memory representation of the disk, you may or may not need to use all parts of the structure.
 * @param size The size of the disk to be represented.
 * @return A representation of the disk based on the size given.
 */
InMemoryDisk* createDisk(int size);

/**
 * Destroys the memory allocated when creating a disk.
 * @param disk The disk to destroy.
 */

void destroyDisk(InMemoryDisk* disk);

/**
 * This function writes a "file" to the disk, once done the filename should be reacable for reading and should exist
 * somewhere identifiable on the disk.  There are a lot of arguments so make sure you understand each.
 * @param data The data to be written to the "file"
 * @param size The size of the data, since the data can be anything we need to know how many bytes we need to write.
 * @param fileName The name of the file to write.
 * @param location The location of the file, this can be a relative or absolute path, either one should work.
 * @param disk The disk on which to create the file.
 * @param uid The user id that owns the new file.
 * @param gid The group id that owns the new file.
 * @param permissions The permissions that should apply to the file based on a 3 octal number as used in unix. See octal
 * permission table at https://www.tutorialspoint.com/unix/unix-file-permission.htm
 * @return A write response that contains every sector written to as part of the operation including direct, pointer, and indirect blocks.
 */
WriteResponse* writeNewFile(unsigned char *data, int size, char *fileName, char *location, InMemoryDisk *disk, int uid, int gid,
                  int permissions);

/**
 * This function appends to an existing file on the disk, if the file does not exist, you should return a NULL.
 * There are a lot of arguments so make sure you understand each.
 * @param data The data to be written to the "file"
 * @param size The size of the data, since the data can be anything we need to know how many bytes we need to write.
 * @param fileName The name of the file to write.
 * @param location The location of the file, this can be a relative or absolute path, either one should work.
 * @param disk The disk on which to append to the file.
 * @param uid The user id that owns the new file.
 * @param gid The group id that owns the new file.
 * @param permissions The permissions that should apply to the file based on a 3 octal number as used in unix. See octal
 * permission table at https://www.tutorialspoint.com/unix/unix-file-permission.htm
 * @return A write response that contains every sector written to as part of the operation including direct, pointer,
 * and indirect blocks. Or NULL if the file does not exist.
 */
WriteResponse* appendToFile(unsigned char *data, int size, char *fileName, char *location, InMemoryDisk *disk, int uid, int gid);

/**
 * Create a directory on the disk. Once done the directory should have a location on the disk and containing a file that
 * has the contents of the directory separated by a new line with each line having the name followed by a colon and the
 * block it is on. For example a directory called the newDir in root looks like this:
 * .:0
 * ..:0
 * newDir:1
 *
 * @param dirName The directory name to create.
 * @param location The location where to create the directory.
 * @param disk The disk on which to create the directory.
 * @param uid The user id of the owner of the directory.
 * @param gid The group id of the group owner of the directory/
 * @param permissions The permissions that should apply to the file based on a 3 octal number as used in unix. See octal
 * permission table at https://www.tutorialspoint.com/unix/unix-file-permission.htm
 * @return A write response with one sector that is a direct pointer and full size of 512 bytes.
 */
WriteResponse* makeDir(char *dirName, char *location, struct InMemoryDisk* disk, int uid, int gid, int permissions);

/**
 * This function will read a file from the disk and put it into the data pointer provided. The provided pointer needs
 * to be allocated before calling this function.
 * @param disk The disk on which to create the directory.
 * @param file The file to read from disk, this file will be in absolute path
 * @param data A pointer to the memory location to copy the file data. The memory behind this pointer should be allocated.
 * @return Returns true if able to read the file successfully, otherwise false
 */
bool readFile(const InMemoryDisk *disk, char* file, void* data);

#endif
