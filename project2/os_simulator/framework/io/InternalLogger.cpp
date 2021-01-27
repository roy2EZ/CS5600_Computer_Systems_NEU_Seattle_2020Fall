#include "InternalLogger.h"
#include <iomanip>
#include <iostream>
#include "Thread.h"

InternalLogger* InternalLogger::instance = NULL;

InternalLogger::InternalLogger() {
    logStream = new stringstream();
    pthread_mutex_init(&streamMutex, NULL);
    verbose = false;
    tick = 0;
}

InternalLogger::~InternalLogger() {
    delete logStream;
}

void InternalLogger::init() {
    instance = new InternalLogger();
}

InternalLogger& InternalLogger::getLogger() {
    return *instance;
}

InternalLogger& InternalLogger::eventSink() {
    // Save fill so we can restore it afterwards
    char fill = (*instance).logStream->fill('0');
    return (*instance) << std::setw(5) << instance->tick << std::setfill(fill)
                       << " ";
}

void InternalLogger::flush() {
    pthread_mutex_lock(&streamMutex);
    stringstream* stream = getLogger().logStream;
    cout << stream->str();
    cout.flush();
    stream->str(std::string());
    pthread_mutex_unlock(&streamMutex);
}

bool InternalLogger::isVerbose() {
    return verbose;
}

void InternalLogger::setVerbose(bool val) {
    verbose = val;
}

void InternalLogger::setTick(int val) {
    tick = val;
}

void logLine(const char* str) {
    InternalLogger::eventSink() << str << "\n";
}

void log(const char* str) {
    InternalLogger::eventSink() << str;
}

void verboseLog(const char* str) {
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink() << str;
        InternalLogger::getLogger().flush();
    }
}

void setVerbose(bool val) {
    InternalLogger::getLogger().setVerbose(val);
}

std::ostream& operator<<(std::ostream& out, const State value) {
    const char* str = 0;
#define TEST_VALUE(v) \
    case (v):         \
        str = #v;     \
        break;
    switch (value) {
        TEST_VALUE(CREATED);
        TEST_VALUE(PAUSED);
        TEST_VALUE(RUNNING);
        TEST_VALUE(TERMINATED);
    }
#undef TEST_VALUE

    return out << str;
}