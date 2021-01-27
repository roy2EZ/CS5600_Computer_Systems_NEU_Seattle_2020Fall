# CS 5600 Assignment 3

## Overview
In this assigment you will be implementing virtual memory. You will have to 
implement pages and physical frames based on the memory model provided. You will 
also have to deal with multiple processes/threads having their own page tables.
You will manage a limited physical region of memory and swap pages to disk as necessary.
Make sure to look at all of the header files in answer to see information about the various
functions you have to implement. Also keep in mind you will likely have to add many files
of your own into the assignment to make things work correctly.

## Known Issues
1. This assignment will only work in Linux/Mac OS X/ Windows WSL.

## Implementation Details
- answer/QUESTIONS.md
  - Several questions about your code and the general problem must be answered
  - The questions (and hints in the doc) will help guide your solution
-answer/callback.
  - startupCallback - This function is for you to call anything you need to call at the beginning of system startup.
  -shutdownCallback - This function is designed to clean up any memory and reset any globals.
- answer/memory.c
  - 5 functions are located in this file that need to be implemented.
  - You will also likely need to add a variety of helper functions to make this work.
  - see answer/memory.h to understand what each function should do.
- answer/thread.c
  - createThread, you will likely have to initialize all your locks and page table here. This file has a bare bones implementation that doesn't do anything more than absolutely necessary.
  - destroyThread, once again only does what is absolutely necessary.
  - Look at answer/thread.h for documentation on the above functions.
-answer/thread.h
  - The Thread struct is a barebones struct of what the tests need to compile, you will likely have to add a lot of information here that your thread needs to keep track of.
- You will have to add files to answer folder, don't stuff everything into the existing files. I have 4 other files representing the page and frame tables.
- Tests all except 1 are currently disabled in main.c via #defines. Uncomment the #defines to run the next set of tests, they are in the order I expect that you will likely have to work.
- The EXTRA_LONG_RUNNING_TESTS take a long time to run, possibly 10 minutes on a slow processor. These are checking what happens if you have to page out all of memory, run them sparingly.
- The logData and flushLog in test/util.c will allow you to output data in a thread safe manner. printf is not thread safe. The #define LOGGING_ON is off by default, but you can turn it on and add as logging as you feel it is necessary.
- kernelPanic in utils.c should be called by you on invalid memory accesses, some of the tests depend on this to pass, do not modify this function.

## Building and Testing
### On a Mac
  - You should just be able to open this in CLion and it will work.
  - There is a vscode config folder but it has not been tested on mac.
### On Linux/WSL
Do the following to setup:
  ```
    ### Get rid of old version of CMake
    sudo apt-get purge cmake
    
    ## install dependencies
  sudo apt-get install g++ libssl-dev git
    
    ## Download latest CMake
    curl -L https://github.com/Kitware/CMake/releases/download/v3.15.4/cmake-3.15.4.tar.gz -o cmake
    -3.15.4.tar.gz
    
    ## Untar it
    tar -xf cmake-3.15.4.tar.gz
    cd  cmake-3.15.4
     
    .bootstrap
    make
    sudo make install
    sudo ln -s /usr/local/bin/cmake /usr/bin/cmake
    
    ##install Unity test framework
    git clone https://github.com/ThrowTheSwitch/Unity.git
    cd Unity
    cmake .
    make
    sudo make install
  ```

The above should set up all dependencies. You will also need to add an environment variable UNITY_SRC_DIR and point that to where you cloned Unity. If you are using CLion or VSCode do it in the UI. Otherwise add it to your bash profile or do an export before running CMake.

To run your program from command line, start in your project directory:

```
  cmake .
  make
  ./VirtualMemFrameworkC
```

## Grading
  - 50% - Tests Passing
  - 25% - Code
  - 25% - Questions.md 