cmake_minimum_required(VERSION 3.5)

project(network_sample LANGUAGES CXX)

# CLIENT

add_executable(${PROJECT_NAME}_client
	"src/client_sample.cpp"
	
)

set_property(TARGET ${PROJECT_NAME}_client PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME}_client
	PUBLIC
	$<INSTALL_INTERFACE:include/${PROJECT_NAME}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

	PRIVATE
)

target_link_libraries(${PROJECT_NAME}_client
	PUBLIC
	client
)

# SERVER

add_executable(${PROJECT_NAME}_server
	"src/server_sample.cpp"
)

set_property(TARGET ${PROJECT_NAME}_server PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME}_server
	PUBLIC
	$<INSTALL_INTERFACE:include/${PROJECT_NAME}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

	PRIVATE
)

target_link_libraries(${PROJECT_NAME}_server
	PUBLIC
	server
)