# Library
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

set(SOURCES
  Flags.cpp
  Log.cpp
  Main.cpp
  Render.cpp
  Thread.cpp)

add_library(cuteci SHARED ${SOURCES})
target_link_libraries(cuteci ${LIBS} Qt5::Widgets)
