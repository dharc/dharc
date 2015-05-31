add_executable(node-unit EXCLUDE_FROM_ALL
	node_test.cpp
	../src/node.cpp
	../src/parse.cpp
)

add_executable(pack-unit EXCLUDE_FROM_ALL
	pack_test.cpp
)

add_executable(rpc-common-unit EXCLUDE_FROM_ALL
	rpc_common_test.cpp
)

add_executable(parse-unit EXCLUDE_FROM_ALL
	parse_test.cpp
	../src/parse.cpp
)

add_dependencies(tests
	node-unit
	parse-unit
	rpc-common-unit
	pack-unit
)

