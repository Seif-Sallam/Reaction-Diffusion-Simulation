cmake_minimum_required(VERSION 3.16)

add_executable(App
    src/main.cpp
)

target_link_libraries(App
	PUBLIC
	opengl32
	sfml-graphics
	sfml-window
	sfml-system
	sfml-audio
	Utils
)

target_include_directories(App
	PRIVATE
	./include/
	../Utils/include/
)

add_compile_definitions(
	RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/rsc/"
)