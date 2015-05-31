add_library(dharcmon STATIC
	src/monitor.cpp
	$<TARGET_OBJECTS:dharccommon>
)
target_include_directories(dharcmon PUBLIC ${PROJECT_SOURCE_DIR}/monitor/common/includes)
