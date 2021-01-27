#ifndef STRUCTURES_LISTMANAGER_H
#define STRUCTURES_LISTMANAGER_H

#include <map>
#include <vector>
#include "StructureManager.h"

using namespace std;

class ListManager : public StructureManager<vector<void*>> {
   private:
    static ListManager* singleton;
    ListManager();
    ~ListManager();

   public:
    static ListManager* getInstance();
    void add(const char* listIdentifier, void* item);
    void add(const char* listIdentifier, int index, void* item);
    void remove(const char* listIdentifier, void* item);
    void* remove(const char* listIdentifier, int index);
    void* get(const char* listIdentifier, int index);
    void* get(const char* list, void* item);
    void sort(const char* listIdentifier, bool (*func)(void*, void*));
};

#endif  // STRUCTURES_LISTMANAGER_H
