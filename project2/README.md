# CS 5600 Assignment 2

## Overview

This assignment is built around a simulated processor. This processor creates
slices of approximately 50μs and switches threads each slice to simulate
interrupt-driven process scheduling. Your task is to implement scheduling,
sleep, and priority donation.

## KNOWN ISSUES

1. The simulator does not work reliably when run on VMs with multiple cores.
   Note that the VirtualBox default is one core.
2. The simulator does not work under WSL on multi-core systems. This is likely
   related to the other multi-core issue but the failure rate is 100% on WSL
   and only 50% on a four-core VirtualBox system.

## STOP HERE

This assignment is _significantly_ more difficult than the first assignment. It
isn't quite impossible to debug but unless you're familiar with how to debug
multithreaded applications you will have a great deal of trouble doing so.

As you saw in assignment 1, the compiler will not help you as much as you would
like. In order to keep you from going too far in a bad direction, this
assignment contains several directions that _must_ be followed exactly. To help
encourage everyone to read every comment and every document, there are
incentives in a few places. For example, when you submit this assignment for
grading, mention a sequel - book, movie, game, anything goes - in the title for
one bonus point.

The negative incentives are clearly spelled out and are not meant as traps. The
bonus points, being positives, are deliberately obnoxious to find. Your time is
better spent reading documentation. Please do not share the location of the
points with others in the class. All of the bonus points are questions or
phrases that must be answered/placed in the issue you submit for grading.

Before you write any code, review `answer/QUESTIONS.md` and keep it in mind as
you design a solution. You should be able to fill it out as you write your
code - it also has several hints about how you will want to handle certain
features.

The simulator framework has been provided as `os_simulator/framework/` but you
should not need to look at it for any part of this assignment.

Take a moment to make sure you've read the second paragraph of this section
before continuing.

## Goals

- In `answer/QUESTIONS.md`:
  - Several questions about your code and the general problem must be answered
  - The questions (and hints in the doc) will help guide your solution
- In `answer/thread.cpp`:
  - A basic scheduler has been provided; extend and/or replace it
  - Some functions have empty implementations. You must implement them
- In `answer/lock.cpp`:
  - You _must_ implement `Thread* getThreadHoldingLock(const char*)`
  - You _may_ take advantage of the other functions; it is probably impossible
    to get full marks without using all of them in some fashion
- In `answer/test_config.h`:
  - Two tests have been disabled **AND MUST BE RE-ENABLED BEFORE GRADING**
  - For more details, read `answer/QUESTIONS.md`
- Ensure that your solution passes all of the included tests
- Ensure that your solution does not leak any memory
- Ensure that your solution is _cleanly formatted_ and _well-documented_ as the
  tests are a smaller part of your grade than before
- No, seriously, look at `answer/QUESTIONS.md` early and often

### Requirements

#### General

- Read _every_ document and comment.
- Document any interesting/difficult sections of your code
- Use this project's code style
  - a `.clang-format` configuration file has been provided
  - `sudo apt install -y clang-format` in your Ubuntu machine
  - `clang-format -i <one or more files>` to format them in place
  - No exceptions this time around, apologies
  - You can configure VS Code and CLion to match this code style by importing
    the Chromium definitions
- Your code must compile without compiler warnings or errors
- Your code must be C, not C++
  - The simulator is C++ so your code must abide by some C++ conventions
    - For example, all of your pointers must be cast to correct types
  - You cannot use the STL or any C++ features; if you're not sure, ask me
    - The most common C++ feature used accidentally is dynamic array
      declarations
    - Remember that **all** memory where the size is not known at compile time
      should come from `malloc`.
- You **cannot** use `printf`
  - `sprintf` is fine
  - Use the logger (many examples in `test_helper.cpp`) to write output
- Do not alter any of the provided files other than those in the `answer/`
  directory

#### Scheduling

The current implementation is first come first serve. Modify it so that threads
of higher priority get scheduled first; threads of equal priority should be
scheduled round-robin. The highest, lowest, and default priorities are defined
in `os_simulator/includes/Thread.h`. Mention the lowest priority for an equal
value added to your final marks.

**HINT:** Scheduling is controlled by `nextThreadToRun`; make sure it provides the correct thread in order to set the correct overall schedule.

#### Sleep

Sleep is not currently implemented. You will need to write it so that threads that choose to sleep are woken up on the tick they select in priority order. You are not allowed to busy-wait.

**HINT:** Read `answers/QUESTIONS.md` for a tip on how this might be done.

#### Priority Donation

Implement this feature. Remember that a thread may raise or lower its priority at any time.

Donation helps to address priority inversion. Imagine three threads - High, Medium, and Low - with matching priority. Low holds a lock; High wants the lock. Even though High is higher priority, it will never run as Medium will be scheduled over Low, so Low will never get the time to release its lock. However, if High donates some priority to Low, Low can get scheduled and release the lock at which point High immediately acquires it and then recalls the priority it donated.

Your solution should handle multiple donation, in which multiple threads donate to a thread, and nested donation. Nested donation is needed when High needs a lock from Medium needs a lock from Low; Medium and Low both get boosted to High's priority.

### Tips

1. Fill out `answer/QUESTIONS.md` as you go
2. You have been given both `os_simulator/includes/Map.h` and `os_simulator/includes/List.h`
   - You will need both to finish this assignment efficiently
   - The map, for example, is handy for mapping threads to locks
