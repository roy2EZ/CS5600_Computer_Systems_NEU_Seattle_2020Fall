# Project 2 Questions

## Directions

Answer each question to the best of your ability; feel free to copy sections of
your code or link to it if it helps explain but answers such as "this line
guarantees that the highest priority thread is woken first" are not sufficient.

Your answers can be relatively short - around 100 words, and no more than 200,
should suffice per subsection.

To submit, edit this file and replace every `ANSWER_BEGIN` to `ANSWER_END` with
your answer. Your submission should not include **any** instances of
`ANSWER_BEGIN` or `ANSWER_END`; the text provided is meant to give you some tips
on how to begin answering.

For one point, remember to delete this section (Directions) before submitting.

## Scheduling

**Two tests have infinite loops with the scheduler you were provided. Explain
why `Running.PriorityOrder` will never terminate with the basic scheduler.**

The provided scheduler called `createAndSetThreadToRun` to create new thread,
the created thread is added to the ready list by calling `addToList`, which just
insert the new thead at the end of the ready list.

The `Running.PriorityOrder` test first created a Lo Pri thread, which executes
the function `recordThreadPriority`. When the created Lo Pri thread called this function,
it will check the argument passed to this function, which is a pointer to a `PriorityInfoList` variable, this structure has a `tcbiList` member, which is a pointer array, each element in this array pointed to one `ThreadCallbackInfo` object.

When the Lo Pri thread was created, the argument passed to the thread function is `PriorityInfoList* infoList`, its `tcbiList` member had already two elements, respectively are `midCallback` and `hiCallback`. These two `ThreadCallbackInfo` objects contained informations used to create new thread.

To understand the reason why the test `Running.PriorityOrder` looped infinitely, we need understand what the function `recordThreadPriorty` did. 

* The Lo Pri thread check the `PriorityInfoList` which has two `ThreadCallbackInfo` and no threads correspond to these two `ThreadCallbackInfo` has been created, so it will create a new thead correspond to the first `ThreadCallbackInfo midCallback`.

* After that, the Lo Pri thread will wait for the new create thead do the same things as it,
until all the `ThreadCallbackInfo` in the `PriorityInfoList` has been used to create a thread. It check the `cont` member, when all threads have been created, this member will be set to true.

* The new created Md Pri thread should did the same thing as the Lo Pri thread, since it has higher priority than the Lo Pri thread, so it should run from the ready list, but the scheduler did not worked as expected, it called the `nextThreadToRun` function to pick a thread in the ready list, which just pick the first thread in the ready list, so the scheduler always pick the Lo Pri thread to run, and the Md Pri thread never has a chance to run, this resulted that the Lo Pri thread looped forver that wait other threads to be created and check the `cont` member to be set true.

## Sleep

**Briefly describe what happens in your `sleep(...)` function.**

1. call `getCurrentThread` to get the current `Thread` object
2. call `getCurrentTick` to record the ticks when sleep called
3. put this thread object into a map called `sleepMap`
4. call `stopExecutingThreadForCycle`, this will stop executing the current thread for the rest of this cycle. (see the comments in Thread.h)
5. after `stopExecutingThreadForCycle` returned, return the tickes when sleep called

**How does your `sleep(...)` function guarantee duration?**

The `sleepMap` stored the sleep information for thread, so when called `nextThreadToRun`, if thread was selected to run, it will check the currentTick with the ticks stored at `sleepMap`, only when thread can wake up, it will be selectd as the next thread to run, otherwise, a lower priority thread will be selected.

```cpp
bool inmap = MAP_CONTAINS(Thread*, sleepMap, ret);
if (inmap) {
    SleepInfo *info = (SleepInfo *)GET_FROM_MAP(Thread*, sleepMap, ret);
    if (info->sleepTick + info->numTicks <= currentTick) {
        REMOVE_FROM_MAP(Thread*, sleepMap, ret);
    } else {
        ret = NULL;
        threadIndex++;
    }
} 
```

**Do you have to check every sleeping thread every tick? Why not?**

No, The scheduler did not check every sleeping thread, it only check the highest priority thread.
If a higher priority thead is selected to run, there is no need to check lower priority thread.

## Locking

**How do you ensure that the highest priority thread waiting for a lock gets it
first?**

Since the theads in the readyList are sorted by priority, and the scheduler alway select the threads list head so the highest prioriy thread will get the lock first.
 
## Priority Donation

**Describe the sequence of events before and during a priority donation.**

1. The thread calls the `lock` function in the framework to request a lock
2. The `lock` function calls the `lockAttempted` before waiting for the lock
3. At the `lockAttempted` function, we will check whether the `lockId` is holding by another thread, if so, we will has a chance to do a priority donation

**How is nested donation handled?**
1. Three theads A, B, C have priority from low to high
2. A holds lock 1, B holds lock 2, C holds lock 3
3. B request lock 1, then A's priority will boost to B's priority
4. C request lock 2, then B's priority will boost to C's priority, at the same time, B knows it is blocked by the lock 1, and it can find the thread A holds the block 1, so it will also boost A's prioriy to C's.

```cpp
Thread *holdThread = (Thread *)GET_FROM_MAP(const char *, lockHoldMap, lockId);
if (holdThread && holdThread->priority < thread->priority) {
    holdThread->priority = thread->priority;
    // nest donation
    bool inmap = MAP_CONTAINS(Thread*, blockedMap, holdThread);
    if (inmap) {
        const char *lockId2 = (const char *)GET_FROM_MAP(Thread*, blockedMap, holdThread);
        inmap = MAP_CONTAINS(const char*, lockHoldMap, lockId2);
        if (inmap) {
            Thread *holdThread2 = (Thread *)GET_FROM_MAP(const char*, lockHoldMap, lockId2);
            if (holdThread2->priority < holdThread->priority) {
                holdThread2->priority = holdThread->priority;
            }
        }
    }
}
```

**How do you ensure that the highest priority thread waiting is called when the
lock is released?**

Since the scheduler selects the threads from higher priority to lower in the ready list, when the lock
is released the highest priority thread should be selected to run.