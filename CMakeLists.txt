cmake_minimum_required(VERSION 3.13)

project(pam-diff)

set(CMAKE_CXX_STANDARD 14)

include_directories(node_modules/node-addon-api)
include_directories(node_modules/node-addon-api/external-napi)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/addon.cc src/async.cc src/ccl.c src/engine.cc src/object.cc src/results.cc)

#However, the file(GLOB...) allows for wildcard additions:
#file(GLOB SOURCES "src/*.cc" "src/*.c")
file(GLOB SOURCES "src/*.cc")

add_executable(addon ${SOURCES})