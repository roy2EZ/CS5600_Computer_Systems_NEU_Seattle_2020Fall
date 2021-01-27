#ifndef FRAMEWORK_LOCK_H
#define FRAMEWORK_LOCK_H

#include "Thread.h"

// The following functions are implemented for your use as a student, you are
// not required to implement them, just use them as needed.

/**
 * Creates a lock, the lock is a UUID that is a unique string.
 * @return A unique string that represents a lock.
 */
const char* createLock();

/**
 * Locks a given lock, if this lock is locked any thread attempting to lock this
 * lock will wait until the other thread unlocks this lock. This can result in
 * a deadlock if two threads end up waiting on each other. If a thread locks a
 * lock, it is responsible for unlocking when it is done.
 * @param lockId The lock id of the lock to be locked.
 * @return true if successful, false otherwise
 */
bool lock(const char* lockId);

/**
 * Unlocks a given lock, this allows another thread requesting the lock to
 * receive the lock. This function must be called if a lock is locked,
 * otherwise another thread could be blocked.
 * @param lockId The lock id of the lock to be unlocked.
 * @return true if successful, false otherwise
 */
bool unlock(const char* lockId);

/**
 * Destroys a given lock, this lock can no longer be used.
 * @param lockId The lock id of the lock to be destroyed.
 */
void destroyLock(const char* lockId);

/**
 * Returns true if this lock is currently held by any thread. This will not
 * tell you which thread currently holds the lock. Maintaining that mapping
 * will fall on the student.
 * @param lockId  The lock id of the lock to be checked.
 * @return true if successful, false otherwise
 */
bool isLocked(const char* lockId);

/**
 * Returns true if this lock currently exists regardless of state. If the lock
 * provided was created but never destroyed, this will return true. If the lock
 * id does not exist it will return false.
 * @param lockId The lock id of the lock to be checked.
 * @return true if lock exists, false otherwise.
 */
bool lockExists(const char* lockId);

#define _INCLUDED_FROM_LOCK_H
#include "Lock.student.h"
#undef _INCLUDED_FROM_LOCK_H

#endif  // FRAMEWORK_LOCK_H
