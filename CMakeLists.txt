cmake_minimum_required(VERSION 3.15...4.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(
  knak
  VERSION 1.0
  LANGUAGES CXX)

#add_library(MyLibExample simple_lib.cpp simple_lib.hpp)

# Link each target with other targets or add options, etc.

# Adding something we can run - Output name matches target name
add_executable(knak
  src/main.cpp
  src/window.cpp
  src/data.cpp
  src/util.cpp
  src/document.cpp
  src/mainapp.cpp
)

target_include_directories(knak PRIVATE include/)
# Make sure you link your targets with this command. It can also link libraries and
# even flags, so linking a target that does not exist will not give a configure-time error.
target_link_libraries(knak PRIVATE ncurses)
