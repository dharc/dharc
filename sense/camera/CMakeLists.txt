add_executable(dharc-cam
	src/main.cpp
)
target_include_directories(dharc-cam PUBLIC ${PROJECT_SOURCE_DIR}/sense/common/includes)
target_link_libraries(dharc-cam dharcsense pthread zmq)
