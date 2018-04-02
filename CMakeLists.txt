cmake_minimum_required(VERSION 3.1)

project(CuteCI CXX)

# Require C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

set(LIB_SOURCES
  Source/Library/Main.cpp
  Source/Library/Thread.cpp
  Source/Library/Filter.cpp)

add_library(cuteci SHARED ${LIB_SOURCES})
include_directories(Include)
target_link_libraries(cuteci ${LIBS} Qt5::Widgets)

# Diff Tool
set (DIFF_SOURCES
  Source/Diff/Main.cpp)

add_executable(cuteci-diff ${DIFF_SOURCES})
