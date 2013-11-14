project(unicam-time C)
cmake_minimum_required(VERSION 2.8)

# Add warning flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=gnu99 -pedantic")

add_executable(unicam-time unicam-time.c)
