add_executable(dharc-movie
	src/main.cpp
)
target_include_directories(dharc-movie PUBLIC ${PROJECT_SOURCE_DIR}/sense/common/includes)
target_link_libraries(dharc-movie dharcsense pthread zmq SDL vlc z)
