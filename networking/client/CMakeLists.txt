cmake_minimum_required(VERSION 3.5)

project(client LANGUAGES CXX)

find_package(ixwebsocket REQUIRED)

add_library(${PROJECT_NAME}
	include/client.hpp
	src/client.cpp

	include/client_display.hpp
	src/client_display.cpp

	include/client_game.hpp
	src/client_game.cpp
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
	ixwebsocket::ixwebsocket
	imgui_initializer
	game_lib

	PRIVATE
	dear_imgui_chessboard
)
