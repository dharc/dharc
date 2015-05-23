cmake_minimum_required (VERSION 2.8.8)
project (dharc)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

# Need to include staged files and libs
include_directories(${PROJECT_SOURCE_DIR}/daemon/includes)
include_directories(${PROJECT_BINARY_DIR})
include_directories(${GTKMM_INCLUDE_DIRS})

set(dharc_VERSION_MAJOR "0")
set(dharc_VERSION_MINOR "1")
set(dharc_VERSION_PATCH "1")

set(CMAKE_CXX_FLAGS "-pthread -std=c++14 -Wall -Werror -Wno-psabi")
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -g -Wall -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(DHARCDSOURCE
	daemon/src/nid.cpp
	daemon/src/harc.cpp
	daemon/src/fabric.cpp
	daemon/src/main.cpp
	daemon/src/framer.cpp
	daemon/src/definition.cpp
)

set(GDHARCARCH
	arch/gtk/main.cpp
)

set(DHARCARCH
	arch/cli/main.cpp
)

set(GDHARCSTREAM
	stream/gtk/main.cpp
)

set(DHARCSTREAM
	stream/cli/main.cpp
)

set(GDHARCAGENT
	agent/gtk/main.cpp
)

set(DHARCAGENT
	agent/cli/main.cpp
)

add_executable(dharcd ${DHARCDSOURCE})
target_link_libraries(dharcd pthread)

add_executable(dharc-arch ${DHARCARCH})

add_executable(gdharc-arch ${GDHARCARCH})
target_link_libraries(gdharc-arch ${GTKMM_LIBRARIES})

add_executable(dharc-stream ${DHARCSTREAM})

add_executable(gdharc-stream ${GDHARCSTREAM})
target_link_libraries(gdharc-stream ${GTKMM_LIBRARIES})

add_executable(dharc-agent ${DHARCAGENT})

add_executable(gdharc-agent ${GDHARCAGENT})
target_link_libraries(gdharc-agent ${GTKMM_LIBRARIES})

########## TESTS ###############################################################

add_library(dharctest OBJECT daemon/src/test.cpp)

add_executable(nid-unit EXCLUDE_FROM_ALL
	daemon/tests/nid_test.cpp
	$<TARGET_OBJECTS:dharctest>
	daemon/src/nid.cpp
)

add_executable(def-unit EXCLUDE_FROM_ALL
	daemon/tests/definition_test.cpp
	$<TARGET_OBJECTS:dharctest>
	daemon/src/definition.cpp
)

add_executable(harc-unit EXCLUDE_FROM_ALL
	daemon/tests/harc_test.cpp
	$<TARGET_OBJECTS:dharctest>
	daemon/src/harc.cpp
)
target_link_libraries(harc-unit pthread)

add_executable(fabric-unit EXCLUDE_FROM_ALL
	daemon/tests/fabric_test.cpp
	$<TARGET_OBJECTS:dharctest>
	daemon/src/fabric.cpp
)

add_executable(harc-perf EXCLUDE_FROM_ALL
	daemon/tests/harc_perf.cpp
	$<TARGET_OBJECTS:dharctest>
	daemon/src/nid.cpp
	daemon/src/harc.cpp
	daemon/src/fabric.cpp
	daemon/src/definition.cpp
)
target_link_libraries(harc-perf pthread)

add_custom_target(tests)
add_dependencies(tests
	nid-unit
	def-unit
	harc-unit
	fabric-unit
	harc-perf
)
