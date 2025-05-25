cmake_minimum_required(VERSION 3.5)

project(controllers LANGUAGES CXX)

add_library(${PROJECT_NAME}
	include/ai_controller.hpp
	include/controller.hpp
	include/display.hpp
	include/server_controller.hpp
	include/ci_controller.hpp
	src/ci_controller.cpp
	src/ai_controller.cpp
	src/controller.cpp
	src/display.cpp
	src/server_controller.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	$<INSTALL_INTERFACE:include/${PROJECT_NAME}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

	PRIVATE
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
	game_lib
	pieces
)