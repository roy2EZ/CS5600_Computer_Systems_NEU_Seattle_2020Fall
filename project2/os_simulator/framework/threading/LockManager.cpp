#include "ThreadManager.h"

#include <uuid/uuid.h>

// TODO: Factor this out into a separate header instead of duplicating it.
#ifndef UUID_LENGTH
// 36 bytes and a null character
#define UUID_LENGTH 37
#endif

using namespace Threading;

LockManager* LockManager::singleton = NULL;

const char* LockManager::createLock() {
    uuid_t uuid;
    uuid_generate(uuid);
    char* id = new char[UUID_LENGTH]();
    uuid_unparse(uuid, id);
    pthread_mutex_t* mutex = new pthread_mutex_t();
    pthread_mutex_init(mutex, NULL);
    locks[id] = mutex;
    lockCreated(id);
    return id;
}

bool LockManager::lock(const char* lockId) {
    Thread* currentThread =
        ThreadManager::getInstance()->currentThread()->getExternalThread();
    if (locks.count(lockId) == 1) {
        lockAttempted(lockId, currentThread);
        if (pthread_mutex_lock(locks[lockId]) == 0) {
            lockAcquired(lockId, currentThread);
            return true;
        }
    }
    lockFailed(lockId, currentThread);
    return false;
}

bool LockManager::unlock(const char* lockId) {
    if (locks.count(lockId) == 1) {
        if (pthread_mutex_unlock(locks[lockId]) == 0) {
            lockReleased(lockId, ThreadManager::getInstance()
                                     ->currentThread()
                                     ->getExternalThread());
            return true;
        }
    }
    return false;
}

LockManager::LockManager() {}

LockManager::~LockManager() {
    for (map<const char*, pthread_mutex_t*>::iterator iter =
             singleton->locks.begin();
         iter != singleton->locks.end(); iter++) {
        pthread_mutex_destroy(iter->second);
        delete iter->second;
        delete iter->first;
    }
    LockManager::singleton = NULL;
}

LockManager* LockManager::getInstance() {
    if (!singleton) {
        singleton = new LockManager();
    }
    return singleton;
}

void LockManager::destroyLock(const char* lockId) {
    if (locks.count(lockId) == 1) {
        pthread_mutex_destroy(locks[lockId]);
        locks.erase(lockId);
    }
}

bool LockManager::lockExists(const char* lockId) {
    return locks.count(lockId) == 1;
}

bool LockManager::isLocked(const char* lockId) {
    bool ret = false;
    if (locks.count(lockId) == 1) {
        int tryLock = pthread_mutex_trylock(locks[lockId]);
        ret = tryLock != 0;
        if (tryLock == 0) {
            pthread_mutex_unlock(locks[lockId]);
        }
    }

    return ret;
}

const char* createLock() {
    return LockManager::getInstance()->createLock();
}

bool lock(const char* lockId) {
    return LockManager::getInstance()->lock(lockId);
}

bool unlock(const char* lockId) {
    return LockManager::getInstance()->unlock(lockId);
}

void destroyLock(const char* lockId) {
    LockManager::getInstance()->destroyLock(lockId);
}

bool isLocked(const char* lockId) {
    return LockManager::getInstance()->isLocked(lockId);
}

bool lockExists(const char* lockId) {
    return LockManager::getInstance()->lockExists(lockId);
}