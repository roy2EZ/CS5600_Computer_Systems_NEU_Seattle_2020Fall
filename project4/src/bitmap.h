#ifndef FILESYSTEM_BITMAP_H
#define FILESYSTEM_BITMAP_H
/**
 * This file contains functions that represent a bitmap, you can create manipulate, set, destroy and unset bits in this
 * bitmap.
 */
#include <math.h>
#include <malloc.h>
#include <string.h>

/**
 * This struct contains an unsigned int that represents individual bits and a size of the bitmap
 */
typedef struct Bitmap {
    unsigned int* bitmap;
    int size;
} Bitmap;

/**
 * Creates a bitmap of the specified size, it will store no extra data.
 * @param numBits The number of bits to be able to store.
 * @return A Bitmap to be used for manipulation.
 */
Bitmap createBitmap(int numBits);

/**
 * Destroys a bitmap, call this when you no longer need a bitmap.
 * @param bitmap The bitmap to destroy
 */
void destroyBitmap(Bitmap bitmap);

/**
 * Sets a bit in the bitmap from 0 to 1 so that isBitSet will now return true for that bit.
 * @param bitmap The bitmap to operate on.
 * @param bit The bit to set, bit is indexed at 0
 */
void setBit(Bitmap bitmap, int bit);

/**
 * Sets a bit in the bitmap from 1 to 0 so that isBitSet will now return false for that bit.
 * @param bitmap The bitmap to operate on.
 * @param bit The bit to set, bit is indexed at 0
 */
void unsetBit(Bitmap bitmap, int bit);

/**
 * Checks if a bit is set,returns true if the bit is set to 1, false if it is set to 0.
 * @param bitmap The bitmap to operate on.
 * @param bit The bit to check.
 * @return true if the bit is set to 1, false if bit is set to 0
 */
int isBitSet(Bitmap bitmap, int bit);

/**
 * Finds the first bit in the bitmap that is set to 0.
 * @param bitmap The bitmap to operate on.
 * @return The index of the unset bit, indexing is 0 based.
 */
int findFirstUnsetBit(Bitmap bitmap);

#endif
