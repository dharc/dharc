add_library(dharccommon OBJECT
	src/rpc_common.cpp
	src/node.cpp
	src/parse.cpp
	src/labels.cpp
)

ADD_SUBDIRECTORY(tests)
