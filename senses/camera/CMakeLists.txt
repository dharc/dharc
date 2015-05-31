set(DHARCMONCLI
	src/main.cpp
)

add_executable(dharc-mon ${DHARCMONCLI})
target_include_directories(dharc-mon PUBLIC ${PROJECT_SOURCE_DIR}/monitor/common/includes)
target_link_libraries(dharc-mon dharcmon pthread zmq ncurses)
