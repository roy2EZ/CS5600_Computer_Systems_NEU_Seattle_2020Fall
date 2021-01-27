#ifndef FRAMEWORK_STRUCTUREMANAGER_H
#define FRAMEWORK_STRUCTUREMANAGER_H

// TODO: Factor this out into a separate header instead of duplicating it.
#ifndef UUID_LENGTH
#define UUID_LENGTH 37
#endif

#include <uuid/uuid.h>
#include <map>

using namespace std;

template <class T>
class StructureManager {
   protected:
    map<const char*, pair<T, pthread_mutex_t>> structures;
    pthread_mutex_t mapMutex;
    StructureManager();
    virtual ~StructureManager();

   public:
    virtual char* create();
    virtual void destroy(const char* identifier);
    virtual int size(const char* identifier);
};

template <class T>
StructureManager<T>::StructureManager() {
    pthread_mutex_init(&mapMutex, NULL);
}

template <class T>
StructureManager<T>::~StructureManager() {
    pthread_mutex_destroy(&mapMutex);
    for (auto iter = structures.begin(); iter != structures.end(); iter++) {
        pthread_mutex_destroy(&(iter->second.second));
    }
}

template <class T>
char* StructureManager<T>::create() {
    uuid_t uuid;
    uuid_generate(uuid);
    char* idPtr = new char[UUID_LENGTH]();
    uuid_unparse(uuid, idPtr);

    pthread_mutex_lock(&mapMutex);
    pthread_mutex_t innerMutex;
    pthread_mutex_init(&innerMutex, NULL);
    structures.insert(make_pair(idPtr, make_pair(T(), innerMutex)));
    pthread_mutex_unlock(&mapMutex);
    return idPtr;
}

template <class T>
void StructureManager<T>::destroy(const char* identifier) {
    pthread_mutex_lock(&mapMutex);
    structures.erase(identifier);
    pthread_mutex_unlock(&mapMutex);
    delete[] identifier;
}

template <class T>
int StructureManager<T>::size(const char* identifier) {
    if (structures.find(identifier) == structures.end())
        return 0;
    pthread_mutex_lock(&structures[identifier].second);
    int ret = structures[identifier].first.size();
    pthread_mutex_unlock(&structures[identifier].second);
    return ret;
}

#endif  // FRAMEWORK_STRUCTUREMANAGER_H
