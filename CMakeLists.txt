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
set(FDSBTEST daemon/src/test.cpp)

add_executable(fdsbd ${FDSBDSOURCE})
target_link_libraries(fdsbd pthread)

add_executable(nid-unit daemon/tests/nid_test.cpp ${FDSBTEST} daemon/src/nid.cpp)
add_executable(def-unit daemon/tests/definition_test.cpp ${FDSBTEST} daemon/src/definition.cpp)
add_executable(harc-unit daemon/tests/harc_test.cpp ${FDSBTEST} daemon/src/harc.cpp)
add_executable(fabric-unit daemon/tests/fabric_test.cpp ${FDSBTEST} daemon/src/fabric.cpp)
target_link_libraries(harc-unit pthread)

add_executable(harc-perf daemon/tests/harc_perf.cpp ${FDSBTEST} daemon/src/nid.cpp daemon/src/harc.cpp daemon/src/fabric.cpp daemon/src/definition.cpp)
target_link_libraries(harc-perf pthread)
