#include "bitmap.h"

Bitmap createBitmap(int numBits) {
    Bitmap ret;
    ret.size = numBits;
    int numInts = (numBits/32)+1;
    ret.bitmap = malloc(numInts * sizeof(unsigned int));
    bzero(ret.bitmap, numInts * sizeof(unsigned int));
    return ret;
}

void destroyBitmap(Bitmap bitmap) {
    free(bitmap.bitmap);
}

void setBit(Bitmap bitmap, int bit) {
    int chooseInt = bit/32;
    int index = bit%32;
    bitmap.bitmap[chooseInt] += (int)pow(2, index);
}

void unsetBit(Bitmap bitmap, int bit) {
    int chooseInt = bit/32;
    int index = bit%32;
    bitmap.bitmap[chooseInt] -= (int)pow(2, index);
}

int isBitSet(Bitmap bitmap, int bit) {
    if (bit > bitmap.size) return -1;
    int chooseInt = bit/32;
    int index = bit%32;
    return bitmap.bitmap[chooseInt] & (int)pow(2, index);
}

int findFirstUnsetBit(Bitmap bitmap) {
    int ret = 0;
    for (int x = 0; x <(bitmap.size/32)+1; x++)
    {
        int availableBits = bitmap.bitmap[x] & 0xFFFFFFFF;
        if (availableBits == 0xFFFFFFFF){
            ret+=32;
            continue;
        }

        while(availableBits & 1) {
            availableBits = availableBits >> 1;
            ret++;
        }
        return ret;
    }
    return -1;
}