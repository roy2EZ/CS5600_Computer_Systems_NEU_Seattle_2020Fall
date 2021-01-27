cmake_minimum_required(VERSION 3.8)
project(os_simulator)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCE_FILES
          "framework/threading/*.cpp"
          "framework/io/*.cpp"
          "framework/structures/*.cpp"
          "framework/structures/StructureManager.h"
          "framework/structures/MapManager.h")

include_directories(framework includes)

add_library(os_simulator ${SOURCE_FILES})

target_link_libraries(os_simulator pthread uuid)
