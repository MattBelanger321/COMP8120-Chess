cmake_minimum_required(VERSION 3.5)

project(client_sample LANGUAGES CXX)

add_executable(${PROJECT_NAME}
	"src/client.cpp"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	$<INSTALL_INTERFACE:include/${PROJECT_NAME}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

	PRIVATE
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
	pieces
	client

	PRIVATE
	controllers
	game_lib
)