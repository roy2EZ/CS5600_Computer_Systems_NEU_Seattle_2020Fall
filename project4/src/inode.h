#ifndef FILESYSTEM_INODE_H
#define FILESYSTEM_INODE_H
#include <time.h>
#include <stdbool.h>

/**
 * This structure represents an INode, you will likely have to add to this structure in order to
 * make your software work, a key entry would be how you store your pointers.
 */
typedef struct INode {
    int permissions;
    int uid;
    int gid;
    int numBlocks;
} INode;
#endif
