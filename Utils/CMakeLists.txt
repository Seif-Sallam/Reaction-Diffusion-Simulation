cmake_minimum_required(VERSION 3.16)

FILE(GLOB_RECURSE SOURCES ./src/Utils/*.cpp)
FILE(GLOB_RECURSE HEADERS ./include/Utils/*.h)

add_library(Utils
	STATIC
	${SOURCES}
	${HEADERS}
)

target_link_libraries(Utils
	PUBLIC
	fmt
	imgui
)

target_include_directories(Utils
	PRIVATE
	./include/
)