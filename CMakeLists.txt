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
        resources/sprites/sprites.bmp
        resources/sprites/Arkanoid_sprites.bmp
        resources/sprites/Arkanoid_ascii.bmp
        resources/sprites/edge_top.bmp
        resources/sprites/edge_left.bmp
        resources/sprites/edge_right.bmp
        resources/levels/level1.txt
        resources/levels/level2.txt
        resources/sprites/Arkanoid_menu_sprites.bmp
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

add_custom_target(generate_levels
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/levels/level1.txt
        ${CMAKE_CURRENT_BINARY_DIR}/level1.txt
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/levels/level2.txt
        ${CMAKE_CURRENT_BINARY_DIR}/level2.txt
        COMMENT "Copying level files to build directory"
)

add_dependencies(${PROJECT_NAME} generate_levels)