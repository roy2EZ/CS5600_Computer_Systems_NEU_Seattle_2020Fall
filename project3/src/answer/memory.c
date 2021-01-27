#include "thread.h"
#include "utils.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

// 4k is the size of a page
const int PAGE_SIZE = 4 * 1024;
// A total of 8M exists in memory
const int ALL_MEM_SIZE = 8 * 1024 * 1024;
// USER Space starts at 1M
const int USER_BASE_ADDR = 1024 * 1024;
// Stack starts at 8M and goes down to 6M
const int STACK_END_ADDR = 6 * 1024 * 1024;
// There are total of 2048 pages
const int NUM_PAGES = 2 * 1024;

const int KERNEL_PAGES = 256;

#define CACHE_FILENAME ".cache_%d_%d"

typedef struct {
    // accessed flag of frame page to represent if memory is referenced and frame is evicted
    bool accessed;
    // dirty flag of frame page to represent if reference is a "write"
    bool dirty;
    int vpn;
    Thread *thread;
} IPTE;

IPTE *InvertedPageTable = NULL;

// The clock hand points to the oldest page
// When a page fault occurs, the frame being pointed to by the hand is inspected
int clock_hand = 0;

char **PageFrames = NULL;

// A mutex is used when a thread tries to allocate a new frame, so it will avoid races
pthread_mutex_t mem_mutex = PTHREAD_MUTEX_INITIALIZER;

void initMemory() {
    // This inverted page table is an array of IPTE with size NUM_PAGES
    // to find a frame that has been allocated to which thread
    InvertedPageTable = calloc(NUM_PAGES, sizeof(IPTE));
    PageFrames = calloc(NUM_PAGES, sizeof(char *));
    for (int i = 0; i < NUM_PAGES; i++) {
        PageFrames[i] = calloc(PAGE_SIZE, sizeof(char));
        // accessed flag initialized as false
        InvertedPageTable[i].accessed = false;
        // dirty flag initialized as false
        InvertedPageTable[i].dirty = false;
        InvertedPageTable[i].vpn = -1;
        InvertedPageTable[i].thread = NULL;
    }
    clock_hand = 0;
}

void freeMemory() {
    free(InvertedPageTable);
    for (int i = 0; i < NUM_PAGES; i++) {
        free(PageFrames[i]);
    }
    free(PageFrames);
}

// This function use clock page replacement algo to evict a frame from the frame table
int evictPageFrame() {
    for (;;) {
        if (InvertedPageTable[clock_hand].accessed == 0)
            // this process is repeated until a frame is found with accessed if false
            break;
        InvertedPageTable[clock_hand].accessed = 0;
        // frame is evicted and the clock hand is advanced one position
        clock_hand++;
        clock_hand %= (NUM_PAGES - KERNEL_PAGES);
    }
    // if accessed flag is true, it is cleared and the hand is advanced to the next page
    return clock_hand + KERNEL_PAGES;
}

void saveCacheFile(Thread *thread, int vpn, void *buf) {
    char filename[BUFSIZ];
    snprintf(filename, sizeof(filename), CACHE_FILENAME, thread->threadId, vpn);
    FILE *file = fopen(filename, "wb");
    fwrite(buf, PAGE_SIZE, 1, file);
    fclose(file);
}

void loadCacheFile(Thread * thread, int vpn, void *buf) {
    char filename[BUFSIZ];
    snprintf(filename, sizeof(filename), CACHE_FILENAME, thread->threadId, vpn);
    FILE *file = fopen(filename, "rb");
    if (file) {
        fread(buf, PAGE_SIZE, 1, file);
        fclose(file);
    }
}

void removeCacheFile(Thread *thread) {
    char filename[BUFSIZ];
    for (int vpn = 0; vpn < NUM_PAGES; vpn++) {
        snprintf(filename, sizeof(filename), CACHE_FILENAME, thread->threadId, vpn);
        unlink(filename);
    }
}

