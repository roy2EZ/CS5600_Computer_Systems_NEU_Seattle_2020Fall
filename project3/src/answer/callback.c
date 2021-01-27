#include "callback.h"

extern void initMemory();
extern void freeMemory();

void startupCallback() {
    initMemory();
}

void shutdownCallback() {
    freeMemory();
}
