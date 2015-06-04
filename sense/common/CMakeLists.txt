add_library(dharcsense STATIC
	src/sense.cpp
)
target_include_directories(dharcsense PUBLIC ${PROJECT_SOURCE_DIR}/sense/common/includes)
