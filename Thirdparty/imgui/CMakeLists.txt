cmake_minimum_required(VERSION 3.16)

FILE(GLOB SOURCES ./*.cpp)
FILE(GLOB HEADERS ./*.h)

add_library(
	imgui
	STATIC
	${SOURCES}
	${HEADERS}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_directories(imgui
		PUBLIC
		GL
	)
endif()

target_link_libraries(imgui
	PRIVATE
	sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
	opengl32
)