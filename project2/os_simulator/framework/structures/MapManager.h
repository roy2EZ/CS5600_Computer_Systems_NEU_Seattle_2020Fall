#ifndef FRAMEWORK_MAPMANAGER_H
#define FRAMEWORK_MAPMANAGER_H

#include "StructureManager.h"

template <class U>
class MapManager : public StructureManager<map<U, void*>> {
   private:
    static MapManager<U>* singleton;
    MapManager();
    ~MapManager();

   public:
    static MapManager<U>* getInstance();
    void put(const char* mapIdentifier, U key, void* value);
    void* remove(const char* mapIdentifier, U key);
    void* get(const char* mapIdentifier, U key);
    bool contains(const char* mapIdentifier, U key);
    void execOnMap(const char* mapIdentifier, void (*func)(U, void*));
};

template <class U>
void MapManager<U>::put(const char* mapIdentifier, U key, void* value) {
    if (this->structures.find(mapIdentifier) == this->structures.end())
        return;
    pthread_mutex_lock(&this->structures[mapIdentifier].second);
    this->structures[mapIdentifier].first[key] = value;
    pthread_mutex_unlock(&this->structures[mapIdentifier].second);
}

template <class U>
void* MapManager<U>::remove(const char* mapIdentifier, U key) {
    if (this->structures.find(mapIdentifier) == this->structures.end())
        return NULL;
    pthread_mutex_lock(&this->structures[mapIdentifier].second);
    void* ret = this->structures[mapIdentifier].first[key];
    this->structures[mapIdentifier].first.erase(key);
    pthread_mutex_unlock(&this->structures[mapIdentifier].second);
    return ret;
}

template <class U>
void* MapManager<U>::get(const char* mapIdentifier, U key) {
    if (this->structures.find(mapIdentifier) == this->structures.end())
        return NULL;
    pthread_mutex_lock(&this->structures[mapIdentifier].second);
    void* ret = this->structures[mapIdentifier].first[key];
    pthread_mutex_unlock(&this->structures[mapIdentifier].second);
    return ret;
}

template <class U>
MapManager<U>::MapManager() : StructureManager<map<U, void*>>() {}

template <class U>
MapManager<U>::~MapManager() {
    if (singleton != NULL) {
        delete singleton;
    }
}

template <class U>
MapManager<U>* MapManager<U>::getInstance() {
    if (singleton == NULL) {
        singleton = new MapManager<U>();
    }
    return singleton;
}

template <class U>
bool MapManager<U>::contains(const char* mapIdentifier, U key) {
    if (this->structures.find(mapIdentifier) == this->structures.end())
        return false;
    pthread_mutex_lock(&this->structures[mapIdentifier].second);
    bool ret = this->structures[mapIdentifier].first.find(key) !=
               this->structures[mapIdentifier].first.end();
    pthread_mutex_unlock(&this->structures[mapIdentifier].second);
    return ret;
}

template <class U>
void MapManager<U>::execOnMap(const char* mapIdentifier,
                              void (*func)(U, void*)) {
    if (this->structures.find(mapIdentifier) == this->structures.end())
        return;
    pthread_mutex_lock(&this->structures[mapIdentifier].second);
    for (auto iter = this->structures[mapIdentifier].first.begin();
         iter != this->structures[mapIdentifier].first.end(); iter++) {
        func(iter->first, iter->second);
    }
    pthread_mutex_unlock(&this->structures[mapIdentifier].second);
}

template <class U>
MapManager<U>* MapManager<U>::singleton = NULL;
#endif  // FRAMEWORK_MAPMANAGER_H
