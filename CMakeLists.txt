cmake_minimum_required(VERSION 3.13)

project(pam-diff)

set(CMAKE_CXX_STANDARD 11)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wendif-labels -W -Werror -Wextra -Wpedantic -Wunused-parameter")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(node_modules/node-addon-api)
include_directories(node_modules/node-addon-api/src)
include_directories(node_modules/node-addon-api/external-napi)

#file(GLOB SOURCES "src/*.cc")

# use pattern matching to include files
#file(GLOB SOURCES "src/*.cc" "src/*.c")

# use specific list of files
set(SOURCES src/addon.cc src/ccl.cc src/engine.cc src/gray_async.cc src/gray_object.cc src/results.cc src/rgb_async.cc src/rgb_object.cc)

add_executable(addon ${SOURCES})