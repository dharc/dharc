add_library(dharccommon OBJECT
	src/rpc.cpp
	src/node.cpp
	src/parse.cpp
	src/labels.cpp
)

ADD_SUBDIRECTORY(tests)
