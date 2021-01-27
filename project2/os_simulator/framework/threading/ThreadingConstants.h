#ifndef OS_THREADING_THREADINGCONSTANTS_H
#define OS_THREADING_THREADINGCONSTANTS_H

#include <time.h>

namespace Threading {
static const int MICROSECONDS_TICK = 50000;
static const struct timespec THREAD_JOIN_TIMEOUT = {0,
                                                    MICROSECONDS_TICK * 1000};
}  // namespace Threading
#endif  // OS_THREADING_THREADINGCONSTANTS_H
