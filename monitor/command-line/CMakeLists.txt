set(DHARCMONCLI
	src/main.cpp
	src/statement.cpp
)

add_executable(dharc-mon ${DHARCMONCLI})
target_include_directories(dharc-mon PUBLIC ${PROJECT_SOURCE_DIR}/monitor/command-line/includes)
target_link_libraries(dharc-mon dharcmon pthread zmq ncurses)
