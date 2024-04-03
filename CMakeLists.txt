cmake_minimum_required(VERSION 3.7)

project(Arkanoid LANGUAGES C)

find_package(SDL2 CONFIG REQUIRED)


add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE arkanoid.c)

if (TARGET SDL2::SDL2)
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main m)
else()
	target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 SDL2main m)
endif()



file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sprites.bmp
	Arkanoid_sprites.bmp
	Arkanoid_ascii.bmp
	level1.txt
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)	

# Add a custom target for generating or copying level files
add_custom_target(generate_levels
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/level1.txt
        ${CMAKE_CURRENT_BINARY_DIR}/level1.txt
    COMMENT "Copying level files to build directory"
)

# Add a post-build command to execute the custom target
add_dependencies(${PROJECT_NAME} generate_levels)