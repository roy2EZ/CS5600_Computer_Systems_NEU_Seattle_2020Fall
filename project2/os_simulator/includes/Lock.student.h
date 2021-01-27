#ifndef _INCLUDED_FROM_LOCK_H
#error This should never be included directly; #include Lock.h instead!
#endif

// The functions below here are functions that are called by the framework when
// an event happens. You can use them to modify your state when a lock is
// manipulated.

/**
 * This function is called right after createLock in order to notify anyone that
 * is interested in keeping state of locks. Primarily this can be used for
 * initializing any structures needed to map locks to threads.
 *
 * @param lockId The lock id of the lock that was created.
 */
void lockCreated(const char* lockId);

/**
 * This function is called when an attempt is made on a lock but has not yet
 * been locked. This function is called synchronously during the locking
 * process so you can be guaranteed the lock will not be acquired by the given
 * thread while in this function.
 *
 * @param lockId The lock id of the lock attempted.
 * @param thread The thread that attempted the lock.
 */
void lockAttempted(const char* lockId, Thread* thread);

/**
 * This function is called when a lock is acquired. This function is called
 * synchronously during the locking process so you can be guaranteed the lock
 * will not be released by the given thread while in this function.
 *
 * @param lockId The lock id of the lock acquired.
 * @param thread The thread that acquired the lock.
 */
void lockAcquired(const char* lockId, Thread* thread);

/**
 * This function is called when acquiring a lock fails. This function is called
 * synchronously during the locking process.
 *
 * @param lockId The lock id of the lock acquired.
 * @param thread The thread that failed to acquire the lock.
 */
void lockFailed(const char* lockId, Thread* thread);

/**
 * This function is called when a lock is released. This function is called
 * synchronously during the locking process so you can be guaranteed the lock
 * will not be acquired again by the given thread while in this function.
 *
 * @param lockId The lock id of the lock released.
 * @param thread The thread that acquired the lock.
 */
void lockReleased(const char* lockId, Thread* thread);

// The function below must be implemented.

/**
 * Returns a pointer to the thread that is currently holding the given lock.
 * @param lockId The lock id of the lock we are interested in.
 * @return A pointer to the thread holding the given lock.
 */
Thread* getThreadHoldingLock(const char* lockId);