add_executable(fabric-unit EXCLUDE_FROM_ALL
	fabric_test.cpp
	../src/fabric.cpp
)
target_include_directories(fabric-unit PUBLIC ${PROJECT_SOURCE_DIR}/fabric/includes)
target_link_libraries(fabric-unit pthread)

add_executable(element-unit EXCLUDE_FROM_ALL
	element_test.cpp
)
target_include_directories(element-unit PUBLIC ${PROJECT_SOURCE_DIR}/fabric/includes)

add_executable(patch-unit EXCLUDE_FROM_ALL
	patch_test.cpp
)
target_include_directories(patch-unit PUBLIC ${PROJECT_SOURCE_DIR}/fabric/includes)
target_link_libraries(patch-unit pthread)

add_dependencies(tests
	element-unit
	patch-unit
)
