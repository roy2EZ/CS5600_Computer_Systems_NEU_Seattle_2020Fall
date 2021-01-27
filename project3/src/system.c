#include <pthread.h>
#include <string.h>
#include "callback.h"
#include "utils.h"
#include "system.h"

extern pthread_mutex_t loggerMutex;
extern const char logBuffer[];
extern const int PAGE_SIZE;
extern const int MAX_BUFFER_SIZE;
pthread_mutex_t loggerMutex;


void systemInit() {
    pthread_mutex_init(&loggerMutex, NULL);
    bzero(logBuffer, MAX_BUFFER_SIZE);
    startupCallback();
}

void systemShutdown() {
    shutdownCallback();
    flushLog();
    bzero(&logBuffer, MAX_BUFFER_SIZE);
    pthread_mutex_destroy(&loggerMutex);
}
