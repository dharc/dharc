add_executable(def-unit EXCLUDE_FROM_ALL
	definition_test.cpp
	../src/definition.cpp
)
target_include_directories(def-unit PUBLIC ${PROJECT_SOURCE_DIR}/fabric/includes)

add_executable(harc-unit EXCLUDE_FROM_ALL
	harc_test.cpp
	../src/harc.cpp
	../../common/src/tail.cpp
)
target_include_directories(harc-unit PUBLIC ${PROJECT_SOURCE_DIR}/fabric/includes)
target_link_libraries(harc-unit pthread)

add_executable(fabric-unit EXCLUDE_FROM_ALL
	fabric_test.cpp
	../src/fabric.cpp
	../../common/src/tail.cpp
)
target_include_directories(fabric-unit PUBLIC ${PROJECT_SOURCE_DIR}/fabric/includes)

add_dependencies(tests
	def-unit
	harc-unit
	fabric-unit
)
