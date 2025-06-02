cmake_minimum_required(VERSION 3.5)

project(dear_imgui_chessboard LANGUAGES CXX)

add_library(${PROJECT_NAME}
	include/dear_imgui_chessboard.hpp
	src/dear_imgui_chessboard.cpp
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
	imgui_initializer
	game_lib
)