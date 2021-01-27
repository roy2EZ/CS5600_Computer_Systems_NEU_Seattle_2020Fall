#include "ListManager.h"
#include "List.h"

#include <algorithm>

ListManager* ListManager::singleton = NULL;

ListManager::ListManager() : StructureManager() {}

ListManager::~ListManager() {
    if (singleton != NULL) {
        delete singleton;
    }
}

const char* createNewList() {
    return ListManager::getInstance()->create();
}

void destroyList(const char* listIdentifier) {
    ListManager::getInstance()->destroy(listIdentifier);
}

void addToList(const char* listIdentifier, void* item) {
    ListManager::getInstance()->add(listIdentifier, item);
}

void addToListAtIndex(const char* listIdentifier, int index, void* item) {
    ListManager::getInstance()->add(listIdentifier, index, item);
}

void removeFromList(const char* listIdentifier, void* item) {
    ListManager::getInstance()->remove(listIdentifier, item);
}

void* removeFromListAtIndex(const char* listIdentifier, int index) {
    return ListManager::getInstance()->remove(listIdentifier, index);
}

int listSize(const char* listIdentifier) {
    return ListManager::getInstance()->size(listIdentifier);
}

void* listGet(const char* listIdentifier, int index) {
    return ListManager::getInstance()->get(listIdentifier, index);
}

void* listGet(const char* listIdentifier, void* item) {
    return ListManager::getInstance()->get(listIdentifier, item);
}

void sortList(const char* listIdentifier, bool (*func)(void*, void*)) {
    ListManager::getInstance()->sort(listIdentifier, func);
}

void ListManager::add(const char* listIdentifier, void* item) {
    if (structures.find(listIdentifier) == structures.end())
        return;
    pthread_mutex_lock(&this->structures[listIdentifier].second);
    structures[listIdentifier].first.push_back(item);
    pthread_mutex_unlock(&this->structures[listIdentifier].second);
}

void ListManager::add(const char* listIdentifier, int index, void* item) {
    if (structures.find(listIdentifier) == structures.end())
        return;
    pthread_mutex_lock(&this->structures[listIdentifier].second);
    vector<void*>* list = &(structures[listIdentifier].first);
    list->insert(list->begin() + index, item);
    pthread_mutex_unlock(&this->structures[listIdentifier].second);
}
void ListManager::remove(const char* listIdentifier, void* item) {
    if (structures.find(listIdentifier) == structures.end())
        return;
    pthread_mutex_lock(&this->structures[listIdentifier].second);
    vector<void*>* list = &(this->structures[listIdentifier].first);
    list->erase(std::remove(list->begin(), list->end(), item), list->end());
    pthread_mutex_unlock(&this->structures[listIdentifier].second);
}

void* ListManager::remove(const char* listIdentifier, int index) {
    if (structures.find(listIdentifier) == structures.end())
        return NULL;
    pthread_mutex_lock(&structures[listIdentifier].second);
    vector<void*>* list = &(structures[listIdentifier]).first;
    void* ret = (*list)[index];
    list->erase(list->begin() + index);
    pthread_mutex_unlock(&structures[listIdentifier].second);
    return ret;
}

ListManager* ListManager::getInstance() {
    if (singleton == NULL) {
        singleton = new ListManager();
    }
    return singleton;
}

void* ListManager::get(const char* listIdentifier, int index) {
    void* ret = NULL;
    if (structures.find(listIdentifier) != structures.end()) {
        pthread_mutex_lock(&this->structures[listIdentifier].second);
        ret = structures[listIdentifier].first[index];
        pthread_mutex_unlock(&this->structures[listIdentifier].second);
    }
    return ret;
}

void* ListManager::get(const char* listIdentifier, void* item) {
    void* ret = NULL;
    if (structures.find(listIdentifier) != structures.end()) {
        pthread_mutex_lock(&this->structures[listIdentifier].second);
        vector<void*>::iterator iter =
            find(structures[listIdentifier].first.begin(),
                 structures[listIdentifier].first.end(), (item));
        if (iter != structures[listIdentifier].first.end()) {
            ret = *iter;
        }
        pthread_mutex_unlock(&this->structures[listIdentifier].second);
    }
    return ret;
}

void ListManager::sort(const char* listIdentifier, bool (*func)(void*, void*)) {
    pthread_mutex_lock(&this->structures[listIdentifier].second);
    std::sort(structures[listIdentifier].first.begin(),
              structures[listIdentifier].first.end(), func);
    pthread_mutex_unlock(&this->structures[listIdentifier].second);
}
