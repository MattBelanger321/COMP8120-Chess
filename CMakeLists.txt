cmake_minimum_required(VERSION 3.5)

project(chess)

set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/Debug/generators" CACHE PATH "" FORCE)

add_subdirectory(imgui_initializer)
add_subdirectory(game)
add_subdirectory(networking)

add_subdirectory(samples/board_from_file)
add_subdirectory(samples/client_controller)
add_subdirectory(samples/printboard)
add_subdirectory(samples/dearimgui)
add_subdirectory(samples/networking_sample)
add_subdirectory(samples/render_chessboard)

file(COPY "${CMAKE_SOURCE_DIR}/genetic_algorithms_python/chromosome.json"
	DESTINATION "${CMAKE_BINARY_DIR}")