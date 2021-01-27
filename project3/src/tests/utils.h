#ifndef VIRTUALMEMFRAMEWORKC_UTILS_H
#define VIRTUALMEMFRAMEWORKC_UTILS_H

#include "thread.h"

void kernelPanic(const Thread* thread, int addr);
void* createRandomData(int size);
int allocateAndWriteHeapData(Thread *thread, void *data, int allocateSize, int writeSize);
int allocateAndWriteStackData(Thread *thread, void *data, int allocateSize, int writeSize);
void logData(const char* info);
void flushLog();
#endif //VIRTUALMEMFRAMEWORKC_UTILS_H
