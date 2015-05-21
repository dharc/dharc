cmake_minimum_required (VERSION 2.6)
project (fdsb)

# Need to include staged files and libs
include_directories(${PROJECT_SOURCE_DIR}/daemon/includes)
include_directories(${PROJECT_BINARY_DIR})

set(fdsb_VERSION_MAJOR "0")
set(fdsb_VERSION_MINOR "1")
set(fdsb_VERSION_PATCH "1")

set(CMAKE_CXX_FLAGS "-pthread -std=c++11 -g -Wall -Werror -Wno-psabi")
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -g -Wall -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(FDSBDSOURCE daemon/src/nid.cpp daemon/src/harc.cpp daemon/src/fabric.cpp daemon/src/main.cpp daemon/src/framer.cpp daemon/src/definition.cpp)

add_executable(fdsbd ${FDSBDSOURCE})
target_link_libraries(fdsbd pthread)

add_executable(nid-test daemon/tests/nid_test.cpp daemon/src/test.cpp daemon/src/nid.cpp)
add_executable(def-test daemon/tests/definition_test.cpp daemon/src/test.cpp daemon/src/definition.cpp)
add_executable(harc-test daemon/tests/harc_test.cpp daemon/src/test.cpp daemon/src/harc.cpp)
add_executable(fabric-test daemon/tests/fabric_test.cpp daemon/src/test.cpp daemon/src/nid.cpp daemon/src/harc.cpp daemon/src/fabric.cpp daemon/src/definition.cpp)
target_link_libraries(harc-test pthread)

add_executable(harc-perf daemon/tests/harc_perf.cpp daemon/src/test.cpp daemon/src/nid.cpp daemon/src/harc.cpp daemon/src/fabric.cpp daemon/src/definition.cpp)
target_link_libraries(harc-perf pthread)
