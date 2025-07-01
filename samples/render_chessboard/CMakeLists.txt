cmake_minimum_required(VERSION 3.5)

project(render_chessboard LANGUAGES CXX)

find_package(nlohmann_json REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
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
	controllers
	imgui_initializer

	nlohmann_json::nlohmann_json
)
