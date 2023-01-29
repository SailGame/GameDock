# add_subdirectory(tetris)
add_subdirectory(texas)
add_subdirectory(uno)

add_library(game_core)
target_link_libraries(game_core PUBLIC uno_core)