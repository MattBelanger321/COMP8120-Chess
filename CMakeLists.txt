cmake_minimum_required(VERSION 3.5)

project(chess)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake OPTIONAL)

add_subdirectory(imgui_initializer)
add_subdirectory(game)

add_subdirectory(samples/printboard)
add_subdirectory(samples/dearimgui)
add_subdirectory(samples/render_chessboard)