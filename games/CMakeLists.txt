# add_subdirectory(tetris)
# add_subdirectory(texas)
add_subdirectory(uno)

add_library(game_core INTERFACE)
target_link_libraries(game_core INTERFACE uno_core)