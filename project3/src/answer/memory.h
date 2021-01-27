#ifndef VIRTUALMEMFRAMEWORKC_MEMORY_H
#define VIRTUALMEMFRAMEWORKC_MEMORY_H

typedef struct Thread Thread;

/**
 * Given a thread and an address, this function will return back the file used to cache the page of memory associated
 * with this address for the given thread. This is used by the tests to very caching and paging works.
 * @param thread The thread for the page cache file we are interested in.
 * @param addr An address to determine the correct page to return.
 * @return The name of the file including the full directory path.
 */
char* getCacheFileName(Thread* thread, int addr);

/**
 * This function will read from a given address in memory. This address may be anywhere in memory. Anything in kernel
 * memory should immediately call the kernel panic method located in utils.c. The tests depend on this feedback to know
 * there was a kernel panic. An invalid address should also result in a kernel panic. Any other call should result in the
 * data from the page being written into the outdata param.
 * @param thread The thread that we are interested in reading memory from.
 * @param addr The address in that thread to read from.
 * @param size The number of bytes to read.
 * @param outData A pointer that has already been allocated with at least the size above that the read data will be put in.
 */
void readFromAddr(Thread* thread, int addr, int size, void* outData);

/**
 * This function will write to a given address in memory. This address may be anywhere in memory. Anything in kernel
 * memory should immediately call the kernel panic method located in utils.c. The tests depend on this feedback to know
 * there was a kernel panic. An invalid address should also result in a kernel panic. Any other call should result in the
 * data provided by data being written into memory.
 * @param thread The thread that we are interested in writing to.
 * @param addr The address in that thread to write to.
 * @param size The number of bytes to write.
 * @param outData A pointer that contains the data to be written into memory.
 */
void writeToAddr(Thread* thread, int addr, int size, const void* data);

/**
 * This function allocates heap memory in the given thread of the given size. If there is no more heap memory left for
 * allocation in this thread, the function returns -1. Heap memory starts at USER_BASE_ADDR and goes up to STACK_END_ADDR.
 * These constants are listed in memory.c.
 * @param thread The thread for which we want to allocate heap memory.
 * @param size The number of bytes we want to allocate.
 * @return The address the space was allocated at or -1 if we were unable to allocate heap memory.
 */
int allocateHeapMem(Thread *thread, int size);

/**
 * This function allocates stack memory in the given thread of the given size. If there is no more stack memory left for
 * allocation in this thread, the function returns -1. Stack memory starts at ALL_MEM_SIZE and goes down to STACK_END_ADDR.
 * These constants are listed in memory.c.
 * @param thread The thread for which we want to allocate stack memory.
 * @param size The number of bytes we want to allocate.
 * @return The address the space was allocated at or -1 if we were unable to allocate stack memory.
 */
int allocateStackMem(Thread *thread, int size);
#endif //VIRTUALMEMFRAMEWORKC_MEMORY_H
