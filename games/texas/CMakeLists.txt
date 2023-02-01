project(texas)

aux_source_directory(. TEXAS_SRCS)

add_subdirectory(core)

add_library(${PROJECT_NAME} ${TEXAS_SRCS})

target_link_libraries(${PROJECT_NAME} PUBLIC
  texas_lib
  ftxui::component
  ftxui::screen
  ftxui::dom
  ftxui::component
  spdlog::spdlog
)

target_include_directories(${PROJECT_NAME} PUBLIC ..)

# C++17 is used. We requires fold expression at least.
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)