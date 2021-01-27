#ifndef _THREAD_LOCK_H
#define _THREAD_LOCK_H

// You can use this header to share data and/or definitions between your
// lock.cpp and thread.cpp.

// For example, if you wanted to share a map between the two...

extern const char* readyList;
extern const char* lockHoldMap;
extern const char* blockedMap;

bool compThreadPrior(void* t1, void* t2);

// Then in ONE OF lock.cpp or thread.cpp you would write

// const char *sharedMap = CREATE_MAP(const char*);

// Now both lock.cpp and thread.cpp may refer to sharedMap.

#endif