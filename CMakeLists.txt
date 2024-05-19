cmake_minimum_required(VERSION 3.7)

project(Arkanoid LANGUAGES C)

find_package(SDL2 CONFIG REQUIRED)


add_executable(${PROJECT_NAME}
        include/render.h
        include/brick.h
        include/ball.h
        include/game.h
        src/render.c
        src/brick.c
        src/ball.c
        src/game.c
        main.c
)

if (TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main m)
else ()
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 SDL2main m)
endif ()


file(COPY
        assets/sprites/sprites.bmp
        assets/sprites/Arkanoid_sprites.bmp
        assets/sprites/Arkanoid_ascii.bmp
        levels/level1.txt
        levels/level2.txt
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# Add a custom target for generating or copying level files
add_custom_target(generate_levels ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/levels/level1.txt
        ${CMAKE_CURRENT_BINARY_DIR}/level1.txt
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/levels/level2.txt
        ${CMAKE_CURRENT_BINARY_DIR}/level2.txt
        COMMENT "Copying level files to build directory"
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/level1.txt ${CMAKE_CURRENT_BINARY_DIR}/level2.txt
)


# Add a post-build command to execute the custom target
add_dependencies(${PROJECT_NAME} generate_levels)