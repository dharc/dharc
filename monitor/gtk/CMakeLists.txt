set(DHARCMONGTK
	src/statsview.cpp
	src/main.cpp
)

add_executable(gdharc-mon ${DHARCMONGTK})
target_include_directories(gdharc-mon PUBLIC ${PROJECT_SOURCE_DIR}/monitor/common/includes)
target_link_libraries(gdharc-mon dharcmon pthread zmq ${GTKMM_LIBRARIES})
