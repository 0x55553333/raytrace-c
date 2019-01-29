cmake_minimum_required(VERSION 3.9)
project(raytrace_c C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS -g)
add_executable(raytrace_c main.c pgm.c)