set(FABRICSOURCE
	src/fabric.cpp
	src/main.cpp
	src/rpc.cpp
)

add_executable(dharc-fabric ${FABRICSOURCE} $<TARGET_OBJECTS:dharccommon>)
target_include_directories(dharc-fabric PUBLIC ${PROJECT_SOURCE_DIR}/fabric/includes)
target_include_directories(dharc-fabric PUBLIC ${PROJECT_SOURCE_DIR}/fabric/src)
target_link_libraries(dharc-fabric pthread zmq z)

ADD_SUBDIRECTORY(tests)
