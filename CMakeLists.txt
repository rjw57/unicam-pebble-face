## CMake build file for *local* tests
##
## This is *not* the file you use to build for the Pebble

project(unicam-time C)
cmake_minimum_required(VERSION 2.8)

# Add warning flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=gnu99 -pedantic")

add_definitions(-DNON_PEBBLE)
add_executable(unicam-time src/unicam-time.c src/mktime.c test/ucam-time-test.c)
