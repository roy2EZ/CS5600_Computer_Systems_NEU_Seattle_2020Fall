#ifndef OS_THREADING_IOMANAGER_H
#define OS_THREADING_IOMANAGER_H

#include <sstream>
#include "threading/ThreadManager.h"

using namespace std;

class InternalLogger {
    friend class Threading::ThreadManager;

   private:
    stringstream* logStream;
    static InternalLogger* instance;
    InternalLogger();
    ~InternalLogger();
    static void init();
    pthread_mutex_t streamMutex;
    bool verbose;
    unsigned int tick = 0;

   public:
    bool isVerbose();
    void setVerbose(bool val);
    void setTick(int val);
    void flush();
    static InternalLogger& getLogger();
    static InternalLogger& eventSink();
    template <typename T>
    friend InternalLogger& operator<<(InternalLogger&, T output);
};

template <typename T>
InternalLogger& operator<<(InternalLogger& logger, T output) {
    pthread_mutex_lock(&(logger.streamMutex));
    *(logger.logStream) << output;
    pthread_mutex_unlock(&(logger.streamMutex));
    return logger;
}

#endif  // OS_THREADING_IOMANAGER_H
