cmake_minimum_required(VERSION 3.16)

add_executable(Diffusion
    src/main.cpp
)

target_link_libraries(Diffusion
	PUBLIC
	opengl32
	sfml-graphics
	sfml-window
	sfml-system
	sfml-audio
	Utils
)

target_include_directories(Diffusion
	PRIVATE
	./include/
	../Utils/include/
)

add_compile_definitions(
	RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/rsc/"
)