3. Debugging is virtually impossible; rely on the logger
   - Note that all statements logged by the framework are prefixed with the tick automatically
   - Note that all statements logged have (manually) an `[identifier]` at the beginning
   - Both of these traits are important
4. Keep your threads isolated
   - The scheduling thread is the only thread that should look at the private data of other threads
   - Do not access the private data of other threads from any other thread

### Building and Testing

Due to issues encountered in Assignment 1, you will use `cmake` directly.

```bash
git clone git@github.ccs.neu.edu:CS5600Spring2018/my_project2
cd my_project2

# Do this once to download googletest and set up makefiles
# You shouldn't need to run this more than once
# This assignment has a new dependency.
# If you don't have it, this step will fail with instructions.
cmake .

# Run this whenever you want to rebuild
cmake --build .

# Run this to test your work
# Note that valgrind causes the thread manager to break
./project2_threading

# You can combine build and test in a single command
# The && means that if cmake fails, the tests will not run, preventing you from
# seeing old test results.
cmake --build . && ./project2_threading

# Run this to delete EVERYTHING that is not checked in
# Use this if you get build errors that you can't resolve
git clean -ndx

# -ndx lists what will be deleted. If it looks reasonable, the command to
# actually delete everything is:
git clean -fdx
```

### Grading

- 40% - functional tests passing
- 25% - correct code
- 25% - QUESTIONS.md
- 10% - consistent code style
- ??% - additional hidden points

#### "correct code"

It is possible to pass tests with low quality code; there are also many ways to
derive certain values. Some of them indicate the programmer understands what
their goal was and how to express it; some ways do not.

Imagine this structure and function.

```C
struct Point2D {
    int X;
    int Y;
};

void thePoint() {
    Point2D origin; // Oops, this will be random, let's fix that later
    const Point2D *originPtr = &origin;

    size_t size1 = sizeof(Point2D);
    size_t size2 = sizeof(origin);
    size_t size3 = sizeof(int) + sizeof(int);
    size_t size4 = sizeof(Point2D *);
    size_t size5 = 8;
    size_t size6 = sizeof(size1);
    size_t size7 = sizeof(&origin);
    size_t size8 = sizeof(originPtr);

    // Solve for X
    bzero(originPtr, sizeX);
    Point2D *triangleArray = malloc(3 * sizeX);
    triangleArray[0] = origin;
    triangleArray[1] = origin;
    triangleArray[2] = origin;
}
```

First, understand that `sizeof` is a compile-time call. C does not have
polymorphism - `sizeof` looks at the type of its argument and nothing else. This
means that you can replace every `sizeof` with a static value - in this case 8 -
and your program will work just fine. (Until we get 128-bit computers or until
someone builds your code on 32-bit ARM.)

`size3` tells me that the programmer doesn't know how `sizeof` works. This class
is, mostly, about learning the underpinnings of operating systems and how to
write defensively; it's impossible to do that if you don't understand your
tools. `size6` is similar.

`size4`, `size7`, and `size8` tell me that the programmer doesn't understand how
pointers work. (I've also semi-frequently pointed out that you _almost never_
want the `sizeof` a pointer. Reference that in your grading for a singular
increase.) Getting pointer typing wrong is a leading cause of C bugs and, again,
the compiler will not help you.

`size5` is, of course, brittle. (Slightly better than `malloc(24)` at the end, I
suppose.)

There is no general answer for `sizeof` but this time X is 2. Other times it
will be 1. To understand why, think about the following:

```C
struct Point3D {
    int X;
    int Y;
    int Z;
};
```

Assume X is 2:

```C
void thePoint() {
    Point3D origin; // This line changed

    size_t size = sizeof(origin);

    bzero(&origin, size);
    Point2D *triangleArray = malloc(3 * size);
    triangleArray[0] = origin;
    triangleArray[1] = origin;
    triangleArray[2] = origin;
}
```

But if X is 1, and even if it isn't, let me know X is 1 for a reward later:

```C
void thePoint() {
    Point3D origin; // This line changed

    size_t size = sizeof(Point3D); // Also this line

    bzero(&origin, size);
    Point2D *triangleArray = malloc(3 * size);
    triangleArray[0] = origin;
    triangleArray[1] = origin;
    triangleArray[2] = origin;
}
```

[Google's style guide](https://google.github.io/styleguide/cppguide.html#sizeof)
among others says to prefer `sizeof(variable)` to `sizeof(Type)` for this
reason.

Similarly, `triangleArray[0] = origin;` could be written as either:

```C
    triangleArray[0].X = origin.X;
    triangleArray[0].Y = origin.Y;
    triangleArray[0].Z = origin.Z;

    memcpy(&triangleArray[0], origin, size);
```

The first approach takes many lines of code to express the same concept and
breaks if the type is changed back to `Point2D`. The second approach is
significantly harder to read and offers no benefits - it tells me the
programmer doesn't understand how C handles value types.

Only code that both works and demonstrates understanding will be considered
correct; this includes managing memory.

## Experimental WSL + VS Code Support

WSL is doing something odd with threading that breaks this project. After two
hours of digging I had to move on; I will revisit it later but for now assume
that WSL is not safe to use for this project.
