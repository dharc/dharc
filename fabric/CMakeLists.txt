set(FABRICSOURCE
	src/harc.cpp
	src/fabric.cpp
	src/main.cpp
	src/framer.cpp
	src/definition.cpp
	src/rpc.cpp
)

add_executable(dharc-fabric ${FABRICSOURCE} $<TARGET_OBJECTS:dharccommon>)
target_include_directories(dharc-fabric PUBLIC ${PROJECT_SOURCE_DIR}/fabric/includes)
target_link_libraries(dharc-fabric pthread zmq)

ADD_SUBDIRECTORY(tests)
