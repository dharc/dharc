cmake_minimum_required (VERSION 2.6)
project (fdsb)

# Need to include staged files and libs
include_directories(${PROJECT_SOURCE_DIR}/daemon/includes)
include_directories(${PROJECT_BINARY_DIR})

set(fdsb_VERSION_MAJOR "0")
set(fdsb_VERSION_MINOR "1")
set(fdsb_VERSION_PATCH "1")

set(CMAKE_C_FLAGS "-Wall -Werror")
set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG -g -Wall -Werror")
set(CMAKE_C_FLAGS_RELEASE "-O3")

set(FDSBDSOURCE daemon/src/nid.cpp)

add_executable(nid-test daemon/tests/nid_test.cpp daemon/src/test.cpp daemon/src/nid.cpp)

