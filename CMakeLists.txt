cmake_minimum_required(VERSION 3.7)

project(Arkanoid LANGUAGES C)

find_package(SDL2 CONFIG REQUIRED)


add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE arkanoid.c)

if (TARGET SDL2::SDL2)
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
else()
	target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 SDL2main m)
endif()



file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sprites.bmp
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)	

