#ifndef FILESYSTEM_FILETESTS_H
#define FILESYSTEM_FILETESTS_H

void testCreateFileDirectBlocksInRoot();
void testCreateFilePermissionsFail();
void testAppendToFileDirectBlocks();
void testAppendFilePermissionsFail();
void testCreateFileDirectBlocksInRootOddSize();
void testAppendToFileDirectBlocksOddSize();
void testCreateFileSingleIndirectBlocksInRoot();
void testAppendFileSingleIndirectBlocksOnBoundaryInRoot();
void testAppendToFileSingleIndirectBlocksOddSize();
void testAppendToFileAcrossDirectBlockBoundary();
void testCreateFileDoubleIndirectBlocksInRoot();
void testAppendFileDoubleIndirectBlocksOnBoundaryInRoot();
void testAppendToFileDoubleIndirectBlocksOddSize();
void testAppendToFileAcrossSingleIndirectBlockBoundary();
void testReadDirectBlockFile();
void testReadSingleIndirectDirectBlockFile();
void testReadDoubleIndirectDirectBlockFile();
#endif
