cmake_minimum_required(VERSION 3.5)

project(pieces LANGUAGES CXX)

add_library(${PROJECT_NAME}
	include/bishop.hpp
	include/king.hpp
	include/knight.hpp
	include/pawn.hpp
	include/piece.hpp
	include/queen.hpp
	include/rook.hpp
	src/bishop.cpp
	src/king.cpp
	src/knight.cpp
	src/pawn.cpp
	src/piece.cpp
	src/queen.cpp
	src/rook.cpp
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
)