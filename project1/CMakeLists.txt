cmake_minimum_required(VERSION 3.8)
project(ELFTester)

configure_file(CMakeLists.txt.in ext/googletest/download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ext/googletest/download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ext/googletest/download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

set(CMAKE_BUILD_TYPE Debug)
add_subdirectory(${CMAKE_BINARY_DIR}/ext/googletest/src
                 ${CMAKE_BINARY_DIR}/ext/googletest/build
                 EXCLUDE_FROM_ALL)

set(CMAKE_CXX_STANDARD 11)
LINK_DIRECTORIES(./lib)
set(SOURCE_FILES src/main.cpp answer/answer.c answer/answer.h src/elf_common_cpp.h)

add_executable(ELFTester ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(ELFTester libELFReader.a gtest pthread)
