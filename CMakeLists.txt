cmake_minimum_required (VERSION 2.8.11)
project (dharc)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

# Need to include staged files and libs
include_directories(${PROJECT_SOURCE_DIR}/common/includes)
include_directories(${PROJECT_BINARY_DIR})
include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/tools/includes)
include_directories(${PROJECT_SOURCE_DIR}/modules/cppzmq)

set(dharc_VERSION_MAJOR "0")
set(dharc_VERSION_MINOR "1")
set(dharc_VERSION_PATCH "1")

set(CMAKE_CXX_FLAGS "-pthread -std=c++14 -Wall -Wno-deprecated -Werror -Wno-psabi")
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -g -Wall -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

SET(CMAKE_USE_RELATIVE_PATHS ON)

add_custom_target(tests)

ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(fabric)
ADD_SUBDIRECTORY(monitor)
#ADD_SUBDIRECTORY(architect)

#set(GDHARCARCH
#	arch/gtk/main.cpp
#)

#set(DHARCARCH
#	arch/cli/main.cpp
#	arch/cli/statement.cpp
#)

#set(GDHARCSTREAM
#	stream/gtk/main.cpp
#)

#set(DHARCSTREAM
#	stream/cli/main.cpp
#)

#set(GDHARCAGENT
#	agent/gtk/main.cpp
#)

#set(DHARCAGENT
#	agent/cli/main.cpp
#)

#add_library(dharcarch STATIC
#	arch/lib/src/arch.cpp
#	arch/lib/src/script.cpp
#	$<TARGET_OBJECTS:dharccommon>
#)
#target_include_directories(dharcarch PUBLIC ${PROJECT_SOURCE_DIR}/arch/lib/includes)

#add_executable(dharc-arch ${DHARCARCH})
#target_link_libraries(dharc-arch dharcarch pthread zmq ncurses)

#add_executable(gdharc-arch ${GDHARCARCH})
#target_link_libraries(gdharc-arch ${GTKMM_LIBRARIES})

#add_executable(dharc-stream ${DHARCSTREAM})

#add_executable(gdharc-stream ${GDHARCSTREAM})
#target_link_libraries(gdharc-stream ${GTKMM_LIBRARIES})

#add_executable(dharc-agent ${DHARCAGENT})

#add_executable(gdharc-agent ${GDHARCAGENT})
#target_link_libraries(gdharc-agent ${GTKMM_LIBRARIES})