void freePageFrames(Thread *thread) {
    pthread_mutex_lock(&mem_mutex);
    for (int vpn = 0; vpn < NUM_PAGES; vpn++) {
        int pfn = thread->PFN[vpn];
        // if PFN[vpn] is not -1, there is frame associated with the virtual page
        if (pfn != -1) {
            InvertedPageTable[pfn].thread = NULL;
        }
    }
    pthread_mutex_unlock(&mem_mutex);
}

int allocatePageFrame(Thread *thread, int vpn) {
    int pfn = KERNEL_PAGES;
    for (; pfn < NUM_PAGES; pfn++) {
        if (InvertedPageTable[pfn].thread == NULL) {
            break;
        }
    }
    if (pfn == NUM_PAGES) {
        pfn = evictPageFrame();
        if (InvertedPageTable[pfn].dirty && InvertedPageTable[pfn].thread) {
            InvertedPageTable[pfn].thread->PFN[InvertedPageTable[pfn].vpn] = -1;
            saveCacheFile(InvertedPageTable[pfn].thread, InvertedPageTable[pfn].vpn,
                          PageFrames[pfn]);
        }
    }
    loadCacheFile(thread, vpn, PageFrames[pfn]);
    InvertedPageTable[pfn].thread = thread;
    InvertedPageTable[pfn].vpn = vpn;
    InvertedPageTable[pfn].accessed = false;
    InvertedPageTable[pfn].dirty = false;
    thread->PFN[vpn] = pfn;
    return pfn;
}

int allocateHeapMem(Thread *thread, int size) {
    if (size <= 0 || thread->heap_ptr + size > STACK_END_ADDR)
        return -1;
    int addr = thread->heap_ptr;
    thread->heap_ptr += size;
    return addr;
}

int allocateStackMem(Thread *thread, int size) {
    if (size <= 0 || thread->stack_ptr - size < STACK_END_ADDR - 1)
        return -1;
    thread->stack_ptr -= size;
    int addr = thread->stack_ptr;
    return addr;
}

void writeToAddr(Thread *thread, int addr, int size, const void *data) {
    pthread_mutex_lock(&mem_mutex);
    int _addr = addr;
    int _size = size;
    const char *ptr = data;
    for (; size > 0 && ((addr >= USER_BASE_ADDR && addr < thread->heap_ptr) ||
                        (addr >= thread->stack_ptr && addr < ALL_MEM_SIZE));
           addr++, size--) {
        int vpn = addr / PAGE_SIZE;
        int pfn = thread->PFN[vpn];
        int offset = addr % PAGE_SIZE;
        if (pfn == -1) {
            pfn = allocatePageFrame(thread, vpn);
        }
        // memory is referenced, set accessed flag to true
        InvertedPageTable[pfn].accessed = true;
        // reference is a wirte, dirty flag to true
        InvertedPageTable[pfn].dirty = true;
        PageFrames[pfn][offset] = *ptr++;
    }
    pthread_mutex_unlock(&mem_mutex);
    if (size > 0)
        kernelPanic(thread, addr);
}

void readFromAddr(Thread *thread, int addr, int size, void *outData) {
    pthread_mutex_lock(&mem_mutex);
    char *ptr = outData;
    for (; size > 0 && ((addr >= USER_BASE_ADDR && addr < thread->heap_ptr) ||
                        (addr >= thread->stack_ptr && addr < ALL_MEM_SIZE));
           addr++, size--) {
        int vpn = addr / PAGE_SIZE;
        int pfn = thread->PFN[vpn];
        int offset = addr % PAGE_SIZE;
        if (pfn == -1) {
            pfn = allocatePageFrame(thread, vpn);
        }
        InvertedPageTable[pfn].accessed = true;
        *ptr++ = PageFrames[pfn][offset];
    }
    pthread_mutex_unlock(&mem_mutex);
    if (size > 0)
        kernelPanic(thread, addr);
}

char *getCacheFileName(Thread *thread, int addr) {
    static char filename[BUFSIZ];
    int vpn = addr / PAGE_SIZE;
    snprintf(filename, sizeof(filename), ".cache_%d_%d", thread->threadId, vpn);
    return filename;
}
