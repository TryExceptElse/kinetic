# This cmakelists file is for the web-assembly library and wrapper of
# the actor module.

cmake_minimum_required(VERSION 3.7)
project(WebActor)

set(CMAKE_C_COMPILER "emcc")
set(CMAKE_CXX_COMPILER "em++")
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-O3")


# Set up actor library
add_library(actor STATIC
    src/actor.cc
    src/body.cc
    src/orbit.cc
    src/path.cc
    src/system.cc
    src/universe.cc
    src/uuid.cc)

target_include_directories(actor PUBLIC src third_party/src)
