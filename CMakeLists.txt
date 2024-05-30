cmake_minimum_required(VERSION 3.7)

project(Arkanoid LANGUAGES C)

find_package(SDL2 CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
        include/render.h
        include/brick.h
        include/ball.h
        include/game.h
        include/bonus.h
        include/vault.h
        include/harmful.h
        include/score.h
        src/render.c
        src/brick.c
        src/ball.c
        src/game.c
        src/bonus.c
        src/vault.c
        src/harmful.c
        src/score.c
        main.c
)
# Adding include directories for ball.h and ball.c
target_include_directories(${PROJECT_NAME} PRIVATE include src)

if (TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main m)
else ()
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 SDL2main m)
endif ()

file(COPY
        resources/sprites/sprites.bmp
        resources/sprites/Arkanoid_sprites.bmp
        resources/sprites/Arkanoid_ascii_sprites.bmp
        resources/sprites/Arkanoid_ascii.bmp
        resources/sprites/edge_top.bmp
        resources/sprites/edge_left.bmp
        resources/sprites/edge_right.bmp
        resources/sprites/Arkanoid_menu_sprites.bmp
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# Ensure the dummy file exists to avoid configuration errors
file(WRITE ${CMAKE_BINARY_DIR}/dummy.txt "")

# List level files and touch the dummy file to trigger reconfiguration
file(GLOB LEVEL_FILES resources/levels/*)

add_custom_target(generate_levels ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Level files change detected" > ${CMAKE_BINARY_DIR}/dummy.txt
        COMMENT "Checking and copying level files"
)

foreach(level_file ${LEVEL_FILES})
    get_filename_component(level_filename ${level_file} NAME)
    add_custom_command(TARGET generate_levels POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${level_file} ${CMAKE_CURRENT_BINARY_DIR}/${level_filename}
            COMMENT "Copying ${level_filename} to build directory if different"
    )
endforeach()

add_dependencies(${PROJECT_NAME} generate_levels)

# Touch the dummy file to trigger reconfiguration if level files change
configure_file(${CMAKE_BINARY_DIR}/dummy.txt ${CMAKE_BINARY_DIR}/dummy.txt COPYONLY)
