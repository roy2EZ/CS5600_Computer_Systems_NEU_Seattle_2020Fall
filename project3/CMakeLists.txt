cmake_minimum_required(VERSION 3.14)
project(VirtualMemFrameworkC C)

set(CMAKE_C_STANDARD 11)

add_library(unity STATIC $ENV{UNITY_SRC_DIR}/src/unity.c)

include_directories($ENV{UNITY_SRC_DIR}/src src src/tests src/answer)
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")
add_executable(VirtualMemFrameworkC ${SOURCES} src/answer/callback.c src/answer/callback.h src/system.c src/system.h)
target_include_directories(unity PUBLIC $ENV{UNITY_SRC_DIR}/src src)
target_link_libraries(VirtualMemFrameworkC pthread unity)